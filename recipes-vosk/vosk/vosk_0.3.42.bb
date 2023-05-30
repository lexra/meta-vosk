SUMMARY = "Offline open source speech recognition API based on Kaldi and Vosk"
HOMEPAGE = "https://github.com/alphacep/vosk-api"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://../COPYING;md5=d09bbd7a3746b6052fbd78b26a87396b"

SRC_URI = " \
	git://github.com/alphacep/vosk-api;protocol=https;branch=master \
	file://0001-Build-fixes-for-shared-library-under-bitbake.patch \
	file://makefile \
	file://list.h \
	file://test_speech*.c \
	file://test_vosk.df \
	file://lyrics.wav \
	file://test_microphone \
"

PV = "0.3.42+git${SRCPV}"
SRCREV = "b1b216d4c87d708935f1601287fe502aa11ee4a9"

S = "${WORKDIR}/git/src"

#DEPENDS += " vosk-kaldi vosk-openfst openblas alsa-utils"
DEPENDS += " vosk-kaldi vosk-openfst alsa-utils"

RDEPENDS:${PN} += " \
"

CFLAGS:append = " -I${STAGING_INCDIR}/kaldi -g "
LDFLAGS:remove = "-Wl,--as-needed"

do_configure() {
	:
}

do_compile_prepend() {
	cp -fv ${WORKDIR}/makefile ${WORKDIR}/git/c/Makefile
	cp -fv ${WORKDIR}/list.h ${WORKDIR}/git/c
	cp -fv ${WORKDIR}/test_speech*.c ${WORKDIR}/git/c

	git -C ${WORKDIR}/git checkout c/test_vosk.c
	patch -p1 -l -f --fuzz 3 -d ${WORKDIR}/git -i ${WORKDIR}/test_vosk.df
}

do_compile() {
	make -C ${S} \
		KALDI_ROOT=${STAGING_INCDIR}/kaldi \
		OPENFST_ROOT=${STAGING_INCDIR} \
		OPENBLAS_ROOT=${STAGING_INCDIR} \
		USE_SHARED=1 EXTRA_CFLAGS="${CFLAGS}" EXTRA_LDFLAGS="${LDFLAGS}" \
		${PARALLEL_MAKE}
	make -C ${S}/../c
}

do_install(){
	cd ${WORKDIR}/git/src
	install -d ${D}${libdir}
	install -m 0644 libvosk.so.0.3.42 ${D}${libdir}
	cd ${D}${libdir}
	ln -sf libvosk.so.0.3.42 libvosk.so
	ln -sf libvosk.so.0.3.42 libvosk.so.0
	cd ${WORKDIR}/git/src

	install -d ${D}${includedir}/vosk
	for i in *.h ; do
		install -m 0644 $i ${D}${includedir}/vosk/
	done

	install -d ${D}${bindir}
	install -m 0755 ${WORKDIR}/git/c/test_vosk ${D}${bindir}
	install -m 0755 ${WORKDIR}/git/c/test_speech ${D}${bindir}
	install -m 0755 ${WORKDIR}/git/c/test_speech_en ${D}${bindir}
	install -m 0755 ${WORKDIR}/git/c/test_speech_zh ${D}${bindir}
	install -m 0755 ${WORKDIR}/test_microphone ${D}${bindir}

	install -d install -d ${D}${prefix}/share/vosk
	install -m 644 ${WORKDIR}/lyrics.wav ${D}${prefix}/share/vosk
}

INSANE_SKIP_${PN} = "ldflags"
INSANE_SKIP_${PN}-dev = "ldflags"

ERROR_QA:remove = "dev-deps"
