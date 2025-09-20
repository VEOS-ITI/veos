#ifndef TEA5767_HAL_H
#define TEA5767_HAL_H

#include <string>

class Tea5767Hal {
public:
    // Construct with default sysfs base path
    explicit Tea5767Hal(const std::string& basePath = "/sys/class/i2c-dev/i2c-1/device/1-0060/tea5767/");

    // Control
    void mute();
    void unmute();
    bool isMuted();

    // Station control
    int getStation();              // returns kHz
    void tune(int frequencyKHz);   // set station in kHz

    // Get station list (if available from driver)
    std::string getStationList();

private:
    std::string basePath_;

    void writeToFile(const std::string& file, const std::string& value);
    std::string readFromFile(const std::string& file);
};

#endif // TEA5767_HAL_H
