#!/bin/sh

pushd build/debug >/dev/null
gdb ./game_of_life -q
popd >/dev/null
