#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"
#include "nhglobals.inc"
#include "nhcolors.inc"


#declare WallTextureOld = texture {
  pigment {
    agate color_map{[0.0 0.3*White][1.0 0.4*White]}
  }
  //normal { agate 0.8 }
  normal { granite 0.5 }
  scale 0.1
}

#declare WallTexture = texture {
  brick 
  texture {
    pigment { color 0.7*Brown2 }
    normal { granite 0.2 scale 1 }
    finish { diffuse 0.4 }
  }
  texture {
    pigment { color 0.2*Gray+<0.02,0.01,0.0> }
    normal { granite 0.2 scale 20 }
    finish { diffuse 0.8 }
  }
  rotate 90*x+45*z
  scale <0.053,0.053,0.07>
}

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

#declare Corner2 = union { 
  // double corner
  // default rotation is upper left corner (northwest)
  // remember: Z increases downwards
  box { < -0.1, -0.10, 0.00 >,< 0.51,  0.10, RoomHeight> }
  box { < -0.1, -0.10, 0.00 >,< 0.10,  0.51, RoomHeight> }
  box { <  0.1, -0.10, 0.00 >,<-0.51,  0.10, RoomHeight> }
  texture { WallTexture }
}

#declare Corner4 = union { 
  // 4-sides corner
  // default rotation is upper left corner (northwest)
  // remember: Z increases downwards
  box { < -0.1, -0.10, 0.00 >,< 0.51,  0.10, RoomHeight> }
  box { < -0.1, -0.10, 0.00 >,< 0.10,  0.51, RoomHeight> }
  box { <  0.1, -0.10, 0.00 >,<-0.51,  0.10, RoomHeight> }
  box { < -0.1,  0.10, 0.00 >,< 0.10, -0.51, RoomHeight> }
  texture { WallTexture }
}
