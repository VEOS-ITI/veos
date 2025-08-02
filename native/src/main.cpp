#include "VEOSService.hpp"

#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <android/binder_process.h>
#include <android/binder_ibinder.h>
#include <aidl/android/vendor/test/IVEOSService.h> 

using aidl::android::vendor::test::VEOSService;

int main() {
    LOG(INFO) << "VEOSService_service starting...";

    ABinderProcess_setThreadPoolMaxThreadCount(0);
    std::shared_ptr<aidl::android::vendor::test::VEOSService> veos_service = ndk::SharedRefBase::make<aidl::android::vendor::test::VEOSService>();
     if (!veos_service) {
        LOG(ERROR) << "Failed to create VEOSService service instance.";
        return EXIT_FAILURE;
    }
    const std::string instance = std::string() + aidl::android::vendor::test::VEOSService::descriptor + "/default";
    LOG(INFO) << "Registering service with name: " << instance;

    binder_status_t status = AServiceManager_addService(veos_service->asBinder().get(), instance.c_str());

        if (status == STATUS_OK) {
            LOG(INFO) << "Service registered successfully.";
        } else {
            LOG(ERROR) << "Failed to register service. Status: " << status;
            return EXIT_FAILURE;
        }
        ABinderProcess_joinThreadPool();
        return EXIT_FAILURE;
}
