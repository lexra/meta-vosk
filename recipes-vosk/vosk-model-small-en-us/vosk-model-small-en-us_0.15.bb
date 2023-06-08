FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

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
SRC_URI[license.sha256sum] = "69e091d43baae61f93c586d939ff7c1997415964621614add8aed06590193f47"

do_install() {
	install -d ${D}/usr/share/vosk
	cp -Rfv ${WORKDIR}/vosk-model-small-en-us-0.15 ${D}/usr/share/vosk
	cp -Rfv ${WORKDIR}/vosk-model-small-cn-0.22 ${D}/usr/share/vosk
	cp -Rfv ${WORKDIR}/model.conf ${D}/usr/share/vosk/vosk-model-small-cn-0.22/conf
	cp -Rfv ${WORKDIR}/mfcc.conf ${D}/usr/share/vosk/vosk-model-small-cn-0.22/conf
}

FILES:${PN} += " \
	/usr/share/vosk \
"

RPROVIDES:${PN} += "virtual/vosk-model"
