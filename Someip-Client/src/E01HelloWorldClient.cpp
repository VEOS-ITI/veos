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
#include "VehicleProtoConverter.hpp"           // Include the converter
#include "../src-gen/protobuf/avisio_ivi.pb.h" // Include your Protobuf header

using namespace v0::commonapi::examples;

void listener(const CommonAPI::AvailabilityStatus &status)
{
    std::cout << "in func android status:" << (int)status << std::endl;
}
int main()
{
    CommonAPI::Runtime::setProperty("LogContext", "E01C");
    CommonAPI::Runtime::setProperty("LogApplication", "E01C");
    CommonAPI::Runtime::setProperty("LibraryBase", "E01HelloWorld");
    CommonAPI::Runtime::setProperty("LogLevel", "DEBUG");

    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    std::string domain = "local";
    std::string instance = "commonapi.examples.HelloWorld";
    std::string connection = "client-sample";

    std::shared_ptr<E01HelloWorldProxy<>> myProxy = runtime->buildProxy<E01HelloWorldProxy>(domain, instance, connection);

    std::function<void(const CommonAPI::AvailabilityStatus &status)> proxyStatusListener = listener;
    // myProxy->getProxyStatusEvent().subscribe(proxyStatusListener);
    std::cout << "Checking availability!" << std::endl;
    while (!myProxy->isAvailable())
    {
        std::cout << "Waiting for service..." << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    std::cout << "Available..." << std::endl;

    myProxy->getStatusEvent().subscribe(
        [&](const CommonAPI::ByteBuffer &Buffer)
        {
          

            std::cerr  << "[E01HelloWorldClient] Received data in getStatusEvent callback." << std::endl;
            try
            {
                // Convert the received ByteBuffer back to a Protobuf message
                CommonAPI::ByteBuffer protoStatus = decryptByteBufferRSA(Buffer);
                avisio_ivi::PrimaryVehicleState state = VehicleProtoConverter::fromByteBuffer(protoStatus);

                // Now you can access the structured data from the receivedState object
                 std::cerr  << "[ProtobufDataHandler] Vehicle State Log:" << std::endl;
                 std::cerr  << "  Timestamp (ms): " << state.timestamp_ms() << std::endl;
                 std::cerr  << "  Speed (km/h): " << state.speed_kmh() << std::endl;
                 std::cerr  << "  Engine RPM: " << state.engine_rpm() << std::endl;
                 std::cerr  << "  Current Gear: " << state.current_gear() << std::endl;
                 std::cerr  << "  Battery Level (%): " << state.battery_level() << std::endl;
                 std::cerr  << "  Engine Temp (°C): " << state.engine_temp_c() << std::endl;
                std::cerr  << "  Charging: " << (state.is_charging() ? "Yes" : "No") << std::endl;
                 std::cerr  << "  Exterior Lights: " << state.exterior_lights_status() << std::endl;
                std::cerr  << "  Cabin Temp (°C): " << state.cabin_temp_c() << std::endl;
                 std::cerr << "  AC On: " << (state.ac_on() ? "Yes" : "No") << std::endl;
                std::cerr << "  Left Signal: " << (state.turn_signal_left() ? "On" : "Off") << std::endl;
                std::cerr << "  Right Signal: " << (state.turn_signal_right() ? "On" : "Off") << std::endl;
              std::cerr  << "  Hazard Lights: " << (state.hazard_lights_on() ? "On" : "Off") << std::endl;
              std::cerr << "------------------------------------------" << std::endl;
                // Add more print statements for other fields as needed
            }
            catch (const std::runtime_error &e)
            {
                std::cerr << "[Client] Error parsing Protobuf from received ByteBuffer: " << e.what() << std::endl;
            }
        });

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
