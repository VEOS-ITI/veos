#include "vehicle_data_sender.hpp"
#include <iostream>
#include <cstring>

VehicleDataSender::VehicleDataSender() : is_initialized_(false) {
}

VehicleDataSender::~VehicleDataSender() {
    if (app_) {
        stop();
    }
}

bool VehicleDataSender::initialize() {
    app_ = vsomeip::runtime::get()->create_application("Logitech-Sender");
    if (!app_) {
        std::cerr << "Failed to create vsomeip application" << std::endl;
        return false;
    }
    
    if (!app_->init()) {
        std::cerr << "Failed to initialize vsomeip application" << std::endl;
        return false;
    }
    
    // Offer all services
    app_->offer_service(SERVICE_ID, INSTANCE_ID);
    
    is_initialized_ = true;
    return true;
}

void VehicleDataSender::start() {
    if (!is_initialized_) {
        std::cerr << "VehicleDataSender not initialized" << std::endl;
        return;
    }
    
    std::cout << "Starting SOME/IP application..." << std::endl;
    app_->start();
}

void VehicleDataSender::stop() {
    if (app_) {
        std::cout << "Stopping SOME/IP application..." << std::endl;
        app_->stop();
    }
}

uint32_t VehicleDataSender::getCurrentTimestamp() {
    auto now = std::chrono::steady_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return static_cast<uint32_t>(millis);
}

template<typename T>
void VehicleDataSender::sendProtobufMessage(uint16_t event_id, const T& message) {
    if (!is_initialized_ || !app_) {
        return;
    }
    
    std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
    
    // Serialize Protobuf message to string
    std::string serialized_data;
    if (!message.SerializeToString(&serialized_data)) {
        std::cerr << "Failed to serialize Protobuf message" << std::endl;
        return;
    }
    
    // Convert string to byte vector
    std::vector<vsomeip::byte_t> data(serialized_data.begin(), serialized_data.end());
    
    payload->set_data(data);
    app_->notify(SERVICE_ID, INSTANCE_ID, event_id, payload);
}

void VehicleDataSender::sendPrimaryVehicleStateProtobuf(const avisio_ivi::PrimaryVehicleState& state) {
    sendProtobufMessage(PRIMARY_VEHICLE_EVENT_ID, state);
}

void VehicleDataSender::sendSignalLightingStatusProtobuf(const avisio_ivi::SignalLightingStatus& status) {
    sendProtobufMessage(SIGNAL_LIGHTING_EVENT_ID, status);
}

void VehicleDataSender::sendPowerThermalStatusProtobuf(const avisio_ivi::PowerThermalStatus& status) {
    sendProtobufMessage(POWER_THERMAL_EVENT_ID, status);
}

void VehicleDataSender::sendDoorSecurityStatusProtobuf(const avisio_ivi::DoorSecurityStatus& status) {
    sendProtobufMessage(DOOR_SECURITY_EVENT_ID, status);
}

