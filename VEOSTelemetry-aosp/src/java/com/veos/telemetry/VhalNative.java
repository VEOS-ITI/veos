package com.veos.telemetry;

import android.util.Log;

public class VhalNative {
    static {
        try {
            System.loadLibrary("veos_vhaljni");
            Log.i("VhalNative", "Loaded veos_vhaljni");
        } catch (UnsatisfiedLinkError e) {
            Log.e("VhalNative", "Failed to load veos_common_vhaljni", e);
        }
    }

    private static long clientPtr = 0;

    public static void init() {
        clientPtr = vhal_connect();
        if (clientPtr == 0) {
            throw new IllegalStateException("vhal_connect() returned 0");
        }
    }

    public static void release() {
        if (clientPtr != 0) {
            vhal_disconnect(clientPtr);
            clientPtr = 0;
        }
    }

    public static void setInt32(int propId, int areaId, int value) {
        if (clientPtr == 0)
            throw new IllegalStateException("not initialized");
        int result = vhal_set_int32(clientPtr, propId, areaId, value);
        if (result != 0) {
            throw new RuntimeException("Failed to set int32 property " + propId + " (err=" + result + ")");
        }
    }

    public static void setBool(int propId, int areaId, boolean value) {
        if (clientPtr == 0)
            throw new IllegalStateException("not initialized");
        int result = vhal_set_bool(clientPtr, propId, areaId, value ? 1 : 0);
        if (result != 0) {
            throw new RuntimeException("Failed to set bool property " + propId + " (err=" + result + ")");
        }
    }

    public static int getInt32(int propId, int areaId) {
        if (clientPtr == 0)
            throw new IllegalStateException("not initialized");
        return vhal_get_int32(clientPtr, propId, areaId);
    }

    public static boolean getBool(int propId, int areaId) {
        if (clientPtr == 0)
            throw new IllegalStateException("not initialized");
        return vhal_get_bool(clientPtr, propId, areaId) == 1;
    }

    // native methods implemented in veos_vhaljni
    private static native long vhal_connect();

    private static native void vhal_disconnect(long c);

    private static native int vhal_get_int32(long c, int propId, int areaId);

    private static native int vhal_get_bool(long c, int propId, int areaId);

    // NEW natives
    private static native int vhal_set_int32(long c, int propId, int areaId, int value);

    private static native int vhal_set_bool(long c, int propId, int areaId, int value);
}
