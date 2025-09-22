package com.veos.telemetry;

import java.util.HashMap;
import java.util.Map;

public class PropMapping {
    private static final Map<Integer, String> map = new HashMap<>();

    static {
        map.put(557846352, "veos_gear_selection");
        map.put(557846353, "veos_perf_vehicle_speed");
        map.put(557846354, "veos_ev_battery_level");
        map.put(557846355, "veos_ev_battery_temp");
        map.put(557846357, "veos_ev_charging_state");
        map.put(557846358, "veos_ev_charge_time_remaining");
        map.put(557846360, "veos_perf_odometer");
        map.put(557846362, "veos_trip_distance");
        map.put(557846363, "veos_trip_duration");
        map.put(555749214, "veos_headlights_state");
        map.put(555749215, "veos_high_beam_lights_state");
        map.put(555749217, "veos_ev_low_battery_warning");
        map.put(555749220, "veos_brake_system_warning");
        map.put(555749221, "veos_vehicle_ready_state");
        map.put(557846374, "veos_last_update_timestamp");
        map.put(557846375,"veos_ev_instant_efficiency");
        map.put(557846376, "veos_ev_avg_efficiency");

    }

    public static String getName(int propId) {
        return map.getOrDefault(propId, String.valueOf(propId));
    }
}
