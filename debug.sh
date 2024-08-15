#!/bin/sh

cmake --build build/debug

if [ $? -ne 0 ]; then
	exit
fi

pushd build/debug >/dev/null
./game_of_life
popd >/dev/null

