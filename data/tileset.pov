#include "colors.inc"
#include "textures.inc"
#include "stones.inc"

// default tiles are just the characters with the correponding color
// and inverse character
// the user may override any particular tile with any object
// (first 33 elements are not used)
#declare Tiles = array[127][16][2];

//#declare TileFont = "courier.ttf";
#declare TileFont = "fonts/FreeMono.ttf";
#declare TileThickness = 0.1;
#declare TileShift     = 0.0;

#declare ANSIColors = array[16]
#declare ANSIColors[0] = pigment { color 0.66*White } // actually used as 'normal'
#declare ANSIColors[1] = pigment { color 0.5*Red }
#declare ANSIColors[2] = pigment { color 0.5*Green }
#declare ANSIColors[3] = pigment { color Orange }
#declare ANSIColors[4] = pigment { color 0.5*Blue }
#declare ANSIColors[5] = pigment { color 0.5*Magenta } // could be Purple
#declare ANSIColors[6] = pigment { color 0.5*Cyan }
#declare ANSIColors[7] = pigment { color 0.66*White } // 'normal' color in Nethack

#declare ANSIColors[8] = pigment { color 0.33*Black } // seldom used
#declare ANSIColors[9] = pigment { color Red }
#declare ANSIColors[10] = pigment { color Green }
#declare ANSIColors[11] = pigment { color Yellow }
#declare ANSIColors[12] = pigment { color Blue }
#declare ANSIColors[13] = pigment { color Magenta }
#declare ANSIColors[14] = pigment { color Cyan }
#declare ANSIColors[15] = pigment { color White }

#declare RoomHeight = 1.0;
#declare LocalLightColor = color <1.0,0.75,0.5>;
#declare GlobalLightColor = White;

#declare Floor = box { 
  <-0.51,-0.01,-0.51>,<0.51,0.0,0.51>
  pigment { color 0.5*White }  // floor
  //texture {T_Stone8 }
  normal { bumps 0.05 scale 0.1}
}

#declare Wall = box {
  <-0.51,0.0,-0.51>,<0.51,RoomHeight,0.51>
  pigment { color <0.50,0.475,0.45>  }
  normal { bumps 0.5 scale 0.2 rotate 45*(x+z)}
  //texture {T_Stone7 }
}

#declare Ceiling = sphere {<0,0,0>,0 } 
  // box { <-0.51,RoomHeight,-0.51>,<-0.51,RoomHeight+0.01,0.51>
//  pigment { color 0.2*White }  // floor
//}

// 
// torch carried by our hero
//
#declare LocalLight =  light_source { 
    <0,RoomHeight-0.1,0>, 0.7*LocalLightColor
    spotlight
    radius 40
    falloff 70 
    point_at <0,0,0>
}

#declare GlobalLight =  light_source { 
    <0,RoomHeight-0.1,0>, 0.15*GlobalLightColor
    area_light 0.2*x,0.2*z,5,5
}

#for (code,32,126) //127 not inc
  #for (col,0,15) 
    #declare BaseTile = text { 
      ttf TileFont chr(code) TileThickness, TileShift 
      pigment { ANSIColors[col] } 
      translate 0.25*y-0.25*x-0.5*TileThickness*z
    }
    #declare Tiles[code][col][0] = union {
      object { Floor } 
      object { BaseTile }
    }
    // inverse color
    // we put a bright floor tile underneath
    #declare Tiles[code][col][1] = union {
      object { Floor }
      object { BaseTile }
    }
   #end
#end

//
// default overrides:
// 32  spc void (nothing; put a black box here)
// 35  # hallway
// 46  . gray   (dark floor)
// 46  . bright (lit floor)
// 124 | gray  v. dark wall
// 124 | white v. lit wall
// 124 | orange open h. door
// 45  - gray  h. wall dark
// 45  - white  h. wall lit
// 45  - orange open v. door
// 43  + closed door
// 62  > down
// 60  < up

//
// void / unknown
//
#declare Tiles[32][0][0] = box { 
  Wall
  finish { ambient 0 }
}

