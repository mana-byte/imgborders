#!/bin/bash
export HYPRLAND_NO_CRASHREPORTER=1
cmake -B build && cmake --build build && hyprland --config ~/.config/hypr/hyprlandd.conf
