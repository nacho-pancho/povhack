#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

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
