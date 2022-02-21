#include "colors.inc"
#include "textures.inc"
#include "stones.inc"
#include "shapes.inc"
#include "candle.inc"

//==================================================================
// easily customizable stuff
// "easy" if you know PovRay :)
//==================================================================

global_settings { 
  ambient_light 0.2*White 
  assumed_gamma 1.0
}

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

#declare TileFont = "fonts/FreeMono.ttf";
#declare TileThickness = 0.1;
#declare TileShift     = 0.0;
#declare TileSize      = 0.5;

#declare RoomHeight = 1.0;
#declare LocalLightHeight  = 0.6*RoomHeight;
#declare GlobalLightHeight = 0.6*RoomHeight;

#declare DoorThickness = 0.03;
#declare DoorSize = 0.25;

//#declare CameraHeight  = 0.7; 
#declare CameraHeight  = 2.5; 
#declare LookAtHeight  = TileSize;
#declare CamDistX = -0.3; // = -1;
#declare CamDistY = 3; // =  5;

#declare LocalLightColor = color <1.0,0.65,0.2>;
#declare GlobalLightColor = White;

#declare Brown1 = color <.25,.20,0.15>;
#declare Brown2 = color <.3,.275,.25>;
#declare Brown3 = color <.4,.35,.3>;
#declare Brown4 = color <.5,.45,.4>;
#declare Brown5 = color <.8,.75,.7>;

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
         diffuse 0.8
      }
      pigment {
         wood
         turbulence 0.025

         color_map {
            [0.0  0.30 color Brown2 color Brown4 ]
            [0.30 0.40 color Brown4 color Brown5 ]
            [0.40 0.50 color Brown5 color Brown4 ]
            [0.85 1.01 color Brown4 color Brown2 ]
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
    agate color_map{[0.0 0.3*White][1.0 0.4*White]}
  }
  //normal { agate 0.8 }
  normal { granite 0.5 }
  scale 0.1
}
  
#declare FloorTexture = texture {
  pigment {
    //crackle color_map{[0.0 0.2*White][1.0 0.5*White]}
    crackle color_map{[0.0 0.7*Brown2][0.05 0.7*Brown2][0.05 0.3*White][1.0 0.3*White]}
  }
  normal { crackle 1.6 }
  scale <0.3,0.3,0.3>
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
  <-0.51, -0.1, 0.0 >,< 0.51, 0.1, RoomHeight>
  texture { WallTexture }
}

#declare Corner = union { 
  // default rotation is upper left corner (northwest)
  // remember: Z increases downwards
  box { < -0.1, -0.1, 0.00 >,< 0.51,  0.10, RoomHeight> }
  box { < -0.1, -0.1, 0.00 >,< 0.10,  0.51, RoomHeight> }
  texture { WallTexture }
}

#declare CrystalMat = material {
  texture { 
    finish { 
      specular 1
      roughness 0.001
      diffuse 0.3
      reflection 0.1
    } 
    pigment { color 0.7*White filter 0.8  }
  } 
  interior { ior 1.4 } 
}

#declare FountainDish =  union {
  cylinder  { <0,0,0>,<0,0.0,0.1>,0.28 texture {Water} }
  cylinder  { <0,0,0>,<0,0.0,0.08>,0.35 texture { White_Marble } }
}

#declare Fountain =  union {
  cylinder  { <0,0,0>,<0,0.0,0.9>,0.05 texture { White_Marble } }
  object { FountainDish }
  object { FountainDish scale <0.5,0.5,0.8> translate 0.6*z }
}

// debug: origin
// cylinder { -10*z+40*x+10*y,10*z+40*x+10*y,0.1 pigment { color Red }}

#declare Floor =
  box {
    < -0.51, -0.51, -0.01 >,< 0.51, 0.51, 0.0 >
    texture { FloorTexture }
  }

#declare Trap =
  box {
    < -0.7, -0.7, -0.21 >,< 0.7, 0.7, -0.2 >
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


#declare Door = union {
  box { <-DoorSize, -DoorThickness, 0.0 >,< DoorSize, DoorThickness, 0.6*RoomHeight>}
  cylinder { <0.0, -DoorThickness, 0.6*RoomHeight>,<0.0, DoorThickness, 0.6*RoomHeight>,DoorSize }
  texture { DoorTexture }
}

