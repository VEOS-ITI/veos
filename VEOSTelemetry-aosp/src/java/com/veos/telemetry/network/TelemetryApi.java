package com.veos.telemetry.network;

import retrofit2.Call;
import retrofit2.http.Body;
import retrofit2.http.POST;
import java.util.Map;

public interface TelemetryApi {
    @POST("telemetry/")
    Call<Void> sendTelemetry(@Body Map<String, Object> payload);
}
