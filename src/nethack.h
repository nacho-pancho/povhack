#ifndef NETHACK_H
#define NETHACK_H

//
// NetHack TileData command info
//
#define NH_CMD_START_GLYPH    0
#define NH_CMD_END_GLYPH      1
#define NH_CMD_SELECT_WINDOW  2
#define NH_CMD_END_FRAME      3

#define is_start_glyph(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_START_GLYPH))
#define is_end_glyph(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_END_GLYPH))
#define is_end_frame(v) (((v)->cmd == VT_CMD_TILEDATA) && ((v)->par[1] == NH_CMD_END_FRAME))

#define FLAG_CORPSE        0x0001
#define FLAG_INVISIBLE     0x0002
#define FLAG_DETECTED      0x0004
#define FLAG_PET           0x0008
#define FLAG_RIDDEN        0x0010
#define FLAG_STATUE        0x0020
#define FLAG_PILE          0x0040
#define FLAG_LAVA_HILIGHT  0x0080
#define FLAG_ICE_HILIGHT   0x0100
#define FLAG_OUT_OF_SIGHT  0x0200
#define FLAG_UNEXPLORED    0x0400
#define FLAG_FEMALE        0x0800
#define FLAG_BADCOORDS     0x1000

#endif
