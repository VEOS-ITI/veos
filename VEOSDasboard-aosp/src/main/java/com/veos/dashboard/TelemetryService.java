package com.veos.dashboard;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.location.Location;
import android.location.LocationManager;
import android.content.Context;

import java.util.HashMap;
import java.util.Map;
import android.os.Handler;
import android.os.Looper;

public class TelemetryService extends Service {
    private static final String TAG = "TelemetryService";
    private Handler handler = new Handler(Looper.getMainLooper());
    private static final long FETCH_INTERVAL_MS = 5000; // every 5 seconds

        // Track current values to detect changes
    private int currentSpeed = 0;
    private int currentDistance = 0;
    private int currentDuration = 0;
    private int currentBattery = 100;
    private int currentTemp = 22;

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "onCreate() started, about to init VHAL");
        VhalNative.init();
        Log.i(TAG, "VHAL init complete, now sending telemetry...");
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        VhalNative.release();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void addLocation(Map<String, Object> payload) {
        try {
            LocationManager lm = (LocationManager) getSystemService(Context.LOCATION_SERVICE);
            if (lm != null) {
                Location loc = lm.getLastKnownLocation(LocationManager.GPS_PROVIDER);
                if (loc == null) {
                    loc = lm.getLastKnownLocation(LocationManager.NETWORK_PROVIDER);
                }
                if (loc != null) {
                    payload.put("latitude", loc.getLatitude());
                    payload.put("longitude", loc.getLongitude());
                } else {
                    Log.w(TAG, "No last known location available");
                }
            }
        } catch (SecurityException se) {
            Log.e(TAG, "Location permission missing", se);
        } catch (Exception e) {
            Log.e(TAG, "Failed to get location", e);
        }
    }

    private void addInt(Map<String, Object> payload, int propId, int areaId) {
        try {
            int value = VhalNative.getInt32(propId, areaId);
            payload.put(PropMapping.getName(propId), value);
        } catch (Exception e) {
            Log.w(TAG, "Failed to read int32 for " + propId, e);
        }
    }

    private void addBool(Map<String, Object> payload, int propId, int areaId) {
        try {
            boolean value = VhalNative.getBool(propId, areaId);
            payload.put(PropMapping.getName(propId), value);
        } catch (Exception e) {
            Log.w(TAG, "Failed to read bool for " + propId, e);
        }
    }

}
