#ifndef VEHICLE_PROPERTIES_H
#define VEHICLE_PROPERTIES_H

#include <stdint.h>

// Define custom VHAL property IDs for the protobuf fields
// These are example values. In a real AOSP project, you would define these
// in a proper vehicle_properties.h or similar file within the VHAL implementation.

// PrimaryVehicleState properties
#define VEHICLE_PROPERTY_PRIMARY_STATE_TIMESTAMP            557846374 // veos_last_update_timestamp (used for timestamp_ms)
#define VEHICLE_PROPERTY_PRIMARY_STATE_SPEED_KMH            557846353 // veos_perf_vehicle_speed
#define VEHICLE_PROPERTY_PRIMARY_STATE_ENGINE_RPM           0x21000003 // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_PRIMARY_STATE_CURRENT_GEAR         557846352 // veos_gear_selection
#define VEHICLE_PROPERTY_PRIMARY_STATE_GEAR_MODE            0x21000005 // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_PRIMARY_STATE_FUEL_LEVEL           0x21000006 // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_PRIMARY_STATE_ENGINE_TEMP_C        0x21000007 // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_PRIMARY_STATE_STATUS_FLAGS         0x21000008 // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_PRIMARY_STATE_ODOMETER_KM          557846360 // veos_perf_odometer
#define VEHICLE_PROPERTY_PRIMARY_STATE_TRIP_DISTANCE_M      557846362 // veos_trip_distance

// SignalLightingStatus properties
#define VEHICLE_PROPERTY_LIGHTING_STATUS_TIMESTAMP          557846374 // veos_last_update_timestamp (used for timestamp_ms)
#define VEHICLE_PROPERTY_LIGHTING_STATUS_TURN_SIGNALS       0x2100000C // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_LIGHTING_STATUS_EXTERIOR_LIGHTS    555749214 // veos_headlights_state (for headlights)
#define VEHICLE_PROPERTY_LIGHTING_STATUS_HIGH_BEAM_LIGHTS   555749215 // veos_high_beam_lights_state

// SafetyStatus properties
#define VEHICLE_PROPERTY_SAFETY_STATUS_TIMESTAMP            557846374 // veos_last_update_timestamp (used for timestamp_ms)
#define VEHICLE_PROPERTY_SAFETY_STATUS_SEATBELT_FASTENED    0x2100000F // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_SAFETY_STATUS_AIRBAG_OK            0x21000010 // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_SAFETY_STATUS_TRACTION_CONTROL     0x21000011 // No direct mapping, using a placeholder
#define VEHICLE_PROPERTY_SAFETY_STATUS_LOW_BATTERY_WARNING  555749217 // veos_ev_low_battery_warning
#define VEHICLE_PROPERTY_SAFETY_STATUS_BRAKE_WARNING        555749220 // veos_brake_system_warning

#endif // VEHICLE_PROPERTIES_H

