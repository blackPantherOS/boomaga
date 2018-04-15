#!/bin/sh
BUILD_DIR=../build
INSTALL_DIR=./OUT

echo "BUILD DIR:   ${BUILD_DIR}"
echo "INSTALL DIR: ${INSTALL_DIR}"
set -e

mkdir -p ${BUILD_DIR} 

cd ${BUILD_DIR}

if [ `uname` = "Darwin" ]; then
    INSTALL_PREFIX="${INSTALL_DIR}"
else
    INSTALL_PREFIX="${INSTALL_DIR}/usr"
fi

cmake \
	-DCMAKE_INSTALL_PREFIX=${INSTALL_PREFIX} \
	-DCUPS_BACKEND_DIR=${INSTALL_PREFIX}/lib/cups/backend \
	-DCUPS_PPD_DIR=${INSTALL_PREFIX}/share/ppd/boomaga \
	-DDBUS_MACINE_ID_DIR=${INSTALL_DIR}/var/lib/dbus \
	..

make && make install
