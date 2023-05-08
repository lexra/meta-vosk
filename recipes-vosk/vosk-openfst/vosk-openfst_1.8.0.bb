LICENSE = "Apache-2.0"
LIC_FILES_CHKSUM = "file://COPYING;md5=973381090441f4eb420224655e05e064"

SRC_URI = "git://github.com/alphacep/openfst.git;protocol=https;branch=master \
           file://0001-build-fixes-for-bitbake.patch \
"

SRCREV = "7dfd808194105162f20084bb4d8e4ee4b65266d5"
S = "${WORKDIR}/git"

inherit autotools python3native

TARGET_CXXFLAGS += " -std=c++17 "

# Specify any options you want to pass to the configure script using EXTRA_OECONF:
EXTRA_OECONF = "--enable-static --enable-shared --enable-far --enable-ngram-fsts --enable-lookahead-fsts --with-pic --disable-bin"

INSANE_SKIP_${PN} += " installed-vs-shipped "
INSANE_SKIP_${PN}-dev += " installed-vs-shipped "

FILES_${PN} += " \
	${libdir}/fst   \
	${libdir}/fst/arc_lookahead-fst.so \
	${libdir}/fst/ilabel_lookahead-fst.so \
	${libdir}/fst/ngram-fst.so \
	${libdir}/fst/olabel_lookahead-fst.so \
"
