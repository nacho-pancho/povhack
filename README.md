# PovHack

Convert a NetHack game into a high quality 3D Photorealistic animation with POVRAY

No special applications are needed to record a game. Just play the game on a VT100-like terminal (most Unix/Linux/MacOs terminals around are) while redirecting its output to a file wit:

```nethack | tee game.vt100```

# Pipeline

There are three applications involved:
1. `hack2frames` takes the game dump and produces a series of `.tty` files which are a bunch of plain ASCII files with the screen characters and some of their VT100 attributes (16 color palette index and bold).
2. `frame2pov` takes a single `tty` file and produces a PovRay `.pov` source file
3. The `povray` Raytracer needs to be installed in the system. With the produced PovRay sources, the main configuration file `tileset.pov`, and a chosen font (specified in `tileset.pov`), `povray` will produce a single `png` frame.
4. Use some video converter like `ffmpeg` to convert the `png` frames into a video. Suggested framerate is 3-5.

The `demo.sh` file located here automates the above steps for you for a sample game shipped with this distribution under `data`.



