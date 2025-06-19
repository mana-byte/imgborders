As seen in 𝕽𝖎𝖛𝖊𝖓𝖉𝖊𝖑𝖑, the winning submission to Hyprland's 4th ricing competition: https://codeberg.org/zacoons/rivendell-hyprdots

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
    insets = 1,2,3,4 # left, right, top, bottom
    scale = 1.2
    smooth = true
    blur = false
}
```

I don't think I need to explain `enabled` or `image`.

`sizes` - (4 integers) Defines the number of pixels from each edge of the image to take.

`insets` - (4 integers) Defines the amount by which to inset each side into the window.

`scale` - Scale the borders by some amount.

`smooth` - Whether the image pixels should have smoothing (true) or if it should be pixelated (false).

`blur` - Whether transparency should have blur (true) or if it should be clear (false).

## Window rules

`plugin:imgborders:noimgborders` - Disables image borders.