#declare DoorHole = union {
  box { <-DoorSize, -1, -0.05 >,< DoorSize, 1, 0.6*RoomHeight>}
  cylinder { <0.0, -1, 0.6*RoomHeight>,<0.0, 1, 0.6*RoomHeight>,DoorSize }
}

#declare BrokenDoor = difference {
  object { Wall }
  object { DoorHole }
  texture { WallTexture }
}

#declare ClosedDoor = union {
  object { BrokenDoor }
  object { Door texture { DoorTexture } }
}

#declare OpenDoor = union {
  object { BrokenDoor }
  object { Door texture { DoorTexture } rotate 90*z translate <-DoorSize,-DoorSize,0> }
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

#declare Boulder = superellipsoid {
  <0.25, 0.25>
  translate <0, 0, 1>
  scale 0.4
  texture {
    pigment { color <0.34,0.32,0.3> }
    normal { granite 1.0 }
    scale 0.2
  }
}

#declare Grave = union {
  box      { <-0.4, 0.0,-0.1>,< 0.4, 0.7, 0.1> }
  cylinder { < 0.0, 0.0,-0.1>,< 0.0, 0.0, 0.1>,0.4 scale <1,0.75,1> translate 0.7*y }
  texture { 
    pigment { color Gray }
    normal  { agate 0.15 scale 0.25 }
    finish  { diffuse 0.3 } 
  }
  rotate 90*x
  rotate -30*z
}

#declare Altar = union {
  box      { <-0.3, 0.0,-0.2>,< 0.3, 0.05, 0.2> }
  cylinder { <-0.15, 0.0, 0.0>,<-0.15, 0.3, 0.0>,0.07 }
  cylinder { < 0.15, 0.0, 0.0>,< 0.15, 0.3, 0.0>,0.07 }
  box      { <-0.4, 0.3,-0.3>,< 0.4, 0.4, 0.3> }
  texture { White_Marble scale 0.5 }
  rotate 90*x
}

#declare Cloth = union {
  box      {  <-0.4, 0.4,-0.2>, < 0.4, 0.41, 0.2> }
  cylinder {  <-0.4, 0.4,-0.2>, <-0.4, 0.4, 0.2>,0.01 }
  cylinder {  < 0.4, 0.4,-0.2>, < 0.4, 0.4, 0.2>,0.01 }
  box      {  <-0.41, 0.2,-0.2>, <-0.4, 0.4, 0.2> }
  box      {  < 0.4, 0.4,-0.2>, < 0.41, 0.2, 0.2> }
  rotate 90*x
}

#declare Candle = Candle_2 ( 1,   // Light on 
                  0,  // 1 = on ; 0 = off
                  1.4,  // Candle_Height, 
                  1.5,  // Candle_Intensity,  
                  0.6,  // Candle_Flame_Scale
                  4, //  3 ~ 5    
                  3     //   2,3,4
                );


#macro PutAltar(X,Y)
  object { 
    Altar 
    translate <X,Y,0>
  }
//  light_source { 
//    <0.0,0.0,1.0>,
//    White 
//    spotlight 
//    point_at <0,0,0>
//    radius 20
//    falloff 40
//    translate <X,Y,0>
//  }
  object { Cloth pigment { color Red } translate <X,Y,0> }
  object { Candle scale 0.1 rotate 90*x translate <X+0.25,Y,0.4> }
  object { Candle scale 0.1 rotate 90*x translate <X-0.25,Y,0.4> }
#end


#declare Rock = sphere { 
  <0.0,0.0,0.2>,0.2 
  scale <1,1,0.65> 
  pigment { color 0.5*Gray }
  normal { granite scale 0.1 }
}

#declare IronBall = sphere {
  <0,0,0.2>,0.2
  texture {
    pigment { color <0.1,0.13,0.14> }
    finish { 
      reflection 0.25
      diffuse 0.75 
      phong 1.0
      phong_size 150
      brilliance 7
      metallic
    } 
    normal { granite 0.1 }
    scale 0.1
  }
}

#declare BookHole = union {
  cylinder { <0,0,-0.2>,<0,0,1>,0.16 scale <0.25,1,1> }
  box      { <0,-0.16,-0.2>,<0.6,0.16,1> }
}

#declare BookCover0 = union {
  cylinder { <0,0,0>,<0,0,0.7>,0.2 scale <0.25,1,1> }
  box      { <0,-0.2,0>,<0.5,0.2,0.7> }
}

