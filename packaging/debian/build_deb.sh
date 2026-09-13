#!/usr/bin/env bash
# VoltBill Debian / Ubuntu .deb Package Builder
# Lead Architect: Akshar Miyani
# Usage: ./packaging/debian/build_deb.sh

set -e

VERSION="2.0.0"
PKG_DIR="voltbill_${VERSION}_amd64"

echo "=== Building VoltBill Debian Package (${PKG_DIR}.deb) ==="

# Build binary if not present
if [ ! -f "bin/voltbill" ]; then
    echo "[+] Compiling voltbill binary..."
    mkdir -p bin
    gcc -O2 -Wall -Wextra -std=c99 -o bin/voltbill src/*.c -lm
fi

# Clean prior build tree
rm -rf "$PKG_DIR" "${PKG_DIR}.deb"

# Create Debian directory layout
mkdir -p "${PKG_DIR}/DEBIAN"
mkdir -p "${PKG_DIR}/usr/local/bin"
mkdir -p "${PKG_DIR}/etc/voltbill"
mkdir -p "${PKG_DIR}/usr/share/man/man1"
mkdir -p "${PKG_DIR}/usr/share/doc/voltbill"

# Copy package control metadata
cp packaging/debian/control "${PKG_DIR}/DEBIAN/control"

# Copy binary and configuration
cp bin/voltbill "${PKG_DIR}/usr/local/bin/voltbill"
chmod 755 "${PKG_DIR}/usr/local/bin/voltbill"

if [ -f "config/tariffs.cfg" ]; then
    cp config/tariffs.cfg "${PKG_DIR}/etc/voltbill/tariffs.cfg"
fi

# Copy documentation and man page if present
if [ -f "man/voltbill.1" ]; then
    cp man/voltbill.1 "${PKG_DIR}/usr/share/man/man1/voltbill.1"
fi
cp README.md "${PKG_DIR}/usr/share/doc/voltbill/README.md"
cp LICENSE "${PKG_DIR}/usr/share/doc/voltbill/copyright"

# Build Debian archive
dpkg-deb --build --root-owner-group "$PKG_DIR"

echo "=== Package successfully built: ${PKG_DIR}.deb ==="
echo "Install using:"
echo "    sudo apt install ./${PKG_DIR}.deb"
echo "or:"
echo "    sudo dpkg -i ${PKG_DIR}.deb"
