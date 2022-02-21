#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"
#include "nhglobals.inc"

#declare WallTexture = texture {
  pigment {
    agate color_map{[0.0 0.3*White][1.0 0.4*White]}
  }
  //normal { agate 0.8 }
  normal { granite 0.5 }
  scale 0.1
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