#declare BookCover = difference { object { BookCover0 } object { BookHole } }

#declare BookPages0 = union {
  cylinder { <0,0,0.05>,<0,0,0.65>,0.16 scale <0.25,1,1> }
  box      { <0,-0.16,0.05>,<0.49,0.16,0.65> }
}

#declare BookPages = difference {
  object { BookPages0 }
  cylinder { <0,0,0.0>,<0,0,0.7>,0.17 scale <0.25,1,1> translate 0.5*x }
}

#declare PagesTex = texture {
  pigment { color <0.8,0.7,0.5> } 
  normal  { marble scale <0.01,0.03,0.03> rotate 90*z }
}

#macro Book(COL) 
union {
  object { 
    BookCover 
    texture {
      pigment { color COL } 
      normal { agate 0.1 scale 0.1 } 
      finish { phong 0.5 phong_size 150 brilliance 2 }
    }
  }
  object { BookPages texture { PagesTex } } 
  scale 0.3
  rotate <20,0,15>
  translate 0.4*z
}
#end


#macro Amulet(COL) 
union {
  cylinder { 
    <0,0,0>,<0,-0.05,0.0>,0.4 
    scale <0.7,0.5,1> 
    translate 0.5*z texture { Silver_Texture } 
  }
  difference {
    sphere { <0,0,0>,0.325 
      scale <0.7,0.2,1> 
      translate 0.5*z 
    }
    plane  { y,0.001 }
    material { 
      CrystalMat 
      texture { pigment { color COL filter 0.8 } } 
    }
  }
  scale 0.3
  rotate 15*z+20*x   
  translate 0.3*z
}
#end

#declare Ring = torus { 0.4,0.05
  scale <0.5,1,0.5> translate 0.5*z texture { Gold_Texture } 
  rotate 20*x+15*z
  scale 0.3
  translate 0.3*z
}

#declare WandTexOld = texture {
  pigment { rgb .5 }
  finish
  { specular 2 roughness .02 metallic
    reflection { .2, .8 }
  }
}

#declare WandTex = texture {
  pigment { color <0.95,0.85,0.70> } 
  finish
  { 
    diffuse 0.75
    brilliance 2
    specular 1.0 roughness 0.001
//    phong 1.0 phong_size 100    
  }
}

#declare Wand0 = julia_fractal
{ <-.4, .2, .3, -.2>
  quaternion sqr
  max_iteration 8
  precision 50
}

#declare Wand = intersection {
  object { Wand0 scale <0.9,0.025,0.025>}
  sphere { <0.5,0,0>,0.5 }
  scale 0.7
  rotate -90*y
  rotate 20*x+20*z
  translate 0.2*z
  texture { WandTex }
}


#declare PotionFlask = lathe {
  bezier_spline
  8
  // bottom
  <0.2    ,0.0    >, // p0
  <0.2+0.2,0.0+0.2>, // c0
  <0.1,0.6-0.15>, // c1
  <0.1,0.6    >, // p1
  // top
  <0.1     ,0.6>, // p2
  <0.1     ,0.6+0.1>, // c2
  <0.1+0.1 ,0.8>, // c3
  <0.1     ,0.8> // p3
  rotate 90*x
}

#declare FlaskTex = material {
  texture { 
    finish { 
    specular 1
    roughness 0.001
    diffuse 0.3
    reflection 0.1
    } 
    pigment { color <0.8,0.9,0.95,0.8> } 
  }
    interior { ior 1.02 }
}

#declare PotionLiquid = difference {
  object { PotionFlask scale 0.9 }
  plane  { -z,-0.4 }
  translate 0.01*z
}

#declare LiquidTex = material {
  texture {
  pigment { color Red  } 
  finish { 
    specular 1
    roughness 0.001
    diffuse 0.3
    reflection 0.1
    } 
  }
  interior { ior 1.1 }
}

#declare PotionCork = cone {
  <0.0,0.75,0.0>,0.09,
  <0.0,0.85,0.0>,0.11
  rotate 90*x
}

#macro Potion(COL)
  union { 
  object {PotionFlask material { FlaskTex } } 
  object {PotionCork texture { Cork scale 0.5} } 
  object {
    PotionLiquid 
    material { 
      LiquidTex  
      texture { pigment { COL filter 0.8 } }
    }
  }
  scale 0.3
  translate 0.2*z
}
#end


