SUMMARY = "A minimal image with vSomeIP and a server application" 
LICENSE = "CLOSED"

inherit core-image

IMAGE_FEATURES += "splash ssh-server-dropbear package-management"

IMAGE_INSTALL:append = " vsomeip systemd systemd-analyze connman-client bash dbus dbus-tools python3-dbus glib-2.0-utils"

IMAGE_ROOTFS_EXTRA_SPACE = "5242880"
