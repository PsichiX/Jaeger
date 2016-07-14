#!/usr/bin/env bash

FILEPATH="jaeger-toolset-1.0.0-linux"

echo "Prepare package files..."

echo "- package binaries..."
mkdir ${FILEPATH}
cp templates/linux/INSTALL ${FILEPATH}/
mkdir ${FILEPATH}/bin
cp ../Jaeger/bin/Linux/Release/I4Jaeger.so ${FILEPATH}/bin/
cp ../tool/bin/Linux/Release/jaeger ${FILEPATH}/bin/
mkdir ${FILEPATH}/std
cp ../tests/std.jg ${FILEPATH}/std/

echo "Make package archive..."
mkdir output
tar -cvzf output/${FILEPATH}.tar.gz ${FILEPATH}/

echo "Cleanup..."
rm -rf ${FILEPATH}

echo "Done!"
