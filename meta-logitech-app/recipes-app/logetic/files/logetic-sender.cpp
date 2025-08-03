#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include <fcntl.h>
#include <unistd.h>
#include <linux/joystick.h>
#include <string.h>

#include <vsomeip/vsomeip.hpp>

#define DEVICE_PATH "/dev/input/js0"

// Define our SOME/IP Service and Event details
#define LOGITECH_SERVICE_ID 0x1111
#define LOGITECH_INSTANCE_ID 0x2222
#define LOGITECH_EVENT_ID 0x8001

// Create a shared pointer for the vsomeip objects
std::shared_ptr<vsomeip::runtime> runtime;
std::shared_ptr<vsomeip::application> app;

// Function to format the joystick data into a string payload
std::string format_joystick_event(const struct js_event& event) {
    std::stringstream ss;
    ss << "T:" << event.time << ",N:" << (int)event.number << ",V:" << event.value;
    if (event.type & JS_EVENT_BUTTON) {
        ss << ",Y:B";
    } else if (event.type & JS_EVENT_AXIS) {
        ss << ",Y:A";
    }
    return ss.str();
}

void run_joystick_reader() {
    std::cout << "Logitech Sender: Thread started for LOCAL TEST." << std::endl;
    
    // --- FAKE DATA LOOP FOR TESTING ---
    int test_value = 0;
    while (true) {
        // Create a fake data string
        std::stringstream ss;
        ss << "T:999,N:1,V:" << test_value++ << ",Y:A";
        std::string payload_str = ss.str();
        
        std::vector<vsomeip::byte_t> payload_data(payload_str.begin(), payload_str.end());
        std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
        payload->set_data(payload_data);

        app->notify(LOGITECH_SERVICE_ID, LOGITECH_INSTANCE_ID, LOGITECH_EVENT_ID, payload);
        std::cout << "Sent Fake Event: " << payload_str << std::endl;

        // Wait for one second
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


// This handler is called when the application's state changes
void on_state_change(vsomeip::state_type_e _state) {
    if (_state == vsomeip::state_type_e::ST_REGISTERED) {
        // Our application is now registered with the vsomeip daemon.
        // We can now safely offer our service.
        std::cout << "Logitech Sender: Application registered. Offering service." << std::endl;
        app->offer_service(LOGITECH_SERVICE_ID, LOGITECH_INSTANCE_ID);
    }
}

int main() {
    runtime = vsomeip::runtime::get();
    app = runtime->create_application("Logitech-Sender");
    
    if (!app->init()) {
        std::cerr << "Couldn't initialize vsomeip application" << std::endl;
        return 1;
    }

    // Register a state handler to know when we can offer the service
    app->register_state_handler(on_state_change);

    // Create and detach the joystick thread.
    // It will wait patiently until the device is ready.
    // The main thread will be blocked by app->start().
    std::thread joystick_thread(run_joystick_reader);

    // Start the vsomeip runtime. This is a blocking call.
    // It will only return when app->stop() is called from another thread.
    app->start();

    // Wait for the joystick thread to finish its execution before exiting main.
    joystick_thread.join();

    return 0;
}

