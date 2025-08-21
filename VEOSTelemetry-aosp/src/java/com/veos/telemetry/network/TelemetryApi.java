package com.veos.telemetry.network;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.POST;
import java.util.Map;
import retrofit2.http.GET;
import retrofit2.http.Path;

public interface TelemetryApi {
    @POST("telemetry/")
    Call<Void> sendTelemetry(@Body Map<String, Object> payload);

    @GET("vehicle-readings/")
    Call<Map<String, Object>> fetchControls(@Path("id") String vehicleId);
}