#declare Tiles[32][0][1] = box { 
  Wall
  finish { ambient 0 }
}
//
// avatar
//
#declare Avatar = text { 
  ttf TileFont "@" TileThickness, TileShift 
  pigment { ANSIColors[15] } 
  translate 0.25*y-0.25*x-0.5*TileThickness*z
}
#declare Tiles[64][7][0] = union { object { Floor } object {Avatar rotate Angle*y } }
#declare Tiles[64][0][1] = union { object { Floor } object {Avatar rotate Angle*y} }
#declare Tiles[64][7][1] = union { object { Floor } object {Avatar rotate Angle*y} }
//
// hallway floor
//
#declare Tiles[35][0][0] = Floor
#declare Tiles[35][0][1] = Floor
#declare Tiles[35][7][0] = Floor
#declare Tiles[35][7][1] = Floor
//#declare Tiles[35][7][0] = union { object { Floor } object {LocalLight} }
//#declare Tiles[35][7][1] = union { object { Floor } object {LocalLight} }
//
// room floor
//
#declare Tiles[46][0][0] = Floor

#declare Tiles[46][0][1] = union {
  object { Floor }
//  object { LocalLight }
}

//
// vertical walls
//
#declare Tiles[124][0][0] = box { 
  Wall
}

#declare Tiles[124][0][1] = box { 
  Wall
}

#declare Tiles[124][3][0] = union {
  object { Floor } 
  box { 
    <-0.5,0.0,-0.5>,<-0.4,RoomHeight,0.5>
    pigment { color Orange }  // this is an open Door, put a wood texture here
  }
}
#declare Tiles[124][3][1] = union {
  object { Floor } 
  box { 
    <-0.5,0.0,-0.5>,<-0.4,RoomHeight,0.5>
    pigment { color Orange }  // this is an open Door, put a wood texture here
  }
}
///
// horizontal walls
//
#declare Tiles[45][0][0] = box { 
  Wall
}

#declare Tiles[45][0][1] = box { 
  Wall
}

#declare Tiles[45][3][0] = union { 
  object { Floor } 
  box { 
    <-0.51,0.0,-0.51>,<0.51,RoomHeight,-0.41>
    pigment { color Orange }  // this is an open Door, put a wood texture here
  }
}
#declare Tiles[45][3][1] = union { 
  object { Floor } 
  box { 
    <-0.51,0.0,-0.51>,<0.51,RoomHeight,-0.41>
    pigment { color Orange }  // this is an open Door, put a wood texture here
  }
}
///
// closed door (+)
//
#declare Tiles[43][3][0] = box { // closed door
  <-0.5,0.0,-0.5>,<0.5,RoomHeight,0.5>
  pigment { color Orange }  // this is a Door, put a wood texture here
}
#declare Tiles[43][3][1] = box { // closed door
  <-0.5,0.0,-0.5>,<0.5,RoomHeight,0.5>
  pigment { color Orange }  // this is a Door, put a wood texture here
}

#declare Ladder = union {
  cylinder {<-0.2,-1.2*RoomHeight,-0.2>,<-0.2,1.2*RoomHeight,-0.2>,0.05}
  cylinder {< 0.2,-1.2*RoomHeight, 0.2>,< 0.2,1.2*RoomHeight, 0.2>,0.05}
  #for (step,-5,5)
    #declare Y = RoomHeight*(step/5)+0.1 ;
    cylinder {< -0.19, Y, -0.19>,< 0.19, Y, 0.19>,0.03 }
  #end  
  pigment { color White }
}

#declare Tiles[60][0][0] = union { // up
  object { Floor }
  object { Ladder }
}
#declare Tiles[60][0][1] = union { // up
  object { Floor }
  object { Ladder }
}

#declare Tiles[62][0][0] = union { // down
  object { Ceiling }
  object { Ladder }
}
#declare Tiles[62][0][1] = union { // down
  object { Ceiling }
  object { Ladder }
}
  

global_settings { ambient_light 0.1*White }
