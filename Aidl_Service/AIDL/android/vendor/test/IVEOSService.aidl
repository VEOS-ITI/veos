package android.vendor.test;

@VintfStability
interface IVEOSService {
    float getSpeed();
    int getGear();
    int getTemp();
    String getTime();
    int getBatteryLevel();
    int getEngineTemp();
    int getOdometerKm();
    int getRightLightingStatus();
    int getLeftLightingStatus();
    int getLightingStatus();
    int getSeatBeltStatus();
    int getTirePressure();
}
