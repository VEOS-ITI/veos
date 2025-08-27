#include "ProtobufDataHandler.hpp"
#include <iostream>
#include "../inc/vhalproperty.h"

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
        processPrimaryVehicleState(primaryState);
        return;
    }

    // Attempt to parse as SignalLightingStatus
    avisio_ivi::SignalLightingStatus lightingStatus;
    if (lightingStatus.ParseFromArray(decryptedData.data(), decryptedData.size())) {
        processSignalLightingStatus(lightingStatus);
        return;
    }

    // Attempt to parse as SafetyStatus
    avisio_ivi::SafetyStatus safetyStatus;
    if (safetyStatus.ParseFromArray(decryptedData.data(), decryptedData.size())) {
        processSafetyStatus(safetyStatus);
        return;
    }

    std::cerr << "[ProtobufDataHandler] Could not parse protobuf message." << std::endl;
}

void ProtobufDataHandler::processPrimaryVehicleState(const avisio_ivi::PrimaryVehicleState& state) {
    if (!vhal_client_) return;

    std::cout << "[ProtobufDataHandler] Processing PrimaryVehicleState..." << std::endl;

    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_TIMESTAMP, 0, state.timestamp_ms());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_SPEED_KMH, 0, static_cast<int32_t>(state.speed_kmh() * 100)); // Scale float to int
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_ENGINE_RPM, 0, static_cast<int32_t>(state.engine_rpm() * 100)); // Scale double to int
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_CURRENT_GEAR, 0, state.current_gear());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_GEAR_MODE, 0, state.gear_mode());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_FUEL_LEVEL, 0, state.fuel_level_millipercent());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_ENGINE_TEMP_C, 0, state.engine_temp_c());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_STATUS_FLAGS, 0, state.status_flags());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_ODOMETER_KM, 0, state.odometer_km());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_PRIMARY_STATE_TRIP_DISTANCE_M, 0, state.trip_distance_m());

    std::cout << "[ProtobufDataHandler] PrimaryVehicleState: Speed=" << state.speed_kmh() << " km/h" << std::endl;
}

void ProtobufDataHandler::processSignalLightingStatus(const avisio_ivi::SignalLightingStatus& status) {
    if (!vhal_client_) return;

    std::cout << "[ProtobufDataHandler] Processing SignalLightingStatus..." << std::endl;

    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_LIGHTING_STATUS_TIMESTAMP, 0, status.timestamp_ms());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_LIGHTING_STATUS_TURN_SIGNALS, 0, status.turn_signals());
    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_LIGHTING_STATUS_EXTERIOR_LIGHTS, 0, status.exterior_lights());

    // Assuming exterior_lights contains information about high beams, or you might need to parse it further
    // For now, let's assume if exterior_lights is set, high beams might be on (this is a simplification)
    // You might need to adjust this logic based on how your exterior_lights bitfield is structured.
    vhal_set_bool(vhal_client_, VEHICLE_PROPERTY_LIGHTING_STATUS_HIGH_BEAM_LIGHTS, 0, (status.exterior_lights() & 0x01)); // Example: assuming bit 0 of exterior_lights indicates high beams

    std::cout << "[ProtobufDataHandler] SignalLightingStatus: Turn Signals=" << status.turn_signals() << std::endl;
}

void ProtobufDataHandler::processSafetyStatus(const avisio_ivi::SafetyStatus& status) {
    if (!vhal_client_) return;

    std::cout << "[ProtobufDataHandler] Processing SafetyStatus..." << std::endl;

    vhal_set_int32(vhal_client_, VEHICLE_PROPERTY_SAFETY_STATUS_TIMESTAMP, 0, status.timestamp_ms());
    vhal_set_bool(vhal_client_, VEHICLE_PROPERTY_SAFETY_STATUS_SEATBELT_FASTENED, 0, status.seatbelt_fastened());
    vhal_set_bool(vhal_client_, VEHICLE_PROPERTY_SAFETY_STATUS_AIRBAG_OK, 0, status.airbag_ok());
    vhal_set_bool(vhal_client_, VEHICLE_PROPERTY_SAFETY_STATUS_TRACTION_CONTROL, 0, status.traction_control());

    // Assuming low battery warning and brake system warning are part of safety status or derived from it
    // These mappings are placeholders and might need actual logic based on your protobuf definition
    vhal_set_bool(vhal_client_, VEHICLE_PROPERTY_SAFETY_STATUS_LOW_BATTERY_WARNING, 0, false); // Placeholder
    vhal_set_bool(vhal_client_, VEHICLE_PROPERTY_SAFETY_STATUS_BRAKE_WARNING, 0, false); // Placeholder

    std::cout << "[ProtobufDataHandler] SafetyStatus: Seatbelt Fastened=" << status.seatbelt_fastened() << " Airbag OK=" << status.airbag_ok() << " Traction Control=" << status.traction_control() << std::endl;
}