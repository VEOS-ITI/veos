#include <jni.h>
#include "vhal_shim.h"   // your C++ shim header

extern "C" {

JNIEXPORT jlong JNICALL
Java_com_veos_telemetry_VhalNative_vhal_1connect(JNIEnv* env, jclass clazz) {
    vhal_client_t* c = vhal_connect();
    return reinterpret_cast<jlong>(c);
}

JNIEXPORT void JNICALL
Java_com_veos_telemetry_VhalNative_vhal_1disconnect(JNIEnv* env, jclass clazz, jlong ptr) {
    vhal_client_t* c = reinterpret_cast<vhal_client_t*>(ptr);
    vhal_disconnect(c);
}

JNIEXPORT jint JNICALL
Java_com_veos_telemetry_VhalNative_vhal_1get_1int32(JNIEnv* env, jclass clazz, jlong ptr, jint propId, jint areaId) {
    vhal_client_t* c = reinterpret_cast<vhal_client_t*>(ptr);
    int32_t out = 0;
    int rc = vhal_get_int32(c, propId, areaId, &out);
    return (rc == 0) ? out : -1;
}

JNIEXPORT jint JNICALL
Java_com_veos_telemetry_VhalNative_vhal_1get_1bool(JNIEnv* env, jclass clazz, jlong ptr, jint propId, jint areaId) {
    vhal_client_t* c = reinterpret_cast<vhal_client_t*>(ptr);
    int out = 0;
    int rc = vhal_get_bool(c, propId, areaId, &out);
    return (rc == 0) ? out : 0;
}

}

