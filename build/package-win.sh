#!/usr/bin/env bash

FILEPATH="jaeger-toolset-1.0.0-win"
SZPATH="/c/Program Files/7-Zip/extra"

echo "Prepare package files..."

echo "- package binaries..."
mkdir ${FILEPATH}
cp templates/win/config.txt ${FILEPATH}/config.txt
cp ../Installer/Installer/bin/Release/Installer.exe ${FILEPATH}/setup.exe
mkdir ${FILEPATH}/installation-dir
cp ../Jaeger/bin/Win/Release/I4Jaeger.dll ${FILEPATH}/installation-dir/
cp ../tool/bin/Win/Release/jaeger.exe ${FILEPATH}/installation-dir/
cp templates/win/shebang.exe ${FILEPATH}/installation-dir/
mkdir ${FILEPATH}/installation-dir/std
cp ../tests/std.jg ${FILEPATH}/installation-dir/std/

echo "Make package archive..."
mkdir output
cd ${FILEPATH}
7z a installer.7z ./*
cat "${SZPATH}/7zS.sfx" config.txt installer.7z > ../output/${FILEPATH}.exe
cd ..

echo "Cleanup..."
rm -rf ${FILEPATH}

echo "Done!"
