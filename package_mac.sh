#!/bin/bash

TRUE=0
FALSE=1

echo "Remember to create a _fresh_ release build in Qt Creator first!"
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

if [ -e "${APPBUNDLE}/Contents/Frameworks/QtCore.framework" ];then
	echo ">>>>>>> Looks like macdeployqt has already been run; skipping."
else
	echo ">>>>>>> Running macdeployqt"
	"${HOME}/QtSDK/Desktop/Qt/${QTVERSION}/gcc/bin/macdeployqt" "$APPBUNDLE"
	
	echo ">>>>>>> Removing unused frameworks from app bundle"
	for framework in "$APPBUNDLE"/Contents/Frameworks/*.framework; do
		bn=$(basename "${framework}")
		bn=${bn%.*} # remove extension
		frameworkstokeep[1]="QtCore"
		frameworkstokeep[2]="QtGui"
		frameworkstokeep[3]="QtNetwork"
		frameworkstokeep[4]="QtWebKit"
		remove=${TRUE}
		for index in 1 2 3 4; do
			[[ "${bn}" == "${frameworkstokeep[index]}" ]] && remove=${FALSE}
		done
		if [[ $remove -eq ${TRUE} ]];then
			rm -rf "${framework}"
		fi
	done
fi

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
ZIPFILE="QarkDown-OSX-${VERSION}.zip"
if [ -e "$ZIPFILE" ];then
	echo ">>>>>>> Deleting existing zip file: $ZIPFILE"
	rm -f "$ZIPFILE"
fi

echo ">>>>>>> Zipping app to: $ZIPFILE"
zip -r "${ZIPFILE}" "$TEMP_DIR"

echo ">>>>>>> Removing the temp installation directory"
rm -rf "$TEMP_DIR"

