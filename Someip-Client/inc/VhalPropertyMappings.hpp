#ifndef VHAL_PROPERTY_MAPPINGS_HPP
#define VHAL_PROPERTY_MAPPINGS_HPP

#include <stdint.h>

/**
 * @brief VHAL Property ID mappings for libveos_vhalshim
 * 
 * This file centralizes all VHAL property ID definitions used by the VhalManager.
 * These IDs correspond to the properties supported by your libveos_vhalshim library.
 */

// Core Vehicle State Properties (int32 type)
#define VEOS_GEAR_SELECTION             557846352    // Current gear selection
#define VEOS_PERF_VEHICLE_SPEED         557846353    // Vehicle speed in km/h
#define VEOS_EV_BATTERY_LEVEL           557846354    // Battery level percentage
#define VEOS_EV_BATTERY_TEMP            557846355    // Battery/Engine temperature in °C
#define VEOS_CABIN_TEMP                 557846364    // Cabin temperature in °C
#define VEOS_LAST_UPDATE_TIMESTAMP      557846374    // Last update timestamp

// Vehicle State Properties (boolean type)
#define VEOS_EV_CHARGING_STATE          557846357    // EV charging state
#define VEOS_HEADLIGHTS_STATE           555749214    // Headlights on/off state

// Signal and Lighting Properties (boolean type)
#define VEOS_TURN_SIGNAL_LEFT_STATE     555749215    // Left turn signal state
#define VEOS_TURN_SIGNAL_RIGHT_STATE    555749216    // Right turn signal state  
#define VEOS_HAZARD_LIGHTS_STATE        555749217    // Hazard lights state

// Climate Control Properties (boolean type)
#define VEOS_AC_STATE                   555749218    // Air conditioning state

// Additional VHAL Properties (for future expansion)
#define VEOS_PERF_ODOMETER              557846360    // Odometer reading in km
#define VEOS_TRIP_DISTANCE              557846362    // Trip distance in meters

// Safety and Warning Properties (boolean type)
#define VEOS_LOW_BATTERY_WARNING        555749219    // Low battery warning
#define VEOS_BRAKE_SYSTEM_WARNING       555749220    // Brake system warning

/**
 * @brief Area ID for global vehicle properties
 * Most vehicle properties use area ID 0 (global/vehicle-wide)
 */
#define VHAL_AREA_GLOBAL                0

#endif // VHAL_PROPERTY_MAPPINGS_HPP
