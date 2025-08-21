# VEOS packages
PRODUCT_PACKAGES += \
	veos_vhal \
	VEOSDashboard \
	VEOSTelemetry \
	veos_common_vhaljni \
	libveos_vhalshim
    
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

PRODUCT_COPY_FILES += \
    vendor/QuaDroid/MUMD/input_screen/input-port-associations.xml:$(TARGET_COPY_OUT_VENDOR)/etc/input-port-associations.xml
    
BOARD_KERNEL_CMDLINE += androidboot.lcd_density=200
PRODUCT_BROKEN_VERIFY_USES_LIBRARIES := true
BUILD_WITH_CLUSTER_OS_DOUBLE := false
ENABLE_CLUSTER_OS_DOUBLE:= false

PRODUCT_PROPERTY_OVERRIDES += \
    persist.sys.cluster.double=0 \
    ro.sys.cluster.double=0
