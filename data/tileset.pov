#include "colors.inc"
#include "textures.inc"
#include "stones.inc"

//==================================================================
// easily customizable stuff
// "easy" if you know PovRay :)
//==================================================================

global_settings { 
  ambient_light 0.1*White 
  assumed_gamma 1.0
}


#declare RoomHeight = 1.0;

#declare TileFont = "fonts/FreeMono.ttf";
#declare TileThickness = 0.1;
#declare TileShift     = 0.0;
#declare TileSize      = 1.0;
//#declare CameraHeight  = 0.7; 
#declare CameraHeight  = 8.0; 
#declare CameraDistance = 1;
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
         translate -50*z
         rotate 1.5*y
      }
      rotate 45*z
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
    //crackle color_map{[0.0 0.2*White][1.0 0.5*White]}
    crackle color_map{[0.0 0.2*White][0.1 0.2*White][0.3 0.5*White][1.0 0.5*White]}
  }
  normal { crackle 1.6 }
  scale <0.1,0.05,0.1>
}

#declare GroundColor1 = color <0.4,0.3,0.2>;
#declare GroundColor2 = color <0.8,0.7,0.6>;

#declare HallwayTexture = FloorTexture
//texture {
//  pigment {
//    granite color_map{ [0.0 GroundColor1][1.0 GroundColor2] }
//  }
//  normal { gradient 1.6 }
//  scale <0.05,0.10,0.05>  
//}

#declare Wall = box { 
  <-0.51, -0.51, 0.0 >,< 0.51, 0.51, RoomHeight>
  texture { WallTexture }
}

#declare Floor =
  box {
    < -0.51, -0.51, -0.01 >,< 0.51, 0.51, 0.0 >
    texture { FloorTexture }
  }

#declare Hallway = box {
  <-0.51, -0.51, -0.01>,<0.51, 0.51, 0.0>
  texture { HallwayTexture }
}

#declare Ceiling =  box {
  <-0.51,-0.51, RoomHeight>,<0.51,0.51,RoomHeight+0.01>
  texture { WallTexture }
  no_image
  no_shadow
}
// 
// torch carried by our hero
//
#declare LocalLight =  light_source { 
    <0,0,RoomHeight-0.1>, LocalLightColor
    spotlight
    radius 40
    falloff 70 
    point_at <0,0,0>
}

#declare GlobalLight =  light_source { 
    <0, 0, RoomHeight-0.1>, 0.2*GlobalLightColor
    area_light 1*x, 1*y, 5, 5
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

#declare ANSIColors[8] = pigment { color 0.66*White } // seldom used
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
#declare Tiles = array[127][16];

#for (code,32,126) //127 not inc
  #for (col,0,15) 
    #declare BaseTile = text { 
      ttf TileFont chr(code) TileThickness, TileShift 
      texture { TileTexture pigment { ANSIColors[col] } }            
      translate <-0.25, 0.2, -0.5*TileThickness>
      scale TileSize
      rotate 90*x // from x,y plane to x,z plane
    }
    #declare Tiles[code][col] = BaseTile
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
#declare Tiles[32][0] = box { 
  Wall
  finish { ambient 0 }
}
#declare Tiles[32][7] = Tiles[32][0];
#declare Tiles[32][8] = Tiles[32][0];
//
// avatar
//
#declare Avatar = text { 
  ttf TileFont "@" TileThickness, TileShift 
  texture { TileTexture pigment { ANSIColors[15] } }
  translate <-0.25,0.25,-0.5*TileThickness>
  scale TileSize
  rotate 90*x // from x,y plane to x,z plane
}

//#declare Tiles[64][7] = Avatar
//#declare Tiles[64][8] = Avatar
//
// hallway floor
//
#declare Tiles[35][0] = Hallway
#declare Tiles[35][8] = Hallway
#declare Tiles[35][7] = Hallway
//
// room floor
//
#declare Tiles[46][0] = Floor
#declare Tiles[46][7] = Floor
#declare Tiles[46][8] = Floor

//
// vertical walls
//
#declare Tiles[124][0] = box { Wall }
#declare Tiles[124][7] = box { Wall }
#declare Tiles[124][8] = box { Wall }

// open horizontal door
#declare Tiles[124][3] = box { 
    <-0.5,-0.5,0.0>,<-0.4,0.5,RoomHeight> 
    texture { DoorTexture } }
#declare Tiles[124][11] = Tiles[124][3]

///
// horizontal walls
//
#declare Tiles[45][0] = box {  Wall }
#declare Tiles[45][7] = box {  Wall }
#declare Tiles[45][8] = box {  Wall }

// open vertical door
#declare Tiles[45][3] =  box { 
    <-0.51,0.0,-0.51>,<0.51,RoomHeight,-0.41>
    texture { DoorTexture }
  }
#declare Tiles[45][11] = Tiles[45][3]
///
// closed door (+)
//
#declare Tiles[43][3] = box { // closed door
  <-0.5,-0.5,0.0>,<0.5,0.5,RoomHeight>
  texture { DoorTexture }
}
#declare Tiles[43][11] = box { // closed door
  <-0.5,-0.5,0.0>,<0.5,0.5,RoomHeight>
  texture { DoorTexture }
}


#declare Ladder = union {
  cylinder {<-0.2,-0.2,-1.2*RoomHeight>,<-0.2,-0.2,1.2*RoomHeight>,0.05}
  cylinder {< 0.2, 0.2,-1.2*RoomHeight>,< 0.2, 0.2,1.2*RoomHeight>,0.05}
  #for (step,-5,5)
    #declare Y = RoomHeight*(step/5)+0.1 ;
    cylinder {< -0.19, -0.19, Y>,< 0.19, 0.19, Y>,0.03 }
  #end  
  texture { DoorTexture }
}

#declare Tiles[60][7] = Ladder

#declare Tiles[60][8] = Tiles[60][7]
#declare Tiles[60][0] = Tiles[60][7]

#declare Tiles[62][0] = Ladder
#declare Tiles[62][7] = Ladder
#declare Tiles[62][8] = Ladder
  