#declare ChestTex0 = texture { 
  DMFDarkOak scale 0.1
  finish { ambient 0 diffuse 0.3 phong 0.3 phong_size 10 }
}
#declare ChestTex1 = texture { 
  Rusty_Iron
  finish { ambient 0 diffuse 0.3 }
}

#declare Chest0 = union {
  box { <-0.3,0.0,-0.25>,<0.3,0.25,0.25> }
  cylinder { <-0.3,0.0,0.0>,<0.3,0.0,0.0>,0.25 scale <1,0.5,1> translate 0.25*y }
}

#declare Chest10 = union {
  box { <-0.31,0.0,-0.26>,<-0.28,0.25,0.26> }
  cylinder { <-0.31,0,0.0>,<-0.28,0.0,0.0>,0.26 scale <1,0.5,1> translate 0.25*y }
}

#declare Chest11 = box {
  <-0.31,0.0,-0.26>,<0.32,0.03,-0.23>
}

#declare Chest12 = box {
  <-0.31,0.0,-0.26>,<-0.28,0.03, 0.26>
}

#declare Chest1 = union {
  object { Chest10 }
  object { Chest10  translate 0.2*x }
  object { Chest10  translate 0.4*x }
  object { Chest10  translate 0.6*x }
  object { Chest11  translate -0.01*x }
  object { Chest11  translate 0.5*z }
  object { Chest11  translate 0.25*y }
  object { Chest11  translate 0.5*z+0.25*y }
  object { Chest12  translate -0.01*x }
  object { Chest12  translate 0.61*x }
  object { Chest12  translate 0.61*x+0.25*y }
  object { Chest12  translate -0.01*x+0.25*y }
}

#declare Chest = union {
  object { Chest0 texture { ChestTex0 } scale <1.075,1,1> }
  object { Chest1 } texture { ChestTex1 }
  rotate 90*x+25*z 
}

#declare DN = 10.0;
#declare DA = 2.0*pi/DN;

#declare Diamond0 = prism {
	  linear_spline
	  conic_sweep 
	    0, 1, DN+1,
//#for (a,0,DN)	    
//	      <cos(a*DA),sin(a*DA)>,
//#end	      
	      <cos(0),sin(0)>,
	      <cos(DA),sin(DA)>,
	      <cos(2*DA),sin(2*DA)>,
	      <cos(3*DA),sin(3*DA)>,
	      <cos(4*DA),sin(4*DA)>,
	      <cos(5*DA),sin(5*DA)>,
	      <cos(6*DA),sin(6*DA)>,
	      <cos(7*DA),sin(7*DA)>,
	      <cos(8*DA),sin(8*DA)>,
	      <cos(9*DA),sin(9*DA)>,
	      <cos(10*DA),sin(10*DA)>
      }


#macro Gem(COL) 
union {
  object { Diamond0 translate -y  }
  difference { 
    object { 
      Diamond0 
      translate -y 
    } 
    plane { y,-0.3 } 
    rotate 180*x 
  }
  rotate 90*x+10*z
  material { 
    CrystalMat 
    texture { pigment { color COL filter 1.0 } }
  }
  scale <0.2,0.2,0.25>
  translate 0.5*z
}
#end
//
// avatar
//
#declare Avatar = text { 
  ttf TileFont "@" TileThickness, TileShift 
  texture { TileTexture pigment { color White } }
  translate <-0.25,0.25,-0.5*TileThickness>
  scale TileSize
  rotate 90*x // from x,y plane to x,z plane
}
#declare Shield00 = prism { 
  bezier_spline
  -0.3,0.3
  12,
  < -1, 0>,
  < -1, 0>,
  <  1, 0>,
  <  1, 0>,

  <  1, 0>,
  <  1,-1>,
  <  1,-2.5>,
  <  0,-3>,

  <  0,-3>,
  < -1,-2.5>,
  < -1,-1>,
  < -1, 0>
  translate 1.5*z
}
#declare Shield01 = difference {
	cylinder { <0,0,2>,<0,0,-2>,1.1 scale <1,0.2,1> translate 0.1*y }
	cylinder { <0,0,2>,<0,0,-2>,1.1 scale <1,0.2,1> translate -0.1*y }
}
#declare Shield10 = prism { 
  bezier_spline
  -0.5,0.5
  12,
  < -0.8, -0.2>,
  < -0.8, -0.2>,
  <  0.8, -0.2>,
  <  0.8, -0.2>,

  <  0.8, -0.2>,
  <  0.8, -1.5>,
  <  0.7, -2.5>,
  <  0.0, -2.8>,

  <  0.0, -2.8>,
  < -0.7, -2.5>,
  < -0.8, -1.5>,
  < -0.8, -0.2>
  translate 1.5*z
}

