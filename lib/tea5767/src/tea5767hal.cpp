#include <fstream>
#include <string>
#include <iostream>
#include <stdexcept>
#include <android-base/logging.h>
#include <cmath>
#include <cstdlib>
#include "tea5767hal.hpp"

Tea5767Hal::Tea5767Hal(const std::string& basePath)
    : basePath_(basePath) {}

void Tea5767Hal::mute() {
    int ret = system("echo 1 > /sys/class/i2c-dev/i2c-1/device/1-0060/tea5767/mute");
    if (ret != 0) {
        LOG(ERROR) << "Failed to mute TEA5767, system() returned: " << ret;
    }
}

void Tea5767Hal::unmute() {
    int ret = system("echo 0 > /sys/class/i2c-dev/i2c-1/device/1-0060/tea5767/mute");
    if (ret != 0) {
        LOG(ERROR) << "Failed to unmute TEA5767, system() returned: " << ret;
    }
}

bool Tea5767Hal::isMuted() {
    std::string val = readFromFile("mute");
    return val == "1";   // sysfs usually outputs just "0" or "1"
}

int Tea5767Hal::getStation() {
    return std::stoi(readFromFile("station"));  // kHz
}

void Tea5767Hal::tune(int frequencyKHz) {
    std::cout<< "frequencyKHz" << frequencyKHz << std::endl;
    std::ofstream file(basePath_ + "station");
    if (file.is_open()) {
        file << frequencyKHz; 
        file << 0;  // write as plain integer text
        file.close();
    } else {
        LOG(ERROR) << "Failed to open station file for writing";
    }
}

std::string Tea5767Hal::getStationList() {
    // Only works if driver exports "stationlist"
    return readFromFile("stationlist");
}

void Tea5767Hal::writeToFile(const std::string& file, const std::string& value) {
    std::ofstream ofs(basePath_ + file);
    if (!ofs.is_open()) {
        LOG(ERROR) << "Failed to open for writing: " << basePath_ + file;
        return;
    }
    ofs << value;
}

std::string Tea5767Hal::readFromFile(const std::string& file) {
    std::ifstream ifs(basePath_ + file);
    if (!ifs.is_open()) {
        LOG(ERROR) << "Failed to open for reading: " << basePath_ + file;
        return "";
    }
    std::string value;
    ifs >> value;
    return value;
}

