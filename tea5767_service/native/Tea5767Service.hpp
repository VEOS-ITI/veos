#ifndef TEA5767_SERVICE_HPP
#define TEA5767_SERVICE_HPP

#include <aidl/android/vendor/radio/BnTea5767Service.h>
#include "tea5767hal.hpp"

namespace aidl::android::vendor::radio 
{

    class Tea5767Service : public BnTea5767Service {
        public:
        Tea5767Service();
        ~Tea5767Service();

        // AIDL methods
        ::ndk::ScopedAStatus mute() override;
        ::ndk::ScopedAStatus unmute() override;
        ::ndk::ScopedAStatus changeStation(int32_t in_frequencyKHz) override;
        private:
        std::unique_ptr<Tea5767Hal> hal_;
    };
}   

#endif // TEA5767_SERVICE_HPP
