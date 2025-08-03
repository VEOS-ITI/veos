#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <string>
#include <vector>

#include <vsomeip/vsomeip.hpp>

// Define the Service and Event details (must match the sender)
#define LOGITECH_SERVICE_ID 0x1111
#define LOGITECH_INSTANCE_ID 0x2222
#define LOGITECH_EVENT_ID 0x8001

std::shared_ptr<vsomeip::application> app;

// --- Data Parsing Logic ---
// A struct to hold our parsed joystick data in a clean format
struct ParsedJoystickEvent {
    long time;
    int number;
    int value;
    char type; // 'B' for Button, 'A' for Axis
};

// This function takes the raw string from the sender and parses it
ParsedJoystickEvent parse_payload(const std::string& payload_str) {
    ParsedJoystickEvent p_event = {0, 0, 0, 'U'}; // Default values
    std::stringstream ss(payload_str);
    std::string segment;
    char dummy; // To consume characters like 'T:', ',', etc.

    while(std::getline(ss, segment, ',')) {
        std::stringstream segment_ss(segment);
        char key;
        segment_ss >> key >> dummy; // Read the key (T, N, V, Y) and the ':'

        switch(key) {
            case 'T':
                segment_ss >> p_event.time;
                break;
            case 'N':
                segment_ss >> p_event.number;
                break;
            case 'V':
                segment_ss >> p_event.value;
                break;
            case 'Y':
                segment_ss >> p_event.type;
                break;
        }
    }
    return p_event;
}
// --- End of Data Parsing Logic ---


void on_message(const std::shared_ptr<vsomeip::message> &_response) {
    // Convert the byte payload back to a string
    const vsomeip::byte_t *data = _response->get_payload()->get_data();
    size_t len = _response->get_payload()->get_length();
    std::string received_str(reinterpret_cast<const char*>(data), len);

    // --- 1. Show the raw string ---
    std::cout << "--------------------------------" << std::endl;
    std::cout << "Raw data string received: \"" << received_str << "\"" << std::endl;

    // --- 2. Parse the string into our struct ---
    ParsedJoystickEvent parsed_data = parse_payload(received_str);

    // --- 3. Print the clean, readable, formatted data ---
    std::cout << "Parsed Data:" << std::endl;
    std::cout << "  Timestamp: " << parsed_data.time << " ms" << std::endl;
    if (parsed_data.type == 'A') {
        std::cout << "  Type:      Axis" << std::endl;
        std::cout << "  Axis ID:   " << parsed_data.number << std::endl;
        std::cout << "  Value:     " << parsed_data.value << std::endl;
    } else if (parsed_data.type == 'B') {
        std::cout << "  Type:      Button" << std::endl;
        std::cout << "  Button ID: " << parsed_data.number << std::endl;
        std::cout << "  State:     " << (parsed_data.value == 1 ? "Pressed" : "Released") << std::endl;
    }
    std::cout << "--------------------------------" << std::endl << std::endl;
}

void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    if (LOGITECH_SERVICE_ID == _service && LOGITECH_INSTANCE_ID == _instance && _is_available) {
        std::cout << "Receiver: Logitech service is now AVAILABLE." << std::endl;
        // Service is available, let's request it and subscribe to the event
        app->request_service(_service, _instance);
        app->subscribe(_service, _instance, LOGITECH_EVENT_ID);
    } else {
        std::cout << "Receiver: Logitech service is now UNAVAILABLE." << std::endl;
    }
}

int main() {
    app = vsomeip::runtime::get()->create_application("Logitech-Receiver");
    if (!app->init()) {
        std::cerr << "Couldn't initialize vsomeip application" << std::endl;
        return 1;
    }

    // Register handlers
    app->register_availability_handler(LOGITECH_SERVICE_ID, LOGITECH_INSTANCE_ID, on_availability);
    app->register_message_handler(LOGITECH_SERVICE_ID, LOGITECH_INSTANCE_ID, vsomeip::ANY_METHOD, on_message);

    // Start the application. This is a blocking call.
    app->start();

    return 0;
}

