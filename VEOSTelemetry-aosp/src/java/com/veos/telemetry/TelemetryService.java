package com.veos.telemetry;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.util.Log;
import android.location.Location;
import android.location.LocationManager;
import android.content.Context;
import com.veos.telemetry.network.ApiClient;
import com.veos.telemetry.network.TelemetryApi;

import java.util.HashMap;
import java.util.Map;

import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class TelemetryService extends Service {
    private static final String TAG = "TelemetryService";

    @Override
    public void onCreate() {
        super.onCreate();
        Log.i(TAG, "onCreate() started, about to init VHAL");
        VhalNative.init();
        Log.i(TAG, "VHAL init complete, now sending telemetry...");
        sendTelemetry();
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

    private void sendTelemetry() {
        Map<String, Object> payload = new HashMap<>();
        payload.put("vehicle_id", "rpi5-001");
        payload.put("timestamp", System.currentTimeMillis());
        addLocation(payload);
        addInt(payload, 557846352, 0);
        addInt(payload, 557846353, 0);
        addInt(payload, 557846354, 0);
        addInt(payload, 557846355, 0);
        addInt(payload, 557846357, 0);
        addInt(payload, 557846358, 0);
        addInt(payload, 557846360, 0);
        addInt(payload, 557846362, 0);
        addInt(payload, 557846363, 0);
        addBool(payload, 555749214, 0);
        addBool(payload, 555749215, 0);
        addBool(payload, 555749217, 0);
        addBool(payload, 555749220, 0);
        addBool(payload, 555749221, 0);
        addInt(payload, 557846374, 0);

        TelemetryApi api = ApiClient.getClient().create(TelemetryApi.class);
        api.sendTelemetry(payload).enqueue(new Callback<Void>() {
            @Override
            public void onResponse(Call<Void> call, Response<Void> response) {
                Log.i(TAG, "Sent telemetry, status=" + response.code());
            }

            @Override
            public void onFailure(Call<Void> call, Throwable t) {
                Log.e(TAG, "Failed to send telemetry", t);
            }
        });
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
