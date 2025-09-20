// Copyright (C) 2014-2019 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <string>
#include <thread>
#include <stdexcept> // For std::runtime_error

#ifndef _WIN32
#include <unistd.h>
#endif

#include <CommonAPI/CommonAPI.hpp>
#include <v0/commonapi/examples/E01HelloWorldProxy.hpp>
#include "RSAHelper.hpp"
#include "VehicleProtoConverter.hpp"           // converter
#include "../src-gen/protobuf/avisio_ivi.pb.h" // Protobuf header
#include "ProtobufDataHandler.hpp"
#include <android/log.h> // Add Android logging

using namespace v0::commonapi::examples;

// Define Android logging tags
#define LOG_TAG "VHAL_Client"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void listener(const CommonAPI::AvailabilityStatus &status)
{
    LOGI("Service status changed: %d", (int)status);
}

int main()
{
    CommonAPI::Runtime::setProperty("LogContext", "E01C");
    CommonAPI::Runtime::setProperty("LogApplication", "E01C");
    CommonAPI::Runtime::setProperty("LibraryBase", "E01HelloWorld");
    CommonAPI::Runtime::setProperty("LogLevel", "DEBUG");

    LOGI("Starting VHAL client...");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examples.HelloWorld";
    std::string connection = "client-sample";

    std::shared_ptr<E01HelloWorldProxy<>> myProxy = runtime->buildProxy<E01HelloWorldProxy>(domain, instance, connection);

    std::function<void(const CommonAPI::AvailabilityStatus &status)> proxyStatusListener = listener;
    // myProxy->getProxyStatusEvent().subscribe(proxyStatusListener);
    LOGI("Checking availability!");
    while (!myProxy->isAvailable())
    {
        LOGI("Waiting for service...");
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    LOGI("Available...");
    
    ProtobufDataHandler dataHandler;
    
    myProxy->getStatusEvent().subscribe(
        [&](const CommonAPI::ByteBuffer &Buffer)
        {
            LOGD("Received data in getStatusEvent callback.");
            
            try
            {
                // Convert the received ByteBuffer back to a Protobuf message
                CommonAPI::ByteBuffer protoStatus = decryptByteBufferRSA(Buffer);
                avisio_ivi::PrimaryVehicleState state = VehicleProtoConverter::fromByteBuffer(protoStatus);

                // Now you can access the structured data from the receivedState object
                LOGD("Vehicle State Log:");
                LOGD("  Timestamp (ms): %u", state.timestamp_ms());
                LOGD("  Speed (km/h): %u", state.speed_kmh());
                LOGD("  Engine RPM: %u", state.engine_rpm());
                LOGD("  Current Gear: %d", state.current_gear());
                LOGD("  Battery Level (%): %u", state.battery_level());
                LOGD("  Engine Temp (°C): %d", state.engine_temp_c());
                LOGD("  Charging: %s", state.is_charging() ? "Yes" : "No");
                LOGD("  Exterior Lights: %d", state.exterior_lights_status());
                LOGD("  Cabin Temp (°C): %d", state.cabin_temp_c());
                LOGD("  AC On: %s", state.ac_on() ? "Yes" : "No");
                LOGD("  Left Signal: %s", state.turn_signal_left() ? "On" : "Off");
                LOGD("  Right Signal: %s", state.turn_signal_right() ? "On" : "Off");
                LOGD("  Hazard Lights: %s", state.hazard_lights_on() ? "On" : "Off");
                LOGD("------------------------------------------");
                
                // Process the parsed data to set VHAL properties
                dataHandler.processPrimaryVehicleState(state);
            }
            catch (const std::runtime_error &e)
            {
                LOGE("Error parsing Protobuf from received ByteBuffer: %s", e.what());
            }
        });

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}