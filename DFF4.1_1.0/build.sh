#!/bin/sh

[ -z "$ARDUINO_HOME" ] && echo "Need to set ARDUINO_HOME environment variable first!" && exit 1;

ARDUINO=$ARDUINO_HOME/arduino
SKETCH_NAME=`find *.ino`
SKETCH="$PWD/$SKETCH_NAME"
BUILD_PATH="$PWD/build"


buildSketch() {
	BOARD=$1
	$ARDUINO --verbose --verify --board $BOARD --preserve-temp-files --pref build.path="$BUILD_PATH" $SKETCH
}

buildSketch "arduino:samd:arduino_zero_native"
