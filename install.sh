#!/usr/bin/env bash
# VoltBill One-Command Unix/Linux/macOS Installer
# Lead Architect: Akshar Miyani (MCA 1st Sem, MUJ)
# Usage: curl -fsSL https://raw.githubusercontent.com/miyaniakshar1234/VoltBill/main/install.sh | bash

set -e

echo ""
echo "  ========================================================================="
echo "    ⚡ Installing VoltBill Utility Engine (Unix / Linux / macOS)"
echo "    Lead Architect: Akshar Miyani | MCA (1st Sem) | Manipal University Jaipur"
echo "  ========================================================================="
echo ""

INSTALL_DIR="${HOME}/.local/bin"
mkdir -p "$INSTALL_DIR"

REPO="miyaniakshar1234/VoltBill"
OS="$(uname -s | tr '[:upper:]' '[:lower:]')"
ARCH="$(uname -m)"

case "$ARCH" in
    x86_64)  ARCH_NAME="x64" ;;
    aarch64|arm64) ARCH_NAME="arm64" ;;
    *)       ARCH_NAME="x64" ;;
esac

TAR_NAME="voltbill-${OS}-${ARCH_NAME}.tar.gz"
RELEASE_URL="https://github.com/${REPO}/releases/latest/download/${TAR_NAME}"

echo "  [1/3] Fetching VoltBill binary..."
if [ -f "bin/voltbill" ]; then
    echo "  [*] Using locally compiled binary..."
    cp "bin/voltbill" "${INSTALL_DIR}/voltbill"
    chmod +x "${INSTALL_DIR}/voltbill"
else
    echo "  [*] Downloading from ${RELEASE_URL}..."
    TMP_DIR="$(mktemp -d)"
    if curl -fsSL "$RELEASE_URL" -o "${TMP_DIR}/${TAR_NAME}" 2>/dev/null; then
        tar -xzf "${TMP_DIR}/${TAR_NAME}" -C "$TMP_DIR"
        cp "${TMP_DIR}/voltbill" "${INSTALL_DIR}/voltbill"
        chmod +x "${INSTALL_DIR}/voltbill"
        rm -rf "$TMP_DIR"
    else
        echo "  [!] Online release not yet published. Attempting source compilation..."
        if command -v gcc >/dev/null 2>&1; then
            mkdir -p bin
            gcc -O2 -Wall -std=c99 -o "${INSTALL_DIR}/voltbill" src/*.c -lm
            chmod +x "${INSTALL_DIR}/voltbill"
        elif command -v clang >/dev/null 2>&1; then
            mkdir -p bin
            clang -O2 -Wall -std=c99 -o "${INSTALL_DIR}/voltbill" src/*.c -lm
            chmod +x "${INSTALL_DIR}/voltbill"
        else
            echo "  [ERROR] Neither release binary nor C compiler found."
            exit 1
        fi
    fi
fi

echo "  [2/3] Verifying PATH..."
case ":$PATH:" in
    *":${INSTALL_DIR}:"*) ;;
    *)
        echo "  [+] Adding ${INSTALL_DIR} to your PATH..."
        SHELL_RC="${HOME}/.bashrc"
        if [ -n "$ZSH_VERSION" ] || [ -f "${HOME}/.zshrc" ]; then
            SHELL_RC="${HOME}/.zshrc"
        fi
        echo "export PATH=\"\$PATH:${INSTALL_DIR}\"" >> "$SHELL_RC"
        echo "  [*] Run: source $SHELL_RC or restart your shell."
        ;;
esac

echo "  [3/3] Installation Complete!"
echo ""
echo "  ========================================================================="
echo "    ✓ SUCCESS! VoltBill is installed at ${INSTALL_DIR}/voltbill"
echo "    Run it using:"
echo "        voltbill"
echo "        voltbill --demo"
echo "  ========================================================================="
echo ""
