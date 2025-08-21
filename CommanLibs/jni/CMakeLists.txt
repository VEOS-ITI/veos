cmake_minimum_required(VERSION 3.22.1)
project(veos_vhaljni)

add_library(veos_vhaljni SHARED
    vhal_jni.cpp
)

# Point to headers (so JNI can include vhal_shim.h)
target_include_directories(veos_vhaljni
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Declare shim as prebuilt
add_library(veos_vhalshim SHARED IMPORTED)
set_target_properties(veos_vhalshim PROPERTIES
    IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/../jniLibs/arm64-v8a/libveos_vhalshim.so
)

# Link JNI → shim + system libs
target_link_libraries(veos_vhaljni
    veos_vhalshim
    android
    log
)

