#include "vhal_shim.h"
#include <iostream>
#include <string>
#include <cstdlib>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "Usage:\n"
                  << "  veos_vhal get <propId> <areaId>\n"
                  << "  veos_vhal set <propId> <areaId> <value>\n";
        return 1;
    }

    std::string cmd = argv[1];
    int32_t propId = std::atoi(argv[2]);
    int32_t areaId = std::atoi(argv[3]);

    vhal_client_t* c = vhal_connect();
    if (!c) {
        std::cerr << "Failed to connect to VHAL\n";
        return 1;
    }

    if (cmd == "get") {
        int32_t value;
        int rc = vhal_get_int32(c, propId, areaId, &value);
        if (rc == 0) {
            std::cout << "Value: " << value << "\n";
        } else {
            std::cerr << "Error reading property (rc=" << rc << ")\n";
        }
    } else if (cmd == "set") {
        if (argc < 5) {
            std::cerr << "Missing value for set\n";
            vhal_disconnect(c);
            return 1;
        }
        int32_t newVal = std::atoi(argv[4]);
        int rc = vhal_set_int32(c, propId, areaId, newVal);
        if (rc == 0) {
            std::cout << "Property updated.\n";
        } else {
            std::cerr << "Error setting property (rc=" << rc << ")\n";
        }
    } else {
        std::cerr << "Unknown command: " << cmd << "\n";
    }

    vhal_disconnect(c);
    return 0;
}
