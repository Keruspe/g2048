g2048 is yet another implementation of [the 2048 game](http://gabrielecirulli.github.io/2048/).

Written by [Marc-Antoine Perennou](http://www.imagination-land.org/)
Default theme by [Hugo Mougard](http://blog.crydee.eu/)
Doge theme taken from [doge2048](http://doge2048.com/)

g2048 will look for themes in two places: `~/.local/share/g2048/themes/` and `/usr/share/g2048/themes/`
and use the first matching the requested theme, fallbacking to default.

Two themes are provided: `default` and `doge`.

You can make your own theme (and maybe submit it here) in a very simple way:

    create a folder inside one of the two main themes folder mentioned above (the name of the folder is the name of the
theme)
    in this folder, create as many images as you can, respecting the correct naming convention (there should be at least
twelve of them).

All images should be 100x100 sized, and their name should be the value of the corresponding tile, with no extension. All
basic image formats are supported (png, jpg, animated gif...).

By default, the needed tiles are 0 (empty), 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 and 2048.

g2048 accepts 3 optional arguments, `--theme name` (defaulting to "default"), `--size s` (defaulting to 4) and
`--target t` (defaulting to 2048).

Provided themes are

    default
    doge

Steps to install it after cloning (skip the `./autogen.sh` part if you're building it from a tarball):

    ./autogen.sh
    ./configure
    make
    sudo make install
