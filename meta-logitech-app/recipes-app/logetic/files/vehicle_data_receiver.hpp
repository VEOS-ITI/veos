#ifndef VEHICLE_DATA_RECEIVER_HPP
#define VEHICLE_DATA_RECEIVER_HPP

#include <memory>
#include <functional>
#include <vsomeip/vsomeip.hpp>
#include "avisio_ivi_data.pb.h" // Include generated Protobuf header

// SOME/IP Service and Event Definitions (must match sender)
#define SERVICE_ID 0x1111
#define INSTANCE_ID 0x2222

// Event IDs for different data types
#define PRIMARY_VEHICLE_EVENT_ID 0x8001
#define SIGNAL_LIGHTING_EVENT_ID 0x8002
#define POWER_THERMAL_EVENT_ID 0x8003
#define DOOR_SECURITY_EVENT_ID 0x8004

// Callback function types for different Protobuf message types
using PrimaryVehicleProtobufCallback = std::function<void(const avisio_ivi::PrimaryVehicleState&)>;
using SignalLightingProtobufCallback = std::function<void(const avisio_ivi::SignalLightingStatus&)>;
using PowerThermalProtobufCallback = std::function<void(const avisio_ivi::PowerThermalStatus&)>;
using DoorSecurityProtobufCallback = std::function<void(const avisio_ivi::DoorSecurityStatus&)>;

class VehicleDataReceiver {
private:
    std::shared_ptr<vsomeip::application> app_;
    bool is_initialized_;
    
    // Callback functions
    PrimaryVehicleProtobufCallback primary_vehicle_callback_;
    SignalLightingProtobufCallback signal_lighting_callback_;
    PowerThermalProtobufCallback power_thermal_callback_;
    DoorSecurityProtobufCallback door_security_callback_;
    
    // SOME/IP event handlers
    void onServiceAvailable(vsomeip::service_t service, vsomeip::instance_t instance, bool is_available);
    void onMessage(const std::shared_ptr<vsomeip::message>& message);
    
    // Parse received Protobuf data
    template<typename T>
    bool parseProtobufMessage(const std::shared_ptr<vsomeip::payload>& payload, T& message);

public:
    VehicleDataReceiver();
    ~VehicleDataReceiver();
    
    // Initialize the SOME/IP application
    bool initialize();
    
    // Start the SOME/IP application (blocking call)
    void start();
    
    // Stop the SOME/IP application
    void stop();
    
    // Set callback functions for different Protobuf message types
    void setPrimaryVehicleCallback(PrimaryVehicleProtobufCallback callback);
    void setSignalLightingCallback(SignalLightingProtobufCallback callback);
    void setPowerThermalCallback(PowerThermalProtobufCallback callback);
    void setDoorSecurityCallback(DoorSecurityProtobufCallback callback);
};

#endif // VEHICLE_DATA_RECEIVER_HPP

