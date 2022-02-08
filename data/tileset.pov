#include "colors.inc"
#include "textures.inc"
#include "stones.inc"

//==================================================================
// easily customizable stuff
// "easy" if you know PovRay :)
//==================================================================

#declare RoomHeight = 1.0;

#declare TileFont = "fonts/FreeMono.ttf";
#declare TileThickness = 0.1;
#declare TileShift     = 0.0;

#declare LocalLightColor = color <1.0,0.75,0.5>;
#declare GlobalLightColor = White;

#declare Brown1 = color <.2,.15,.1>;
#declare Brown2 = color <.3,.25,.2>;
#declare Brown3 = color <.4,.35,.3>;
#declare Brown4 = color <.5,.475,.45>;

#declare TileTexture = texture {
  pigment { color White }
  finish { 
    phong 1 phong_size 10 
    diffuse  0.9
  }
}

#declare DoorTexture = texture {
      pigment {
         bozo
         color_map {
            [0.0 0.4 color Brown1 color Brown1 ]
            [0.4 1.01 color Brown3 color Brown3 ]
         }
         scale <4, 0.05, 0.05>
      }
      rotate y*45
      scale 0.1
   } texture {
      finish {
         phong 1
         phong_size 100
         brilliance 3
         ambient 0.2
         diffuse 0.8
      }
      pigment {
         wood
         turbulence 0.025

         color_map {
            [0.0 0.15 color Brown2 color Brown4 ]
            [0.15 0.40 color Brown4 color Clear ]
            [0.40 0.80 color Clear  color Brown4 ]
            [0.80 1.01 color Brown4 color Brown2 ]
         }

         scale <3.5, 1, 1>
         translate -50*y
         rotate 1.5*z
      }
      rotate y*45
      scale 0.1
   }

#declare WallTexture = texture {
  pigment {
    agate color_map{[0.0 0.3*White][1.0 0.5*White]}
  }
  normal { agate 0.5 }
  scale 0.1
}
  
#declare FloorTexture = texture {
  pigment {
    crackle color_map{[0.0 0.2*White][1.0 0.5*White]}
  }
  normal { crackle 1.6 }
  scale <0.1,0.05,0.1>
}

#declare GroundColor1 = color <0.4,0.3,0.2>;
#declare GroundColor2 = color <0.8,0.7,0.6>;

#declare HallwayTexture = texture {
  pigment {
    granite color_map{ [0.0 GroundColor1][1.0 GroundColor2] }
  }
  normal { gradient 1.6 }
  scale <0.05,0.10,0.05>  
}

#declare Wall = box { 
  <-0.51,0.0,-0.51>,<0.51,RoomHeight,0.51>
  texture { WallTexture }
}

#declare Floor = box {
  <-0.51,-0.01,-0.51>,<0.51,0.0,0.51>
  texture { FloorTexture }
}

#declare Hallway = box {
  <-0.51,-0.01,-0.51>,<0.51,0.0,0.51>
  texture { HallwayTexture }
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

//
// NetHack uses (usually) 16 color codes
// the colors below are the standard VT100/ANSI assignments
// to these codes
// you may change them to whatever you like
//
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

//
//==================================================================
// automation stuff
// touch if you dare
//==================================================================
//
// default tiles are just the characters with the correponding color
// and inverse character
// the user may override any particular tile with any object
// (first 33 elements are not used)
//
#declare Tiles = array[127][16][2];


#for (code,32,126) //127 not inc
  #for (col,0,15) 
    #declare BaseTile = text { 
      ttf TileFont chr(code) TileThickness, TileShift 
      texture { TileTexture pigment { ANSIColors[col] } }
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
  texture { TileTexture pigment { ANSIColors[15] } }
  translate 0.25*y-0.25*x-0.5*TileThickness*z
}
#declare Tiles[64][7][0] = union { object { Floor } object {Avatar rotate Angle*y } }
#declare Tiles[64][0][1] = union { object { Floor } object {Avatar rotate Angle*y} }
#declare Tiles[64][7][1] = union { object { Floor } object {Avatar rotate Angle*y} }
//
// hallway floor
//
#declare Tiles[35][0][0] = Hallway
#declare Tiles[35][0][1] = Tiles[35][0][0]
#declare Tiles[35][7][0] = Tiles[35][0][0]
#declare Tiles[35][7][1] = Tiles[35][0][0]
//
// room floor
//
#declare Tiles[46][0][0] = Floor

#declare Tiles[46][0][1] = Floor

//
// vertical walls
//
#declare Tiles[124][0][0] = box { 
  Wall
}

#declare Tiles[124][0][1] = box { 
  Wall
}

// open horizontal door
#declare Tiles[124][3][0] = union {
  object { Floor } 
  box { 
    <-0.5,0.0,-0.5>,<-0.4,RoomHeight,0.5>
  }
    texture { DoorTexture }
}

#declare Tiles[124][3][1] = Tiles[124][3][0]

///
// horizontal walls
//
#declare Tiles[45][0][0] = box { 
  Wall
}

#declare Tiles[45][0][1] = box { 
  Wall
}

// open vertical door
#declare Tiles[45][3][0] = union { 
  object { Floor } 
  box { 
    <-0.51,0.0,-0.51>,<0.51,RoomHeight,-0.41>
    texture { DoorTexture }
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
  texture { DoorTexture }
}
#declare Tiles[43][3][1] = Tiles[43][3][0]


#declare Ladder = union {
  cylinder {<-0.2,-1.2*RoomHeight,-0.2>,<-0.2,1.2*RoomHeight,-0.2>,0.05}
  cylinder {< 0.2,-1.2*RoomHeight, 0.2>,< 0.2,1.2*RoomHeight, 0.2>,0.05}
  #for (step,-5,5)
    #declare Y = RoomHeight*(step/5)+0.1 ;
    cylinder {< -0.19, Y, -0.19>,< 0.19, Y, 0.19>,0.03 }
  #end  
  texture { DoorTexture }
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
  object { Ladder }
}
#declare Tiles[62][0][1] = union { // down
  object { Ladder }
}
  

global_settings { ambient_light 0.1*White }
