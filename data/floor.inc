#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"
#include "nhcolors.inc"

#declare FloorTexture = texture {
  pigment {
    //crackle color_map{[0.0 0.2*White][1.0 0.5*White]}
    crackle color_map{[0.0 0.7*Brown2][0.05 0.7*Brown2][0.05 0.3*White][1.0 0.3*White]}
  }
  normal { crackle 1.4 }
  finish { diffuse 0.8 }
  scale <0.25,0.25,0.25>
}

#declare GroundTexture = texture {
  pigment { color 0.5*Brown1 }
  normal { granite scale 0.1 }
}

#macro PutGround(X,Y)
box {
  < -0.501, -0.501, -0.02 >,< 0.501, 0.501, -0.01 >
  translate <X,Y,0>
  texture { GroundTexture }
  finish { diffuse 0.4 }
}  
#end

#macro PutFloor(X,Y)
  box {
    < -0.501, -0.501, -0.01 >,< 0.501, 0.501, 0.0 >
    translate <X,Y,0>
    texture { FloorTexture }
  }
#end

#declare Trap =
  box {
    < -0.7, -0.7, -0.21 >,< 0.7, 0.7, -0.2 >
    texture { FloorTexture }
  }

#declare Hallway = box {
  <-0.51, -0.51, -0.01>,<0.51, 0.51, 0.0>
  texture { GroundTexture }
}
