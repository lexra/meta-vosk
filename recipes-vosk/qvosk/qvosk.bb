FILESEXTRAPATHS_prepend := "${THISDIR}/files:"
LICENSE = "CLOSED"

DEPENDS = " \
	qtbase \
	qtmultimedia \
	vosk \
	alsa-lib \
"

inherit qmake5

S = "${WORKDIR}"

SRC_URI_append = " \
	file://qvosk.pro \
	file://MainDialog.ui \
	file://list.h \
	file://main.cpp \
	file://main.h \
	file://MainDialog.cpp \
	file://MainDialog.h \
	file://QVosk.cpp \
	file://QVosk.h \
"

do_configure () {
	${OE_QMAKE_QMAKE} ${S}/qvosk.pro
}

do_install() {
        install -d ${D}${bindir}
        install -m 755 ${WORKDIR}/build/qvosk ${D}${bindir}
}
