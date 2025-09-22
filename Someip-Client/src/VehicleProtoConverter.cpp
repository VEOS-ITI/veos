#include "VehicleProtoConverter.hpp"

CommonAPI::ByteBuffer VehicleProtoConverter::toByteBuffer(const avisio_ivi::PrimaryVehicleState& state) {
    std::string data;
    if (!state.SerializeToString(&data)) {
        throw std::runtime_error("Failed to serialize PrimaryVehicleState");
    }
    return CommonAPI::ByteBuffer(data.begin(), data.end());
}

avisio_ivi::PrimaryVehicleState VehicleProtoConverter::fromByteBuffer(const CommonAPI::ByteBuffer& buffer) {
    avisio_ivi::PrimaryVehicleState state;
    std::string data(buffer.begin(), buffer.end());
    if (!state.ParseFromString(data)) {
        throw std::runtime_error("Failed to parse PrimaryVehicleState from ByteBuffer");
    }
    return state;
}
