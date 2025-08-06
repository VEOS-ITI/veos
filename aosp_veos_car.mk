
# Copyright (C) 2021-2023 KonstaKANG

# SPDX-License-Identifier: Apache-2.0
#

# Inherit device configuration
$(call inherit-product, device/brcm/rpi5/aosp_rpi5_car.mk)
veos_mk_files := $(wildcard vendor/veos/*.mk)

PRODUCT_OTA_ENFORCE_VINTF_KERNEL_REQUIREMENTS := false


$(foreach f, $(veos_mk_files), $(call inherit-product, $(f)))

PRODUCT_DEVICE := rpi5
PRODUCT_NAME := aosp_veos_car
PRODUCT_BRAND := Raspberry
PRODUCT_MODEL := Pi 5
PRODUCT_MANUFACTURER := Raspberry
