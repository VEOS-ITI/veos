#pragma once

#include <CommonAPI/CommonAPI.hpp>
#include "../src-gen/protobuf/avisio_ivi.pb.h"
#include <stdexcept>
#include <string>

class VehicleProtoConverter {
public:
    static CommonAPI::ByteBuffer toByteBuffer(const avisio_ivi::PrimaryVehicleState& state);
    static avisio_ivi::PrimaryVehicleState fromByteBuffer(const CommonAPI::ByteBuffer& buffer);
};