#declare Shield1 = intersection  { 
  object { Shield10 }
  object { Shield01 }
}

#declare Shield0 = 
  difference {
    intersection { 
      object { Shield00 } 
      object { Shield01 }  
    }
    object { Shield1 scale <1,2,1> }
}

#declare ShieldTex = texture {
  checker
  texture { pigment { color Blue } finish { phong 0.8 phong_size 100 brilliance 5 } }
  texture { pigment { color Yellow  }   finish { phong 0.8 phong_size 100 brilliance 5 } }
  scale 2
  translate 0.25*z
}

#declare Shield = union { 
  object { Shield0 texture { pigment { color Gray } finish { Metal } } }
  object { Shield1 texture { ShieldTex } }
}

#declare ScrollPaper = difference {
  intersection { 
    object { Hyperboloid_Y scale <0.25,1.0,0.25> }
    sphere { <0,0,0>,1 }
  }
  sphere { <0,0,0>,1 scale <1,0.3,1> translate 1.2*y }
  sphere { <0,0,0>,1 scale <1,0.3,1> translate -1.2*y }
  pigment { color <0.8,0.7,0.5> } 
}

#declare ScrollPole = union {
  cylinder { <0.0,-1.1,0.0>,<0.0, 1.1,0.0>,0.1 }
  sphere   { <0.0, 0.0,0.0>,0.2 scale <1,0.6,1> translate -1.1*y }
  sphere   { <0.0, 0.0,0.0>,0.2 scale <1,0.6,1> translate  1.1*y }
  texture { DMFDarkOak scale 0.2  }
}

#declare Scroll  = union { 
  object { ScrollPole }
  object { ScrollPaper }
  rotate 90*x
  scale 0.2
  translate 0.5*y
}


// 
// torch carried by our hero
//
#declare LocalLight =  light_source { 
    <0,0, LocalLightHeight>, LocalLightColor
    spotlight
    radius 40
    falloff 70 
    point_at <0,0,0>
}

#declare GlobalLight =  light_source { 
    <0, 0, GlobalLightHeight>, 0.3*GlobalLightColor
    area_light 1*x, 1*y, 3, 3
}

//==================================================================
// mostly automated stuff; touch if you dare
//==============================================================
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
      rotate 90*x // from x,y plane to x,z plane
      translate <-0.25, 0.5*TileThickness, 0.2>
      scale TileSize
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
  no_image
}
#declare Tiles[32][7] = Tiles[32][0];
#declare Tiles[32][8] = Tiles[32][0];

// multi-colored items
#for (col,0,15) 
  // !: wand
  #declare Tiles[33][col] = Potion( ANSIColors[col] )
  // ": amulet
  #declare Tiles[34][col] = Amulet( ANSIColors[col] )
  // *: gems
  #declare Tiles[42][col] = Gem( ANSIColors[col] )
  // +: book
  #declare Tiles[43][col] = Book( ANSIColors[col] )
  // ?: scroll
  // /: wand
  #declare Tiles[47][col] = Wand
  #declare Tiles[61][col] = Ring  
#end

// orange ) is a chest
#declare Tiles[40][3]  = Chest
#declare Tiles[40][11] = Chest

#declare Tiles[48][7]  = Boulder
#declare Tiles[48][0]  = Boulder
#declare Tiles[48][8]  = Boulder
#declare Tiles[48][15] = Boulder

// gray * is a rock, not a gem
#declare Tiles[42][7]  = Rock
#declare Tiles[42][0]  = Rock
#declare Tiles[42][8]  = Rock
#declare Tiles[42][15] = Rock

// %: food
// &
// # cyan: grating
// }: water
// {: fountain
// \: yellow, throne
// . cyan: ice
// } red: lava
// # green: tree
// # isolated: sink
// 0 cyan: iron ball
// _ cyan: chain
// . orange : venom
// / wand
// = ring

