#include "ProtobufDataHandler.hpp"
#include <iostream>
#include "../inc/vhalproperty.h"

// Define VHAL Property IDs with their exact decimal values and types
#define VEOS_GEAR_SELECTION             557846352    // int32
#define VEOS_PERF_VEHICLE_SPEED         557846353    // int32
#define VEOS_EV_BATTERY_LEVEL           557846354    // int32
#define VEOS_EV_BATTERY_TEMP            557846355    // int32
#define VEOS_EV_CHARGING_STATE          557846357    // bool 
#define VEOS_CABIN_TEMP                 557846364    // int32
#define VEOS_HEADLIGHTS_STATE           555749214    // bool
#define VEOS_LAST_UPDATE_TIMESTAMP      557846374    // int32

// Specific boolean properties for turn signals and AC, as per user's request
// These are mapped to the provided boolean VHAL properties
#define VEOS_TURN_SIGNAL_LEFT_STATE     555749215 // Assuming this is for high beam, but using it for left signal as per user's request for now
#define VEOS_TURN_SIGNAL_RIGHT_STATE    555749216 // Assuming this is for DRL, but using it for right signal as per user's request for now
#define VEOS_HAZARD_LIGHTS_STATE        555749217 // Assuming this is for low battery warning, but using it for hazard as per user's request for now
#define VEOS_AC_STATE                   555749218 // Assuming this is for charging fault, but using it for AC as per user's request for now

ProtobufDataHandler::ProtobufDataHandler() {
    vhal_client_ = vhal_connect();
    if (!vhal_client_) {
        std::cerr << "Failed to connect to VHAL." << std::endl;
    }
}

ProtobufDataHandler::~ProtobufDataHandler() {
    if (vhal_client_) {
        vhal_disconnect(vhal_client_);
    }
}

void ProtobufDataHandler::handleProtobufData(const CommonAPI::ByteBuffer& encryptedData) {
    CommonAPI::ByteBuffer decryptedData = decryptByteBufferRSA(encryptedData);
    if (decryptedData.empty()) {
        std::cerr << "[ProtobufDataHandler] Decrypted data is empty." << std::endl;
        return;
    }

    // Attempt to parse as PrimaryVehicleState
    avisio_ivi::PrimaryVehicleState primaryState;
    if (primaryState.ParseFromArray(decryptedData.data(), decryptedData.size())) {
        std::cout << "[ProtobufDataHandler] Successfully parsed PrimaryVehicleState:" << std::endl;
        std::cout << "  Timestamp: " << primaryState.timestamp_ms() << std::endl;
        std::cout << "  Speed: " << primaryState.speed_kmh() << " km/h" << std::endl;
        std::cout << "  Engine RPM: " << primaryState.engine_rpm() << std::endl;
        std::cout << "  Current Gear: " << primaryState.current_gear() << std::endl;
        std::cout << "  Battery Level: " << primaryState.battery_level() << std::endl;
        std::cout << "  Is Charging: " << (primaryState.is_charging() ? "true" : "false") << std::endl;
        std::cout << "  Exterior Lights: " << primaryState.exterior_lights_status() << std::endl;
        std::cout << "  Cabin Temp: " << primaryState.cabin_temp_c() << std::endl;
        std::cout << "  AC On: " << (primaryState.ac_on() ? "true" : "false") << std::endl;
        std::cout << "  Turn Signal Left: " << (primaryState.turn_signal_left() ? "true" : "false") << std::endl;
        std::cout << "  Turn Signal Right: " << (primaryState.turn_signal_right() ? "true" : "false") << std::endl;
        std::cout << "  Hazard Lights On: " << (primaryState.hazard_lights_on() ? "true" : "false") << std::endl;

        processPrimaryVehicleState(primaryState);
        return;
    }

    // The client now only expects PrimaryVehicleState, so other parsing attempts are removed.
    std::cerr << "[ProtobufDataHandler] Could not parse protobuf message as PrimaryVehicleState." << std::endl;
}

