// Copyright (C) 2014-2019 Bayerische Motoren Werke Aktiengesellschaft (BMW AG)
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include <string>
#include <thread>

#ifndef _WIN32
#include <unistd.h>
#endif

#include <CommonAPI/CommonAPI.hpp>
#include <v0/commonapi/examples/E01HelloWorldProxy.hpp>
#include "RSAHelper.hpp"
#include "ProtobufDataHandler.hpp" 

using namespace v0::commonapi::examples;

void listener(const CommonAPI::AvailabilityStatus &status)
{
    std::cout << "in func android status:" << (int)status << std::endl;
}
int main() {
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
    while (!myProxy->isAvailable()) {
        std::cout << "Waiting for service..." << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }
    std::cout << "Available..." << std::endl;

    // REMOVED THE UNUSED VARIABLES THAT CAUSE SEGFAULT:
    // const std::string name = "World";
    // CommonAPI::CallStatus callStatus;
    // std::string returnMessage;
    // CommonAPI::CallInfo info(1000);
    // info.sender_ = 1234;

    ProtobufDataHandler dataHandler; 

    myProxy->getStatusEvent().subscribe(
        [&](const CommonAPI::ByteBuffer &encrypted) { // Capture dataHandler by reference
            // CommonAPI::ByteBuffer buffer = decryptByteBufferRSA(encrypted);
            // std::cout << "[Client] Received status event." << buffer.size() << std::endl;
            // if (buffer.empty()) {
            //     std::cout << "[Client] Received empty broadcast." << std::endl;
            //     return;
            // }
            // std::string msg(buffer.begin(), buffer.end());
            // std::cout << "[Client] Received broadcast: " << msg << std::endl;
            dataHandler.handleProtobufData(encrypted); // Call the handler
        });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

