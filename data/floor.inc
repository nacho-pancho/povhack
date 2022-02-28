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
  normal { crackle 1.6 }
  finish { diffuse 0.8 }
  scale <0.3,0.3,0.3>
}

#declare HallwayTexture = FloorTexture


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
