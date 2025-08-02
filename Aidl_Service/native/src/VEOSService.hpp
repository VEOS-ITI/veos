#pragma once

#include <aidl/android/vendor/test/BnVEOSService.h>
#include <android/binder_ibinder.h>

namespace aidl::android::vendor::test
{

    class VEOSService : public aidl::android::vendor::test::BnVEOSService
    {
    public:
        VEOSService()
        {
            // Constructor body (empty)
        }

        ~VEOSService()
        {
            // Destructor body (empty)
        }
        ::ndk::ScopedAStatus getSpeed(std::float *_aidl_return) override;
        ::ndk::ScopedAStatus getGear(std::int32 *_aidl_return) override;
        ::ndk::ScopedAStatus getTemp(std::uint32 *_aidl_return) override;
        ::ndk::ScopedAStatus getTime(std::string *_aidl_return) override;
        ::ndk::ScopedAStatus getBatteryLevel(std::uint32 *_aidl_return) override;
        ::ndk::ScopedAStatus getEngineTemp(std::uint32 *_aidl_return) override;
        ::ndk::ScopedAStatus getOdometerKm(std::uint32 *_aidl_return) override;
        ::ndk::ScopedAStatus getRightLightingStatus(std::int32 *_aidl_return) override;
        ::ndk::ScopedAStatus getLeftLightingStatus(std::int32 *_aidl_return) override;
        ::ndk::ScopedAStatus getLightingStatus(std::int32 *_aidl_return) override;
        ::ndk::ScopedAStatus getSeatBeltStatus(std::int32 *_aidl_return) override;
        ::ndk::ScopedAStatus getTirePressure(std::uint32 *_aidl_return) override;
    };

} // namespace aidl::android::vendor::test
