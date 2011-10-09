#!/bin/bash

echo "Remember to create a fresh release build in Qt Creator first!"
read

DEFAULT_QTVERSION="4.8.0"
echo "Type in the Qt version to use, or press enter to use the default ($DEFAULT_QTVERSION)"
read QTVERSION

[ "$QTVERSION" == "" ] && QTVERSION="$DEFAULT_QTVERSION"

find_qarkdown_app()
{
	find . -name 'qarkdown.app' | grep '__Release' | grep "${1//./_}" | head -n 1
}

APPBUNDLE=$(find_qarkdown_app "$QTVERSION")

if [ "$APPBUNDLE" == "" ];then
	echo "Error: cannot find a release build for given Qt version." >&2
	exit 1
fi

echo "Running macdeployqt..."
"${HOME}/QtSDK/Desktop/Qt/${QTVERSION}/gcc/bin/macdeployqt" "$APPBUNDLE"
