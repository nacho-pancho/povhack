#include "colors.inc"

//
// NetHack uses (usually) 16 color codes
// the colors below are the standard VT100/ANSI assignments
// to these codes
// you may change them to whatever you like
//
#declare ANSIColors = array[16]
#declare ANSIColors[0] =  0.66*White;  // actually used as 'normal'
#declare ANSIColors[1] =  0.5*Red; 
#declare ANSIColors[2] =  0.5*Green;
#declare ANSIColors[3] =  Orange; 
#declare ANSIColors[4] =  0.5*Blue;
#declare ANSIColors[5] =  0.5*Magenta; // could be Purple
#declare ANSIColors[6] = 0.5*Cyan;
#declare ANSIColors[7] = 0.66*White; // 'normal' color in Nethack

#declare ANSIColors[8] =  0.66*White; // seldom used
#declare ANSIColors[9] =   Red;
#declare ANSIColors[10] = Green;
#declare ANSIColors[11] = Yellow; 
#declare ANSIColors[12] = Blue ;
#declare ANSIColors[13] = Magenta ;
#declare ANSIColors[14] = Cyan ;
#declare ANSIColors[15] = White;

#declare Brown1 = color <.25,.20,0.15>;
#declare Brown2 = color <.3,.275,.25>;
#declare Brown3 = color <.4,.35,.3>;
#declare Brown4 = color <.5,.45,.4>;
#declare Brown5 = color <.8,.75,.7>;
