#include "colors.inc"
#include "textures.inc"
#include "stones.inc"
#include "shapes.inc"

global_settings { 
  ambient_light 0.2*White 
  assumed_gamma 1.0
}

#include "nhglobals.inc"
#include "nhobjects.inc"
#include "nhstructures.inc"

//==================================================================
// easily customizable stuff
// "easy" if you know PovRay :)
//==================================================================



#declare TileFont = "fonts/FreeMono.ttf";
#declare TileThickness = 0.1;
#declare TileShift     = 0.0;
#declare TileSize      = 0.5;

#declare LocalLightHeight  = 0.6*RoomHeight;
#declare GlobalLightHeight = 0.6*RoomHeight;


//#declare CameraHeight  = 0.7; 
#declare CameraHeight  = 4.0; 
#declare LookAtHeight  = TileSize;
#declare CamDistX = -1; // = -1;
#declare CamDistY = 3; // =  5;
#declare AspectRatio = 1920.0/1080.0;
#declare LocalLightColor = color <1.0,0.65,0.2>;
#declare GlobalLightColor = White;


#declare TileTexture = texture {
  pigment { color White }
  finish { 
    phong 1 phong_size 10 
    diffuse  0.9
  }
}

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

// 
// torch carried by our hero
//
#declare LocalLight =  light_source { 
    <0,0, LocalLightHeight>, 0.8*LocalLightColor
    spotlight
    radius 40
    falloff 70 
    point_at <0,0,0>
}

#declare GlobalLight =  light_source { 
    <0, 0, GlobalLightHeight>, 0.2*GlobalLightColor
    area_light 1*x, 1*y, 5, 5
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
// void / unknown
//
#declare Tiles[32][0] = PutVoid(0,0)
#declare Tiles[32][7] = Tiles[32][0];
#declare Tiles[32][8] = Tiles[32][0];

// multi-colored items
#for (col,0,15) 
  // !: potion
  #declare Tiles[33][col] = Potion( ANSIColors[col] )
  // ": amulet
  #declare Tiles[34][col] = Amulet( ANSIColors[col] )
  // %: food 
  #declare Tiles[37][col] = Drumstick
  // (: tool
  #declare Tiles[40][col]  = Chest
  // ): weapon
  #declare Tiles[41][col] = Sword
  // *: gems
  #declare Tiles[42][col] = Gem( ANSIColors[col] )
  // +: book
  #declare Tiles[43][col] = Book( ANSIColors[col] )
  // /: wand
  #declare Tiles[47][col] = Wand
  // ": ring
  #declare Tiles[61][col] = Ring  
  // ?: scroll
  #declare Tiles[63][col] = Scroll
  // [: shield
  #declare Tiles[91][col] = Shield
#end
// ( weapon

// ) is tool
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

#declare Tiles[95][8] = Altar
#declare Tiles[95][15] = Altar

// &
// # cyan: grating
// }: water
// {: fountain
// \: yellow, throne
// . cyan: ice
// } red: lava
// # green: tree
// # isolated: sink
// _ cyan: chain
// . orange : venom

