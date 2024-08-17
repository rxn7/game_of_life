#!/bin/sh

pushd build/debug/bin >/dev/null
gdb ./game_of_life -q
popd >/dev/null
