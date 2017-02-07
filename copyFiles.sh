#!/bin/bash
# about the escaping the space http://unix.stackexchange.com/questions/108635/why-i-cant-escape-spaces-on-a-bash-script

FOLDER=C:/Users/Minh/Documents/Visual\ Studio\ 2015/Projects/dxProject/dxProject

if [ "$1" == "--help" ]; then
	echo "options:"
	echo "--vstogit: copy from VS project folder to git folder"
	echo "--gittovs: copy from git folder to VS project folder"
	exit 0
elif [ "$1" == "--gittovs" ]; then
	cp ./*.h "${FOLDER}"
	cp ./*.cpp "${FOLDER}"
	cp ./*.hlsl "${FOLDER}"
elif [ "$1" == "--vstogit" ]; then
	cp "${FOLDER}"/*.h .
	cp "${FOLDER}"/*.cpp .
	cp "${FOLDER}"/*.hlsl .
else
	echo "Invalid argument was given"
	exit 0
fi
