# VEOS packages
PRODUCT_PACKAGES += \
    veos_vhal \
    VEOSDashboard \
    VEOSTelemetry \
    libveos_vhalshim \
    libvsomeip3 \
    libvsomeip3-cfg \
    libvsomeip3-sd \
    libvsomeip3-e2e \
    libcommonapi_someip \
    libcommonapi \
    E01HelloWorldClient \
    
 
    
PRODUCT_PACKAGES += \
    CarSystemUIControllsSystemBarInsetsRRO \
    CarSystemUISystemBarPersistcyBarPolicy \
    CarSystemUISystemBarPersistcyImmersive \
    CarSystemUISystemBarPersistcyImmersiveWithNav \
    CarSystemUISystemBarPersistcyNonImmersive \
    CarSystemUIBottomRRO \
    CarSystemUIBottomRoundedRRO \
    CarSystemUILeftRRO \
    CarSystemUIRightRRO \
    CarSystemUIStatusBarTranslucent \
    CarSystemUINavBarTranslucent

# <<< ADD THIS NEW SECTION TO COPY YOUR CONFIG FILES >>>
PRODUCT_COPY_FILES += \
    vendor/QuaDroid/MUMD/input_screen/input-port-associations.xml:$(TARGET_COPY_OUT_VENDOR)/etc/input-port-associations.xml \
    vendor/VEOS/Someip-Client/commonapi4someip.ini:$(TARGET_COPY_OUT_VENDOR)/etc/commonapi4someip.ini \
    vendor/VEOS/Someip-Client/vsomeip-client.json:$(TARGET_COPY_OUT_VENDOR)/etc/vsomeip-client.json \
    vendor/VEOS/Someip-Client/private.pem:$(TARGET_COPY_OUT_VENDOR)/etc/private.pem \
    vendor/VEOS/Someip-Client/public.pem:$(TARGET_COPY_OUT_VENDOR)/etc/public.pem

BOARD_KERNEL_CMDLINE += androidboot.lcd_density=200
PRODUCT_BROKEN_VERIFY_USES_LIBRARIES := true
BUILD_WITH_CLUSTER_OS_DOUBLE := false
ENABLE_CLUSTER_OS_DOUBLE:= false

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.cluster.double=0 \
    ro.sys.cluster.double=0
    ro.vendor.ld.libpath=/vendor/lib64

