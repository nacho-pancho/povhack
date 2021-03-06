#ifndef NETHACK_H
#define NETHACK_H


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

#define NWINDOWS      10
#define WIN_BASE      0 // special 'base' window, for 'absolute screen' positioning
#define WIN_MESSAGE   1 // most of the messages 
#define WIN_STATUS    2 // status
#define WIN_MAP       3 // these don't get absolute positioning, apparently
#define WIN_INVENTORY 4 // inventory


#endif
