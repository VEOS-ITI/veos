#ifndef VEHICLE_DATA_SENDER_HPP
#define VEHICLE_DATA_SENDER_HPP

#include <memory>
#include <chrono>
#include <vsomeip/vsomeip.hpp>
#include "avisio_ivi_data.pb.h" // Include generated Protobuf header

// SOME/IP Service and Event Definitions
#define SERVICE_ID 0x1111
#define INSTANCE_ID 0x2222

// Event IDs for different data types
#define PRIMARY_VEHICLE_EVENT_ID 0x8001
#define SIGNAL_LIGHTING_EVENT_ID 0x8002
#define POWER_THERMAL_EVENT_ID 0x8003
#define DOOR_SECURITY_EVENT_ID 0x8004

class VehicleDataSender {
private:
    std::shared_ptr<vsomeip::application> app_;
    bool is_initialized_;
    
    // Send a generic Protobuf message via SOME/IP
    template<typename T>
    void sendProtobufMessage(uint16_t event_id, const T& message);

public:
    VehicleDataSender();
    ~VehicleDataSender();
    
    // Initialize the SOME/IP application
    bool initialize();
    
    // Start the SOME/IP application (blocking call)
    void start();
    
    // Stop the SOME/IP application
    void stop();
    
    // Get current timestamp in milliseconds since boot
    uint32_t getCurrentTimestamp();
    
    // Send different types of vehicle data using Protobuf messages
    void sendPrimaryVehicleStateProtobuf(const avisio_ivi::PrimaryVehicleState& state);
    void sendSignalLightingStatusProtobuf(const avisio_ivi::SignalLightingStatus& status);
    void sendPowerThermalStatusProtobuf(const avisio_ivi::PowerThermalStatus& status);
    void sendDoorSecurityStatusProtobuf(const avisio_ivi::DoorSecurityStatus& status);
};

#endif // VEHICLE_DATA_SENDER_HPP

