#include "vhal_shim.h"

#include <IVhalClient.h>
#include <IHalPropValue.h>
#include <VehicleUtils.h>

#include <android-base/result.h>
#include <android-base/logging.h>

#include <memory>

using ::android::frameworks::automotive::vhal::IVhalClient;
using ::android::frameworks::automotive::vhal::IHalPropValue;

// Opaque handle
struct vhal_client {
    std::shared_ptr<IVhalClient> client;
};

static int to_errno_like(::android::frameworks::automotive::vhal::ErrorCode ec) {
    using EC = ::android::frameworks::automotive::vhal::ErrorCode;
    switch (ec) {
        case EC::OK:                       return 0;
        case EC::INVALID_ARG:              return -22;  // EINVAL
        case EC::TIMEOUT:                  return -110; // ETIMEDOUT
        case EC::TRANSACTION_ERROR:        return -74;  // EBADMSG-ish
        case EC::TRY_AGAIN_FROM_VHAL:      return -11;  // EAGAIN
        case EC::NOT_AVAILABLE_FROM_VHAL:  return -19;  // ENODEV
        case EC::ACCESS_DENIED_FROM_VHAL:  return -13;  // EACCES
        case EC::INTERNAL_ERROR_FROM_VHAL: return -5;   // EIO
        default:                           return -5;
    }
}

extern "C" vhal_client_t* vhal_connect(void) {
    auto c = new vhal_client();
    c->client = IVhalClient::create();     // blocks for service; returns nullptr on failure
    if (!c->client) { delete c; return nullptr; }
    return c;
}

extern "C" void vhal_disconnect(vhal_client_t* c) {
    if (!c) return;
    c->client.reset();
    delete c;
}

static int get_one(vhal_client_t* c, int32_t propId, int32_t areaId,
                   std::vector<int32_t>* i32, std::vector<int64_t>* i64,
                   std::vector<float>* f32, std::string* str, std::vector<uint8_t>* bytes) {
    if (!c || !c->client) return -5;
    auto req = c->client->createHalPropValue(propId, areaId);
    auto res = c->client->getValueSync(*req);
    if (!res.ok()) return to_errno_like(res.error().code());

    std::unique_ptr<IHalPropValue> v = std::move(res.value());
    if (i32)   *i32   = v->getInt32Values();
    if (str)   *str   = v->getStringValue();
    if (bytes) *bytes = v->getByteValues();
    return 0;
}

static int set_one(vhal_client_t* c, int32_t propId, int32_t areaId,
                   const std::vector<int32_t>* i32, const std::vector<int64_t>* i64,
                   const std::vector<float>* f32, const std::string* str,
                   const std::vector<uint8_t>* bytes) {
    if (!c || !c->client) return -5;
    auto val = c->client->createHalPropValue(propId, areaId);
    if (i32)   val->setInt32Values(*i32);
    if (str)   val->setStringValue(*str);
    if (bytes) val->setByteValues(*bytes);

    auto r = c->client->setValueSync(*val);
if (!r.ok()) return to_errno_like(r.error().code());
    return 0;
}

// -------- typed helpers --------
extern "C" int vhal_get_int32(vhal_client_t* c, int32_t propId, int32_t areaId, int32_t* out) {
    if (!out) return -22;
    std::vector<int32_t> v;
    int rc = get_one(c, propId, areaId, &v, nullptr, nullptr, nullptr, nullptr);
    if (rc != 0) return rc;
    if (v.empty()) return -5;
    *out = v[0];
    return 0;
}

extern "C" int vhal_get_bool(vhal_client_t* c, int32_t propId, int32_t areaId, int* out01) {
    if (!out01) return -22;
    std::vector<int32_t> v;
    int rc = get_one(c, propId, areaId, &v, nullptr, nullptr, nullptr, nullptr);
    if (rc != 0) return rc;
    if (v.empty()) return -5;
    *out01 = (v[0] != 0) ? 1 : 0;
    return 0;
}

extern "C" int vhal_set_int32(vhal_client_t* c, int32_t propId, int32_t areaId, int32_t value) {
    std::vector<int32_t> v{value};
    return set_one(c, propId, areaId, &v, nullptr, nullptr, nullptr, nullptr);
}

extern "C" int vhal_set_bool(vhal_client_t* c, int32_t propId, int32_t areaId, int value01) {
    std::vector<int32_t> v{ value01 ? 1 : 0 };
    return set_one(c, propId, areaId, &v, nullptr, nullptr, nullptr, nullptr);
}
