#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Ring = torus { 0.4,0.05
  scale <0.5,1,0.5> translate 0.5*z texture { Gold_Texture } 
  rotate 20*x+15*z
  scale 0.3
  translate 0.3*z
}
