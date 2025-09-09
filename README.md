As seen in 𝕽𝖎𝖛𝖊𝖓𝖉𝖊𝖑𝖑, the winning submission to Hyprland's 4th ricing competition: https://codeberg.org/zacoons/rivendell-hyprdots

Forked from https://codeberg.org/zacoons/imgborders
All i did was adding a nix flake and change some lines of ImgBorders.cpp to make it compile.

# Building

See https://wiki.hypr.land/Plugins/Using-Plugins/ to use it with Hyprland (In Manual section).

```sh
nix build .#imgborders
```

# Config'ing

```conf
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

# Contributing

```sh
git clone https://codeberg.org/zacoons/imgborders
```

## Debugging

See https://wiki.hypr.land/Plugins/Development/Getting-Started/

Make sure you have a copy of your Hyprland config file in `~/.config/hypr/hyprlandd.conf`.
But in this one you must remove all `exec-once` directives.
After doing that, add an exec-once directive which executes this command `hyprctl plugin load /home/you/code/imgborders/build/imgborders.so`.

Then to start debugging run the very simple `./debug.sh` script.

Good luck!
