#include "vehicle_data_receiver.hpp"
#include <iostream>
#include <cstring>

VehicleDataReceiver::VehicleDataReceiver() : is_initialized_(false) {
}

VehicleDataReceiver::~VehicleDataReceiver() {
    if (app_) {
        stop();
    }
}

bool VehicleDataReceiver::initialize() {
    app_ = vsomeip::runtime::get()->create_application("Vehicle-Data-Client");
    if (!app_) {
        std::cerr << "Failed to create vsomeip application" << std::endl;
        return false;
    }
    
    if (!app_->init()) {
        std::cerr << "Failed to initialize vsomeip application" << std::endl;
        return false;
    }
    
    // Set up service availability handler
    app_->register_availability_handler(SERVICE_ID, INSTANCE_ID,
        std::bind(&VehicleDataReceiver::onServiceAvailable, this,
                  std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    
    // Set up message handler
    app_->register_message_handler(SERVICE_ID, INSTANCE_ID, vsomeip::ANY_METHOD,
        std::bind(&VehicleDataReceiver::onMessage, this, std::placeholders::_1));
    
    // Request the service
    app_->request_service(SERVICE_ID, INSTANCE_ID);
    
    is_initialized_ = true;
    return true;
}

void VehicleDataReceiver::start() {
    if (!is_initialized_) {
        std::cerr << "VehicleDataReceiver not initialized" << std::endl;
        return;
    }
    
    std::cout << "Starting SOME/IP client application..." << std::endl;
    app_->start();
}

void VehicleDataReceiver::stop() {
    if (app_) {
        std::cout << "Stopping SOME/IP client application..." << std::endl;
        app_->stop();
    }
}

void VehicleDataReceiver::onServiceAvailable(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available) {
    if (service == SERVICE_ID && instance == INSTANCE_ID) {
        if (is_available) {
            std::cout << "Vehicle data service is available, subscribing to events..." << std::endl;
            
            // Subscribe to all events
            app_->subscribe(SERVICE_ID, INSTANCE_ID, PRIMARY_VEHICLE_EVENT_ID);
            app_->subscribe(SERVICE_ID, INSTANCE_ID, SIGNAL_LIGHTING_EVENT_ID);
            app_->subscribe(SERVICE_ID, INSTANCE_ID, POWER_THERMAL_EVENT_ID);
            app_->subscribe(SERVICE_ID, INSTANCE_ID, DOOR_SECURITY_EVENT_ID);
        } else {
            std::cout << "Vehicle data service is no longer available" << std::endl;
        }
    }
}

void VehicleDataReceiver::onMessage(const std::shared_ptr<vsomeip::message>& message) {
    if (message->get_service() != SERVICE_ID || message->get_instance() != INSTANCE_ID) {
        return;
    }
    
    auto payload = message->get_payload();
    if (!payload) {
        return;
    }
    
    // Handle different event types
    switch (message->get_method()) {
        case PRIMARY_VEHICLE_EVENT_ID: {
            avisio_ivi::PrimaryVehicleState state;
            if (parseProtobufMessage(payload, state) && primary_vehicle_callback_) {
                primary_vehicle_callback_(state);
            }
            break;
        }
        
        case SIGNAL_LIGHTING_EVENT_ID: {
            avisio_ivi::SignalLightingStatus status;
            if (parseProtobufMessage(payload, status) && signal_lighting_callback_) {
                signal_lighting_callback_(status);
            }
            break;
        }
        
        case POWER_THERMAL_EVENT_ID: {
            avisio_ivi::PowerThermalStatus status;
            if (parseProtobufMessage(payload, status) && power_thermal_callback_) {
                power_thermal_callback_(status);
            }
            break;
        }
        
        case DOOR_SECURITY_EVENT_ID: {
            avisio_ivi::DoorSecurityStatus status;
            if (parseProtobufMessage(payload, status) && door_security_callback_) {
                door_security_callback_(status);
            }
            break;
        }
        
        default:
            std::cout << "Received unknown event ID: " << message->get_method() << std::endl;
            break;
    }
}

template<typename T>
bool VehicleDataReceiver::parseProtobufMessage(const std::shared_ptr<vsomeip::payload>& payload, T& message) {
    // Convert payload to string
    std::string serialized_data(reinterpret_cast<const char*>(payload->get_data()), payload->get_length());
    
    // Parse Protobuf message from string
    if (!message.ParseFromString(serialized_data)) {
        std::cerr << "Failed to parse Protobuf message" << std::endl;
        return false;
    }
    
    return true;
}

void VehicleDataReceiver::setPrimaryVehicleCallback(PrimaryVehicleProtobufCallback callback) {
    primary_vehicle_callback_ = callback;
}

void VehicleDataReceiver::setSignalLightingCallback(SignalLightingProtobufCallback callback) {
    signal_lighting_callback_ = callback;
}

void VehicleDataReceiver::setPowerThermalCallback(PowerThermalProtobufCallback callback) {
    power_thermal_callback_ = callback;
}

void VehicleDataReceiver::setDoorSecurityCallback(DoorSecurityProtobufCallback callback) {
    door_security_callback_ = callback;
}

