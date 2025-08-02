package android.vendor.test;

@VintfStability
interface IVEOSService{

    float getSpeed();
    int32 getGear();
    uint32 getTemp();
    String getTime();
    uint32 getBatteryLevel();
    uint32 getEngineTemp();
    uint32 getOdometerKm();
    int32 getRightLightingStatus();
    int32 getLeftLightingStatus();
    int32 getLightingStatus();
    int32 getSeatBeltStatus();
    uint32 getTirePressure();
}