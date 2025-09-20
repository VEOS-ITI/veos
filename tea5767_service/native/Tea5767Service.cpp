#include "Tea5767Service.hpp"
#include <android-base/logging.h>

namespace aidl::android::vendor::radio {

Tea5767Service::Tea5767Service() {
    hal_ = std::make_unique<Tea5767Hal>();
    LOG(INFO) << "Tea5767Service created";
}

Tea5767Service::~Tea5767Service() {
    LOG(INFO) << "Tea5767Service destroyed";
}

::ndk::ScopedAStatus Tea5767Service::mute() {
    if (!hal_) {
        LOG(ERROR) << "HAL not initialized";
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }
    hal_->mute();   // no return to check
    LOG(INFO) << "Muted radio";
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Tea5767Service::unmute() {
    if (!hal_) {
        LOG(ERROR) << "HAL not initialized";
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }
    hal_->unmute();   // always assumed to succeed
    LOG(INFO) << "Unmuted radio";
    return ::ndk::ScopedAStatus::ok();
}

::ndk::ScopedAStatus Tea5767Service::changeStation(int32_t in_frequencyKHz) {
    if (!hal_) {
        LOG(ERROR) << "HAL not initialized";
        return ::ndk::ScopedAStatus::fromExceptionCode(EX_NULL_POINTER);
    }
    hal_->tune(in_frequencyKHz);
    LOG(INFO) << "Changed station to " << in_frequencyKHz << " kHz";
    return ::ndk::ScopedAStatus::ok();

} 
}// namespace aidl::android::vendor::radio
