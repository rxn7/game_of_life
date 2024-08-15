#!/bin/sh

cmake --build build/debug

if [ $? -ne 0 ]; then
	exit
fi

pushd build/debug/bin >/dev/null
./game_of_life
popd >/dev/null

