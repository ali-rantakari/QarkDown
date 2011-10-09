#!/bin/sh

echo "Remember to create a fresh release build, using Qt 4.8, in Qt Creator first!"
read
echo
~/QtSDK/Desktop/Qt/4.8.0/gcc/bin/macdeployqt qarkdown-build-desktop/qarkdown.app

