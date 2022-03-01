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
    //normal { granite 0.2 scale 1 }
    finish { diffuse 0.6 }
  },
  texture {
    pigment { color 0.3*Gray+<0.02,0.01,0.0> }
    normal { granite 0.2 scale 20 }
    finish { diffuse 0.8 }
  }
  //brick_size <8,3,4.5>
  mortar 0.2
  rotate 90*x+45*z
  scale <0.053,0.053,0.07>
}

#declare Wall = box {
  <-0.51, -0.1, 0.0 >,< 0.51, 0.1, RoomHeight>
}

#macro PutWall(X,Y,A) 
box { 
  Wall
  rotate A*z
  translate <X,Y,0>
  texture { WallTexture }
}
#end

#macro PutCorner(X,Y,A)
union { 
  // default rotation is upper left corner (northwest)
  // remember: Z increases downwards
  box { < -0.1, -0.1, 0.00 >,< 0.51,  0.10, RoomHeight> }
  box { < -0.1, -0.1, 0.00 >,< 0.10,  0.51, RoomHeight> }
  rotate A*z
  translate <X,Y,0>
  texture { WallTexture }
}
#end

#macro PutCorner2(X,Y,A)
union { 
  // double corner
  // default rotation is upper left corner (northwest)
  // remember: Z increases downwards
  box { < -0.1, -0.10, 0.00 >,< 0.51,  0.10, RoomHeight> }
  box { < -0.1, -0.10, 0.00 >,< 0.10,  0.51, RoomHeight> }
  box { <  0.1, -0.10, 0.00 >,<-0.51,  0.10, RoomHeight> }
  rotate A*z
  translate <X,Y,0>
  texture { WallTexture }
}
#end

#macro PutCorner4(X,Y)
union { 
  // 4-sides corner
  // default rotation is upper left corner (northwest)
  // remember: Z increases downwards
  box { < -0.1, -0.10, 0.00 >,< 0.51,  0.10, RoomHeight> }
  box { < -0.1, -0.10, 0.00 >,< 0.10,  0.51, RoomHeight> }
  box { <  0.1, -0.10, 0.00 >,<-0.51,  0.10, RoomHeight> }
  box { < -0.1,  0.10, 0.00 >,< 0.10, -0.51, RoomHeight> }
  translate <X,Y,0>  
  texture { WallTexture }
}
#end