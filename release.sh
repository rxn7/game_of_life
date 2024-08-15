#!/bin/sh

cmake --build build/release

if [ $? -ne 0 ]; then
	exit
fi

pushd build/release >/dev/null
./game_of_life
popd >/dev/null

