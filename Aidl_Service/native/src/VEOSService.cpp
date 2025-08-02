#include "VEOSService.hpp"
#include <chrono>
#include <ctime>

namespace aidl::android::vendor::test {

::ndk::ScopedAStatus VEOSService::getSpeed(std::float *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}

::ndk::ScopedAStatus VEOSService::getGear(std::int32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getTemp(std::uint32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getTime(std::string *_aidl_return)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm = *std::localtime(&now_c);
    char buf[9];
    strftime(buf, sizeof(buf), "%H:%M:%S", &now_tm);
    *_aidl_return = buf;  
    return ndk::ScopedAStatus::ok();
}
::ndk::ScopedAStatus VEOSService::getBatteryLevel(std::uint32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getEngineTemp(std::uint32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getOdometerKm(std::uint32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getRightLightingStatus(std::int32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getLeftLightingStatus(std::int32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getLightingStatus(std::int32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getSeatBeltStatus(std::int32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
::ndk::ScopedAStatus VEOSService::getTirePressure(std::uint32 *_aidl_return)
{
    return ::ndk::ScopedAStatus();
}
}
