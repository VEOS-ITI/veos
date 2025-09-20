#include "Tea5767Service.hpp"
#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>

using aidl::android::vendor::radio::Tea5767Service;

int main() {
    LOG(INFO) << "Starting Tea5767 AIDL Service...";

    // Setup binder thread pool
    ABinderProcess_setThreadPoolMaxThreadCount(0);

    // Create service instance
    std::shared_ptr<Tea5767Service> service = ndk::SharedRefBase::make<Tea5767Service>();

    // Register with ServiceManager
    const std::string instance = std::string() + Tea5767Service::descriptor + "/default";
    if (AServiceManager_addService(service->asBinder().get(), instance.c_str()) != STATUS_OK) {
        LOG(FATAL) << "Failed to register Tea5767Service with instance: " << instance;
        return -1;
    }

    LOG(INFO) << "Tea5767Service registered as " << instance;

    // Join binder thread pool
    ABinderProcess_joinThreadPool();
    return 0;
}
