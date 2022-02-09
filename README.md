# PovHack

Convert a NetHack game into a high quality 3D Photorealistic animation with POVRAY

In order to parse the output of Nethack, the `vt_tiledata` option needs to be set.
As of NetHack 3.6.1, this is an optional feature that needs to be enabled at compile time (check whether your distribution has it!)
and as a program option. More information on this below.

Besides that, no special applications are needed to record a game. Just play the game on a VT100-like terminal (most Unix/Linux/MacOs terminals around are) while redirecting its output to a file wit:

```nethack | tee game.vt100```

# Pipeline

There are three applications involved:
1. `hack2frames` takes the game dump and produces a series of `.tty` files which are a bunch of plain ASCII files with the screen characters and some of their VT100 attributes (16 color palette index and bold).
2. `frame2pov` takes a single `tty` file and produces a PovRay `.pov` source file
3. The `povray` Raytracer needs to be installed in the system. With the produced PovRay sources, the main configuration file `tileset.pov`, and a chosen font (specified in `tileset.pov`), `povray` will produce a single `png` frame.
4. Use some video converter like `ffmpeg` to convert the `png` frames into a video. Suggested framerate is 3-5.

The `demo.sh` file located here automates the above steps for you for a sample game shipped with this distribution under `data`.

# Enabling tiledata

NetHack produces its output using VT100/ANSI escape codes. These are ASCII codes of the form <ESC>[<param1>;<param2>...<letter>
The vt_tiledata allows NetHack to output especial escape codes that allow for an easy interpretation of its output by programs such as this.
In order to enable this feature, two things need to be done:

1. enable them at compile time
1. enable them at run time (game options)

The first step is done by modifying the 'hints' file prior to compilation.
Each system has such a file. 
In the case of Unix/Linux, it is located in "sys/unix/hints/linux" in the source tree.
The following lines need to be UNCOMMENTED:

```
...

FLAGS+=-DTTY_TILES_ESCCODES

...

# if TTY_TILES_ESCCODES
WINSRC += tile.c
WINOBJ += tile.o
```

Then, as usual, you need to go to `sys/unix`, run `sh setup.sh hints/linux`, go back to the root, and run `make all && make install` 
in order to have the game running with this feature (you might want to change the `PREFIX` parameter in the hints file to select where the
game is to be installed)






