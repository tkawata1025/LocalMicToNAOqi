#!/bin/bash

if [ -f ".naoqisdklib" ]; then
	echo
else
	touch .naoqisdklib
fi

NAOQISDKLIB=`cat .naoqisdklib`

if [ -f "$NAOQISDKLIB/toolchain.xml" ]; then
	echo "NAOQI C++ SDK DIR:" $NAOQISDKLIB
else
	echo "ENTER NAOQI C++ SDK DIR:"
	read INPUT

	NAOQISDKLIB=$INPUT
	echo $INPUT > .naoqisdklib
fi


if [ -f "$NAOQISDKLIB/toolchain.xml" ]; then
	echo
else
	echo "NAOQI C++ SDK DIR:" $NAOQISDKLIB " doesn't exist"
	exit 1
fi

if [ -d "LocalMicToNAO.app/Contents/Frameworks/QtCore.framework" ]; then
	echo "QtCore.framework exist.."
else
	echo "Embedding qt frameworks.."	
	macdeployqt LocalMicToNAO.app
fi

if [ -d "LocalMicToNAO.app/Contents/Library" ]; then
	echo "NAOqi lib directory exists.."
else
	echo "Creating NAOqi lib directory.."
	mkdir -p LocalMicToNAO.app/Contents/Library/lib
fi

cp NAOqi/build/lib/* LocalMicToNAO.app/Contents/Library/lib
find $NAOQISDKLIB/lib -name *.dylib | xargs -I {} cp {} LocalMicToNAO.app/Contents/Library/lib/

cd LocalMicToNAO.app/Contents/Library/lib
for dylibfile in `ls *.dylib`;
do 
	ls libboost_*.dylib | grep -v ${dylibfile} | xargs -I {} install_name_tool -change {} @rpath/lib/{} ${dylibfile}
done