void ProtobufDataHandler::processPrimaryVehicleState(const avisio_ivi::PrimaryVehicleState& state) {
    if (!vhal_client_) {
        std::cerr << "[ProtobufDataHandler] VHAL client not connected." << std::endl;
        return;
    }

    std::cout << "[ProtobufDataHandler] Attempting to set VHAL Properties..." << std::endl;

    // Map and set VHAL properties

    // VEOS_GEAR_SELECTION (int32)
    vhal_set_int32(vhal_client_, VEOS_GEAR_SELECTION, 0, static_cast<int32_t>(state.current_gear()));

    // VEOS_PERF_VEHICLE_SPEED (int32, scaled 0-180 km/h by server)
    vhal_set_int32(vhal_client_, VEOS_PERF_VEHICLE_SPEED, 0, static_cast<int32_t>(state.speed_kmh()));

    // VEOS_EV_BATTERY_LEVEL (int32)
    vhal_set_int32(vhal_client_, VEOS_EV_BATTERY_LEVEL, 0, static_cast<int32_t>(state.battery_level()));

    // VEOS_EV_BATTERY_TEMP (int32) - Using engine_temp_c for now as a placeholder
    vhal_set_int32(vhal_client_, VEOS_EV_BATTERY_TEMP, 0, static_cast<int32_t>(state.engine_temp_c()));

    // VEOS_EV_CHARGING_STATE (bool)
    vhal_set_bool(vhal_client_, VEOS_EV_CHARGING_STATE, 0, state.is_charging());

    // VEOS_CABIN_TEMP (int32)
    vhal_set_int32(vhal_client_, VEOS_CABIN_TEMP, 0, static_cast<int32_t>(state.cabin_temp_c()));

    // VEOS_HEADLIGHTS_STATE (bool)
    // Mapping LightStatus enum to bool: LIGHTS_ON -> true, others -> false
    vhal_set_bool(vhal_client_, VEOS_HEADLIGHTS_STATE, 0, state.exterior_lights_status() == avisio_ivi::LIGHTS_ON);

    // VEOS_LAST_UPDATE_TIMESTAMP (int32)
    vhal_set_int32(vhal_client_, VEOS_LAST_UPDATE_TIMESTAMP, 0, static_cast<int32_t>(state.timestamp_ms()));

    // Turn Signals and Hazard Lights (bool)
    vhal_set_bool(vhal_client_, VEOS_TURN_SIGNAL_LEFT_STATE, 0, state.turn_signal_left());
    vhal_set_bool(vhal_client_, VEOS_TURN_SIGNAL_RIGHT_STATE, 0, state.turn_signal_right());
    vhal_set_bool(vhal_client_, VEOS_HAZARD_LIGHTS_STATE, 0, state.hazard_lights_on());

    // AC State (bool)
    vhal_set_bool(vhal_client_, VEOS_AC_STATE, 0, state.ac_on());

    std::cout << "[ProtobufDataHandler] VHAL Properties Set:" << std::endl;
    std::cout << "  Gear: " << state.current_gear() << std::endl;
    std::cout << "  Speed: " << state.speed_kmh() << " km/h" << std::endl;
    std::cout << "  Battery Level: " << state.battery_level() << std::endl;
    std::cout << "  Battery Temp: " << state.engine_temp_c() << std::endl;
    std::cout << "  Charging State: " << (state.is_charging() ? "ON" : "OFF") << std::endl;
    std::cout << "  Cabin Temp: " << state.cabin_temp_c() << std::endl;
    std::cout << "  Lights State: " << state.exterior_lights_status() << std::endl;
    std::cout << "  Left Signal: " << (state.turn_signal_left() ? "ON" : "OFF") << std::endl;
    std::cout << "  Right Signal: " << (state.turn_signal_right() ? "ON" : "OFF") << std::endl;
    std::cout << "  Hazard Lights: " << (state.hazard_lights_on() ? "ON" : "OFF") << std::endl;
    std::cout << "  AC State: " << (state.ac_on() ? "ON" : "OFF") << std::endl;
}