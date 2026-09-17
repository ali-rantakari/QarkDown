#!/bin/bash

set -euo pipefail

echo_err() {
    echo "$@" >&2
}

QTVERSION="6.11.2"
QT_SDK_PATH="${HOME}/Qt"

echo "Remember to create a _fresh_ release build in Qt Creator first!"
echo "Assuming Qt SDK is at: ${QT_SDK_PATH}"
echo "Using Qt version: ${QTVERSION}"
echo "Press return to continue."
read

# ------------------------------------------------------------
# Find tools

find_qarkdown_app() {
    find . -name 'qarkdown.app' | grep '\_Release' | grep "${1//./_}" | head -n 1 || echo ''
}

find_macdeployqt() {
    find "$1" -name macdeployqt | head -n 1
}

APPBUNDLE=$(find_qarkdown_app "$QTVERSION")
MACDEPLOYQT=$(find_macdeployqt "$QT_SDK_PATH")

if [ "$APPBUNDLE" == "" ];then
    echo_err "Error: cannot find a release build for given Qt version."
    exit 1
fi
if [ "$MACDEPLOYQT" == "" ];then
    echo_err "Error: cannot find macdeployqt."
    exit 1
fi

if ! command -v security >/dev/null 2>&1;then
    echo_err "Error: cannot find the macOS security command."
    exit 1
fi
if ! command -v codesign >/dev/null 2>&1;then
    echo_err "Error: cannot find codesign."
    exit 1
fi

# ------------------------------------------------------------
# Find code-signing identity

echo "Available code-signing identities:"
SIGNING_IDENTITIES=$(security find-identity -v -p codesigning)
if ! grep -qE '^[[:space:]]*[0-9]+\)' <<< "$SIGNING_IDENTITIES";then
    echo_err "Error: no code-signing identities were found in the keychain."
    exit 1
fi

echo "$SIGNING_IDENTITIES"

SIGNING_IDENTITY_COUNT=$(grep -Ec '^[[:space:]]*[0-9]+\)' <<< "$SIGNING_IDENTITIES")
if [ "$SIGNING_IDENTITY_COUNT" -eq 1 ];then
    SIGNING_IDENTITY=$(awk '/^[[:space:]]*[0-9]+\)/ { print $2; exit }' <<< "$SIGNING_IDENTITIES")
    echo "Using the only available signing identity: $SIGNING_IDENTITY"
else
    echo "Enter the certificate name or SHA-1 hash to use for signing."
    read -r -p "Signing identity: " SIGNING_IDENTITY
fi

if [ -z "$SIGNING_IDENTITY" ];then
    echo_err "Error: a signing identity is required."
    exit 1
fi

# ------------------------------------------------------------
# Run macdeployqt

if [ -e "${APPBUNDLE}/Contents/Frameworks/QtCore.framework" ];then
    echo ">>>>>>> Looks like macdeployqt has already been run; skipping."
else
    echo ">>>>>>> Running macdeployqt"
    "${MACDEPLOYQT}" "$APPBUNDLE"

    echo ">>>>>>> Removing unused frameworks from app bundle"
    for framework in "$APPBUNDLE"/Contents/Frameworks/*.framework; do
        bn=$(basename "${framework}")
        bn=${bn%.*} # remove extension
        case "$bn" in
            QtCore|QtGui|QtWidgets|QtNetwork|QtWebKit|QtWebKitWidgets|QtQuick|QtQml|QtOpenGL|\
            QtPrintSupport|QtSql|QtV8|QtSensors|QtDBus)
                # ^-- keep these --^
                ;;
            *)
                # remove all other frameworks
                rm -rf "$framework"
                ;;
        esac
    done
fi

# ------------------------------------------------------------
# Code-sign

echo ">>>>>>> Code-signing app with identity: $SIGNING_IDENTITY"
codesign --deep --force --verbose --sign "$SIGNING_IDENTITY" "$APPBUNDLE"

# ------------------------------------------------------------
# Create archive

TEMP_DIR="QarkDown"
echo ">>>>>>> Making temp installation directory"
mkdir -p "$TEMP_DIR"

ROOTDIR=$(pwd)
BUNDLEBASENAME=$(basename "$APPBUNDLE")
BUNDLEDIRNAME=$(dirname "$APPBUNDLE")

echo ">>>>>>> Copying .app to temp installation directory"
cp -R "$APPBUNDLE" "$TEMP_DIR/QarkDown.app"

echo ">>>>>>> Copying other files to temp installation directory"
cp "README.md" "$TEMP_DIR/."
cp "LICENSE.md" "$TEMP_DIR/."

VERSION=$(./get_version.py)
ZIPFILE="QarkDown-macOS-${VERSION}.zip"
if [ -e "$ZIPFILE" ];then
    echo ">>>>>>> Deleting existing zip file: $ZIPFILE"
    rm -f "$ZIPFILE"
fi

echo ">>>>>>> Zipping app to: $ZIPFILE"
zip -r "${ZIPFILE}" "$TEMP_DIR"

echo ">>>>>>> Removing the temp installation directory"
rm -rf "$TEMP_DIR"
