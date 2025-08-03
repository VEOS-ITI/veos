SUMMARY = "Logitech Joystick SOME/IP Sender Application"
DESCRIPTION = "Application to read Logitech joystick data, process it, and send it via SOME/IP."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://simulation_main.cpp \
    file://vehicle_data_sender.cpp \
    file://vehicle_data_sender.hpp \
    file://vehicle_data_receiver.cpp \
    file://vehicle_data_receiver.hpp \
    file://poky/avisio_ivi_data.proto \
    file://logetic-sender.json \
    file://logetic-sender.service \
"

S = "${WORKDIR}"

DEPENDS = "vsomeip protobuf-native protobuf"

inherit systemd

SYSTEMD_SERVICE:${PN} = "logetic-sender.service"

do_configure() {
    # Nothing to do
    :
}

do_compile() {
    mkdir -p ${S}/poky

    protoc --proto_path=${S}/poky --cpp_out=${S}/poky ${S}/poky/avisio_ivi_data.proto

    ${CXX} ${CXXFLAGS} ${LDFLAGS} -std=c++17 \
        -I${S}/poky \
        -I${STAGING_INCDIR}/protobuf \
        -I${STAGING_INCDIR}/google \
        simulation_main.cpp \
        vehicle_data_sender.cpp \
        vehicle_data_receiver.cpp \
        poky/avisio_ivi_data.pb.cc \
        -lvsomeip3 -lpthread -lprotobuf \
        -o simulation_main
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 simulation_main ${D}${bindir}/logetic-sender

    install -d ${D}${sysconfdir}/vsomeip
    install -m 0644 ${S}/logetic-sender.json ${D}${sysconfdir}/vsomeip/

    install -d ${D}${systemd_system_unitdir}
    install -m 0644 ${S}/logetic-sender.service ${D}${systemd_system_unitdir}/
}

FILES:${PN} += "${sysconfdir}/vsomeip/logetic-sender.json"

