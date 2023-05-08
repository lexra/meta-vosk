FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}:"

SUMMARY = "Kaldi Speech Recognition Toolkit"
HOMEPAGE = "http://kaldi-asr.org/"
LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://../COPYING;md5=a10e448a64dbd3723ff3fb2f397fba2e \
                    file://doc/legal.dox;md5=3cba845003f27e67da70faa5da924c1e"

SRC_URI = " \
	git://github.com/alphacep/kaldi.git;protocol=https;branch=vosk \
	file://configure.patch \
"

PV = "1.0+git${SRCPV}"
SRCREV = "a25f216f5ce4eec5e45a6ab7651e20c9840a05cd"

S = "${WORKDIR}/git/src"

DEPENDS += "openblas vosk-openfst"
inherit python3native

ALLOW_EMPTY_${PN} = "1"

PARALLEL_MAKE = "-j8"

MYCONF = "--mathlib=OPENBLAS --shared --use-cuda=no --fst-root=${STAGING_INCDIR}/.. --fst-version=1.8.0 --openblas-root=${STAGING_INCDIR}/.. "

# remove x86-specific optimizations
do_configure:prepend:aarch64(){
	cd ${S}
	sed -i -e "s#-msse -msse2##g" ${S}/makefiles/linux_openblas.mk
	sed "951s|/lib/|/lib64/|" -i ${S}/configure
	sed "952s|/lib/|/lib64/|" -i ${S}/configure
	sed "s|^check_for_slow_expf;|#check_for_slow_expf;|" -i ${S}/configure
}

do_configure:prepend:arm(){
	sed -i -e "s#-msse -msse2##g" ${S}/makefiles/linux_openblas.mk
}

do_configure() {
	./configure ${MYCONF}
	sed "s/-isystem//g" -i kaldi.mk
	sed "s/^ARCH = x86_64/ARCH = aarch64/g" -i kaldi.mk
}

do_compile() {
	oe_runmake SDKTARGETSYSROOT=${STAGING_DIR_TARGET} ${PARALLEL_MAKE}
}

do_install() {
	install -d ${D}${libdir}
	for i in lib/*.so ; do
		install -m 0644 ${i} ${D}${libdir}/
	done
	#for i in */*.a ; do
		#install -m 0644 ${i} ${D}${libdir}/
	#done

	for j in base chain decoder feat fstext gmm gst-plugin hmm itf ivector kws lat lm matrix nnet nnet2 nnet3 online online2 rnnlm sgmm2 tfrnnlm transform tree util cudadecoder cudadecoderbin cudafeat cudamatrix ; do
		install -d ${D}${includedir}/kaldi/${j}
		for i in ${j}/*.h ; do 
			install -m 0644 ${i} ${D}${includedir}/kaldi/${j}/
		done
	done

	# make sure we have the package vosk-kaldi
	install -d ${D}/usr/share/kaldi
	echo "This is vosk-kaldi" > ${D}/usr/share/kaldi/README
}

FILES:${PN} += " \
	/usr/share/kaldi \
	/usr/share/kaldi/README \
"
ERROR_QA:remove = "rpaths"
ERROR_QA:remove = "dev-elf"
