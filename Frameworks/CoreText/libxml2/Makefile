MK_WERROR = no
SUBDIR =

# We have to trick shitty cmake into not producing broken ELF shared
# objects named *.dylib.

all: libintl
	CC=${CC} CXX=${CXX} LD=${CC} AR=${AR} RANLIB=${RANLIB} LIBTOOL=${LIBTOOL} \
	LDFLAGS="-fuse-ld=${LD} --sysroot=${RAVYN_SDKROOT} -mmacos-version-min=${MACOS_VERSION_MIN}" \
	cmake -G "Unix Makefiles" \
		-DCMAKE_C_COMPILER="${CC}" \
		-DCMAKE_CXX_COMPILER="${CXX}" \
		-DCMAKE_AR="${AR}" \
		-DCMAKE_RANLIB="${RANLIB}" \
		-DCMAKE_C_FLAGS="--sysroot=${RAVYN_SDKROOT} -mmacos-version-min=${MACOS_VERSION_MIN}" \
		-DCMAKE_SYSTEM_NAME="Darwin" \
		-DCMAKE_SHARED_LIBRARY_VERSION_SCRIPT_FLAG="" \
		-DCMAKE_SHARED_LIBRARY_SONAME_FLAG="" \
		-DCMAKE_SHARED_LINKER_FLAGS="${LDFLAGS}" \
		-DBUILD_SHARED_LIBS=OFF \
		-DPKG_CONFIG_EXECUTABLE=/usr/bin/true \
		-DCMAKE_INSTALL_PREFIX=/usr \
		-DLIBXML2_WITH_SAX1=ON \
		-DLIBXML2_WITH_PYTHON=OFF \
		-DIconv_LIBRARY=/usr/lib/x86_64-linux-gnu/libc.so \
		-DIconv_INCLUDE_DIR=/usr/include \
		-DLIBLZMA_LIBRARY=/usr/lib/x86_64-linux-gnu/liblzma.so \
		-DLIBLZMA_INCLUDE_DIR=/usr/include \
		-DLIBLZMA_HAS_AUTO_DECODER=ON -DLIBLZMA_HAS_EASY_ENCODER=ON \
		-DLIBLZMA_HAS_LZMA_PRESET=ON \
		-DZLIB_LIBRARY=${ROOT_BINARY_DIR}${ROOT_SOURCE_DIR}/Libraries/zlib/libz.a \
		-DZLIB_INCLUDE_DIR=${ROOT_SOURCE_DIR}/Libraries/zlib \
		-S ${.CURDIR} -B ${.OBJDIR}
	${MAKE} -C ${.OBJDIR}
	${MAKE} install DESTDIR=${SYSROOT_DIR}
	${MAKE} install DESTDIR=${RAVYN_SDKROOT}



libintl: ${.CURDIR:H}/gettext-tiny/Makefile
	CC=${CC} CXX=${CXX} LD=${CC} AR=${AR} RANLIB=${RANLIB} \
	NM=${NM} LIBTOOL=${LIBTOOL} \
	LDFLAGS="-fuse-ld=${LD} --sysroot=${RAVYN_SDKROOT} -mmacos-version-min=${MACOS_VERSION_MIN} -L/usr/lib/x86_64-linux-gnu" \
	CFLAGS="--sysroot=${RAVYN_SDKROOT} -mmacos-version-min=${MACOS_VERSION_MIN} -I/usr/include" \
	MAKEFLAGS="" ${GMAKE} LIBINTL=1 -C ${.CURDIR:H}/gettext-tiny
	cp -fv ${.CURDIR:H}/gettext-tiny/libintl.a ${RAVYN_SDKROOT}/usr/lib
	cp -fv ${.CURDIR:H}/gettext-tiny/msgfmt ${.CURDIR:H}/gettext-tiny/msgmerge \
		${SYSROOT_DIR}/usr/bin/


.include <rvn.framework.mk>
.include <bsd.subdir.mk>
