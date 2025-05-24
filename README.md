# Installation

Build from source:

```
% git clone https://codeberg.org/zacoons/imgborders
% cmake -B build
% cmake --build build
```

Then add it to Hyprland:

```
% hyprctl plugin load /home/you/code/imgborders/build/imgborders.so
```

To remove it just replace `load` with `unload` in the above.

# Config'ing

```
plugin:imgborders {
    enabled = true
    image = ~/path/to/file
    sizes = 1,2,3,4 # left, right, top, bottom
    scale = 1.2
    smooth = true
}
```

I don't think I need to explain `enabled` or `image`.

`sizes` - (integer) Defines the number of pixels from each edge of the image to take.

`scale` - Scale the borders by some amount.

`smooth` - Whether the image pixels should have smoothing (true) or if it should be pixelated (false).

## Window rules

`plugin:imgborders:noimgborders` - Disables image borders.
