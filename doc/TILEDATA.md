
3.4.3-NAO uses the following codes:

* ESC [ 0 ; n z: Start a glyph (aka a tile) number n.
* ESC [ 1 z: End a glyph.
* ESC [ 2 ; n z: Select a window n to output to.
* ESC [ 3 z: End of data. NetHack has finished sending data to you, and is waiting for your input.

3.6.1 uses the following codes: [1]

* ESC [ 1 ; 0 ; n ; m z: Start a glyph (aka a tile) number n, with flags m
* ESC [ 1 ; 1 z: End a glyph.
* ESC [ 1 ; 2 ; n z: Select a window n to output to.
* ESC [ 1 ; 3 z: End of data. NetHack has finished sending data, and is waiting for input.

Special flags of a glyph can be: [2]

    0x0001: corpse
    0x0002: invisible
    0x0004: detected
    0x0008: pet
    0x0010: ridden
    0x0020: statue
    0x0040: object pile
    0x0080: lava hilight

The following information pertains to an upcoming version (Nethack 3.7.0). If this version is now released, please verify that it is still accurate, then update the page to incorporate this information.


    0x0100: ice hilight
    0x0200: out-of-sight lit areas when dark_room is disabled
    0x0400: unexplored
    0x0800: female
    0x1000: bad coordinates


