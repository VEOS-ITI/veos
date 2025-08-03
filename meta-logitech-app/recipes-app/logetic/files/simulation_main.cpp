#include <iostream>
#include <unordered_map>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>
#include <iomanip>


#include "vehicle_data_sender.hpp"
#include "avisio_ivi_data.pb.h" // Include generated Protobuf header

// --- The Simplified Virtual Car State (Automatic Transmission) ---
struct VirtualCar {
    double speed = 0.0;
    int current_gear = 0;
    double engine_rpm = 800.0;
    double idle_rpm = 800.0;
    double max_rpm = 7000.0;
    double gear_ratios[8] = {0.0, 3.5, 2.5, 1.8, 1.3, 1.0, 0.8, -3.0};
    double final_drive_ratio = 4.1;
    double wheel_radius = 0.3;
    double braking_rate = 80.0;
    double coasting_drag = 0.5;
};

// --- Normalization Function (Unchanged) ---
double normalize_pedal_input(short raw_value) {
    double inverted_value = -static_cast<double>(raw_value);
    double shifted_value = inverted_value + 32767.0;
    double normalized = shifted_value / 65534.0;
    if (normalized < 0.0) return 0.0;
    if (normalized > 1.0) return 1.0;
    return normalized;
}

// --- Joystick Reading Thread Function ---
void joystick_thread_func(const char* device_path, std::atomic<bool>& running) {
    std::unordered_map<Key, std::unique_ptr<Input>, KeyHash> inputs;
    // Define mappings for shifters and pedals
    inputs[{JS_EVENT_BUTTON, 12}] = std::make_unique<Button>("Shifter_Pos_1");
    inputs[{JS_EVENT_BUTTON, 13}] = std::make_unique<Button>("Shifter_Pos_2");
    inputs[{JS_EVENT_BUTTON, 14}] = std::make_unique<Button>("Shifter_Pos_3");
    inputs[{JS_EVENT_BUTTON, 15}] = std::make_unique<Button>("Shifter_Pos_4");
    inputs[{JS_EVENT_BUTTON, 16}] = std::make_unique<Button>("Shifter_Pos_5");
    inputs[{JS_EVENT_BUTTON, 17}] = std::make_unique<Button>("Shifter_Pos_6");
    inputs[{JS_EVENT_BUTTON, 18}] = std::make_unique<Button>("Shifter_Reverse");
    inputs[{JS_EVENT_AXIS, 2}] = std::make_unique<Axis>("Gas_Pedal");
    inputs[{JS_EVENT_AXIS, 3}] = std::make_unique<Axis>("Brake_Pedal");
    inputs[{JS_EVENT_AXIS, 0}] = std::make_unique<Axis>("Steering_Wheel");

    int fd = open(device_path, O_RDONLY);
    if (fd < 0) { running = false; return; }
    struct js_event event;
    while (running && read(fd, &event, sizeof(event)) == sizeof(event)) {
        event.type &= ~JS_EVENT_INIT;
        Key key{event.type, event.number};
        if (inputs.count(key)) { inputs.at(key)->process_event(event.value); }
    }
    close(fd);
}

