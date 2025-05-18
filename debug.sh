#!/bin/bash

cmake -B build && cmake --build build

if [[ $? -ne 0 ]]; then
	echo failed to compile
	exit 1
fi

HYPRLAND_NO_CRASHREPORTER=1 hyprland --config ~/.config/hypr/hyprlandd.conf > log.txt
