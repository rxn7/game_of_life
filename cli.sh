#!/usr/bin/env bash

print_arg_desc() {
	echo -e "\e[1;34m$1\e[0;0m: $2"
}

show_help() {
	echo -e "\e[1;35mrxn's Game of Life CLI\n"
	print_arg_desc "-help" "Show possible arguments"
	print_arg_desc "-linux" "Build the game for linux"
	print_arg_desc "-run" "Build the game for linux and run it natively"
	print_arg_desc "-clean" "Clean the obj and bin directories"
}

for arg in "$@"; do
	if [ "$arg" = "-help" ]; then
		show_help
		exit

	elif [ "$arg" = "-linux" ] || [ "$arg" = "-lin" ]; then
		make -f linux.mk
		exit

	elif [ "$arg" = "-run" ]; then
		echo -e "\e[1;32mBuilding the game for linux and running it natively...\e[0;0m"
		make -f linux.mk
		pushd bin/linux ; ./game_of_life ; popd
		exit

	elif [ "$arg" = "-clean" ]; then
		rm -rf obj
		rm -rf bin
		exit

	else
		echo -e "\e[1;31mUnknown argument '$arg'\e[0;0m"

	fi
done

show_help
exit