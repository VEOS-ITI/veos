#ifndef PROTOBUF_DATA_HANDLER_HPP
#define PROTOBUF_DATA_HANDLER_HPP

#include <CommonAPI/CommonAPI.hpp>
#include "../src-gen/protobuf/avisio_ivi.pb.h"
#include "RSAHelper.hpp"
#include "../inc/vhal_shim.h"

class ProtobufDataHandler {
public:
    ProtobufDataHandler();
    ~ProtobufDataHandler();
    void handleProtobufData(const CommonAPI::ByteBuffer& encryptedData);

private:
    vhal_client_t* vhal_client_;
    void processPrimaryVehicleState(const avisio_ivi::PrimaryVehicleState& state);
    void processSignalLightingStatus(const avisio_ivi::SignalLightingStatus& status);
    void processSafetyStatus(const avisio_ivi::SafetyStatus& status);
};

#endif // PROTOBUF_DATA_HANDLER_HPP