// --- Main Application (Automatic Transmission Game Loop) ---
int main() {
    VirtualCar car;
    std::atomic<bool> is_running(true);

    // Initialize VehicleDataSender
    VehicleDataSender data_sender;
    if (!data_sender.initialize()) {
        std::cerr << "Failed to initialize VehicleDataSender" << std::endl;
        return 1;
    }

    // Start the SOME/IP application in a separate thread
    std::thread vsomeip_app_thread([&]() { data_sender.start(); });

    // Start the joystick reader on a separate thread
    std::thread joystick_thread(joystick_thread_func, "/dev/input/js0", std::ref(is_running));

    const std::chrono::milliseconds loop_interval(16); // ~60 FPS
    while (is_running) {
        auto loop_start_time = std::chrono::high_resolution_clock::now();
        double delta_time = 0.016;

        // 1. Gather Input
        double gas_pressure = normalize_pedal_input(g_gas_pedal_raw.load());
        double brake_pressure = normalize_pedal_input(g_brake_pedal_raw.load());
        int desired_gear = g_shifter_position.load();

        // 2. Update State
        if (car.current_gear != desired_gear) {
            car.current_gear = desired_gear;
        }

        if (gas_pressure > 0.05) {
            car.engine_rpm += (gas_pressure * 4000.0) * delta_time;
        } else {
            car.engine_rpm -= (2500.0) * delta_time;
        }
        if (car.engine_rpm < car.idle_rpm) car.engine_rpm = car.idle_rpm;
        if (car.engine_rpm > car.max_rpm) car.engine_rpm = car.max_rpm;

        double engine_force = 0.0;
        if (car.current_gear != 0) {
            double gear_ratio = (car.current_gear == -1) ? car.gear_ratios[7] : car.gear_ratios[car.current_gear];
            double wheel_rpm = car.engine_rpm / (gear_ratio * car.final_drive_ratio);
            double wheel_circumference = 2 * 3.14159 * car.wheel_radius;
            double target_speed_mps = (wheel_rpm * wheel_circumference) / 60.0;
            double target_speed_kmh = target_speed_mps * 3.6;
            engine_force = (target_speed_kmh - car.speed) * 2.5;
        }

        double brake_force = car.braking_rate * brake_pressure;
        double drag_force = car.speed * 0.1;

        car.speed += (engine_force - brake_force - drag_force) * delta_time;
        if (car.speed < 0) car.speed = 0;

        // 3. Send Data via SOME/IP using VehicleDataSender
        // Create a Protobuf message
        avisio_ivi::PrimaryVehicleState primary_state_pb;
        primary_state_pb.set_timestamp_ms(data_sender.getCurrentTimestamp());
        primary_state_pb.set_speed_kmh(car.speed);
        primary_state_pb.set_engine_rpm(car.engine_rpm);
        
        // Map current_gear to Protobuf enum
        switch (car.current_gear) {
            case 0: primary_state_pb.set_current_gear(avisio_ivi::GEAR_NEUTRAL); break;
            case 1: primary_state_pb.set_current_gear(avisio_ivi::GEAR_FIRST); break;
            case 2: primary_state_pb.set_current_gear(avisio_ivi::GEAR_SECOND); break;
            case 3: primary_state_pb.set_current_gear(avisio_ivi::GEAR_THIRD); break;
            case 4: primary_state_pb.set_current_gear(avisio_ivi::GEAR_FOURTH); break;
            case 5: primary_state_pb.set_current_gear(avisio_ivi::GEAR_FIFTH); break;
            case 6: primary_state_pb.set_current_gear(avisio_ivi::GEAR_SIXTH); break;
            case -1: primary_state_pb.set_current_gear(avisio_ivi::GEAR_REVERSE); break;
            default: primary_state_pb.set_current_gear(avisio_ivi::GEAR_NEUTRAL); break; // Default to neutral
        }
        
        primary_state_pb.set_gear_mode(avisio_ivi::GEAR_MODE_DRIVE); // Assuming drive mode for automatic
        primary_state_pb.set_fuel_level_millipercent(750); // Example value
        primary_state_pb.set_engine_temp_c(90); // Example value
        primary_state_pb.set_status_flags(0); // Example value
        primary_state_pb.set_odometer_km(12345);
        primary_state_pb.set_trip_distance_m(1500);

        data_sender.sendPrimaryVehicleStateProtobuf(primary_state_pb);

        // Also print to console for local debugging
        std::cout << "\r" << "Speed: " << std::fixed << std::setprecision(1) << car.speed << " km/h | "
                  << "Gear: " << (car.current_gear == 0 ? "N" : (car.current_gear == -1 ? "R" : std::to_string(car.current_gear))) << " | "
                  << "RPM: " << std::setprecision(0) << car.engine_rpm << "  " << std::flush;
        
        auto loop_end_time = std::chrono::high_resolution_clock::now();
        auto loop_duration = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end_time - loop_start_time);
        if (loop_duration < loop_interval) {
            std::this_thread::sleep_for(loop_interval - loop_duration);
        }
    }

    // Clean up
    is_running = false; // Signal joystick thread to stop
    joystick_thread.join();
    vsomeip_app_thread.join(); // Wait for vsomeip app thread to finish
    data_sender.stop(); // Stop vsomeip application via sender

    return 0;
}

