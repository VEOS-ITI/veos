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
import java.util.List;
import java.util.Map;
import android.os.Handler;
import android.os.Looper;
import retrofit2.http.Query;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

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
        sendTelemetry();
        startFetchingControls();
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

    /**
     * Fetsh the rectricted data from the server
     */
    private void startFetchingControls() {
        handler.postDelayed(new Runnable() {
            @Override
            public void run() {
                fetchControls();
                handler.postDelayed(this, FETCH_INTERVAL_MS);
            }
        }, FETCH_INTERVAL_MS);
    }

private void fetchControls() {
    TelemetryApi api = ApiClient.getClient().create(TelemetryApi.class);
    api.fetchControls(1).enqueue(new Callback<Map<String, Object>>() {
        @Override
        public void onResponse(Call<Map<String, Object>> call, Response<Map<String, Object>> response) {
            if (response.isSuccessful() && response.body() != null) {
                Map<String, Object> responseData = response.body();
                
                // Extract the first (latest) reading from results array
                List<Map<String, Object>> results = (List<Map<String, Object>>) responseData.get("results");
                if (results != null && !results.isEmpty()) {
                    Map<String, Object> latestReading = results.get(0);
                    Log.i(TAG, "Latest reading: " + latestReading);
                    applyControls(latestReading);
                } else {
                    Log.w(TAG, "No readings found");
                }
            } else {
                Log.w(TAG, "Failed to fetch controls, code=" + response.code());
            }
        }

        @Override
        public void onFailure(Call<Map<String, Object>> call, Throwable t) {
            Log.e(TAG, "Error fetching controls", t);
        }
    });
}

    /**
     * Update VHAL properties with server values only if they differ from defaults
     */
       private void applyControls(Map<String, Object> controls) {
        try {
            if (controls.containsKey("veos_perf_vehicle_speed")) {
                int newSpeed = ((Number) controls.get("veos_perf_vehicle_speed")).intValue();
                if (newSpeed != currentSpeed) {
                    VhalNative.setInt32(557846375, 0, newSpeed);
                    currentSpeed = newSpeed;
                    Log.i(TAG, "Updated speed limit to: " + newSpeed);
                }
            }
            if (controls.containsKey("veos_trip_distance")) {
                int newDistance = ((Number) controls.get("veos_trip_distance")).intValue();
                if (newDistance != currentDistance) {
                    VhalNative.setInt32(557846362, 0, newDistance);
                    currentDistance = newDistance;
                    Log.i(TAG, "Updated distance to: " + newDistance);
                }
            }
            if (controls.containsKey("veos_trip_duration")) {
                int newDuration = ((Number) controls.get("veos_trip_duration")).intValue();
                if (newDuration != currentDuration) {
                    VhalNative.setInt32(557846363, 0, newDuration);
                    currentDuration = newDuration;
                    Log.i(TAG, "Updated duration to: " + newDuration);
                }
            }
            if (controls.containsKey("veos_ev_battery_level")) {
                int newBattery = ((Number) controls.get("veos_ev_battery_level")).intValue();
                if (newBattery != currentBattery) {
                    VhalNative.setInt32(557846354, 0, newBattery);
                    currentBattery = newBattery;
                    Log.i(TAG, "Updated battery level to: " + newBattery);
                }
            }
            if (controls.containsKey("veos_cabin_temp")) {
                int newTemp = ((Number) controls.get("veos_cabin_temp")).intValue();
                if (newTemp != currentTemp) {
                    VhalNative.setInt32(557846376, 0, newTemp);
                    currentTemp = newTemp;
                    Log.i(TAG, "Updated cabin temperature to: " + newTemp);
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Failed applying controls", e);
        }
    }

private void sendTelemetry() {
    Map<String, Object> payload = new HashMap<>();
    payload.put("vehicle_id", "rpi5-001");
    payload.put("timestamp", System.currentTimeMillis());
    
    Log.d(TAG, "=== Starting VHAL readings ===");
    addLocation(payload);
    
    // Read all VHAL properties with logging
    addInt(payload, 557846352, 0);  // gear_selection
    addInt(payload, 557846353, 0);  // vehicle_speed  
    addInt(payload, 557846354, 0);  // battery_level
    addInt(payload, 557846355, 0);  // battery_temp
    addInt(payload, 557846357, 0);  // charging_state
    addInt(payload, 557846358, 0);  // charge_time_remaining
    addInt(payload, 557846360, 0);  // odometer
    addInt(payload, 557846362, 0);  // trip_distance
    addInt(payload, 557846363, 0);  // trip_duration
    addBool(payload, 555749214, 0); // headlights_state
    addBool(payload, 555749215, 0); // high_beam_lights_state
    addBool(payload, 555749217, 0); // low_battery_warning
    addBool(payload, 555749220, 0); // brake_system_warning
    addBool(payload, 555749221, 0); // vehicle_ready_state
    addInt(payload, 557846374, 0);  // last_update_timestamp

    Log.i(TAG, "=== Complete payload to send ===");
    Log.i(TAG, "Payload: " + payload.toString());

    TelemetryApi api = ApiClient.getClient().create(TelemetryApi.class);
    api.sendTelemetry(payload).enqueue(new Callback<Void>() {
        @Override
        public void onResponse(Call<Void> call, Response<Void> response) {
            Log.i(TAG, "Sent telemetry successfully, status=" + response.code());
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
        String propName = PropMapping.getName(propId);
        payload.put(propName, value);
        Log.d(TAG, "Read from VHAL - Property: " + propName + " (ID: " + propId + ") = " + value);
    } catch (Exception e) {
        Log.w(TAG, "Failed to read int32 for " + PropMapping.getName(propId) + " (" + propId + ")", e);
    }
}

private void addBool(Map<String, Object> payload, int propId, int areaId) {
    try {
        boolean value = VhalNative.getBool(propId, areaId);
        String propName = PropMapping.getName(propId);
        payload.put(propName, value);
        Log.d(TAG, "Read from VHAL - Property: " + propName + " (ID: " + propId + ") = " + value);
    } catch (Exception e) {
        Log.w(TAG, "Failed to read bool for " + PropMapping.getName(propId) + " (" + propId + ")", e);
    }
}

}
