FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://${WORKDIR}/models.md;beginline=32;endline=32;md5=bb5cd71a409da2b13fa3bcb67a3573d1"

SRC_URI = " \
    https://alphacephei.com/vosk/models/vosk-model-small-cn-0.22.zip;name=cn_model \
    https://alphacephei.com/vosk/models/vosk-model-small-en-us-0.15.zip;name=en_model \
    https://raw.githubusercontent.com/alphacep/vosk-space/master/models.md;name=license \
    file://model.conf \
    file://mfcc.conf \
"

SRC_URI[cn_model.sha256sum] = "3af8b0e7e0f835ae9d414ce5df580237a3cfb08d586c9fbbb0f7ff29ad5b14ba"
SRC_URI[en_model.sha256sum] = "30f26242c4eb449f948e42cb302dd7a686cb29a3423a8367f99ff41780942498"
SRC_URI[license.sha256sum] = "be3599963ee12a6424f194c50f5358e190a19f125a27611f77b590549d8e4915"

S = "${WORKDIR}"

PACKAGES =+ " \
    ${PN}-small-en-us \
    ${PN}-small-cn \
"

do_install() {
    install -d ${D}${datadir}/vosk

    cp -R ${S}/vosk-model-small-en-us-0.15 \
        ${D}${datadir}/vosk/

    cp -R ${S}/vosk-model-small-cn-0.22 \
        ${D}${datadir}/vosk/

    install -m 0644 ${WORKDIR}/model.conf \
        ${D}${datadir}/vosk/vosk-model-small-cn-0.22/conf/

    install -m 0644 ${WORKDIR}/mfcc.conf \
        ${D}${datadir}/vosk/vosk-model-small-cn-0.22/conf/
}

FILES:${PN}-small-en-us = " \
    ${datadir}/vosk/vosk-model-small-en-us-0.15 \
"

FILES:${PN}-small-cn = " \
    ${datadir}/vosk/vosk-model-small-cn-0.22 \
"

RPROVIDES:${PN}-small-en-us += "vosk-model-small-en-us"
RPROVIDES:${PN}-small-cn += "vosk-model-small-cn"
