#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Rock = sphere { 
  <0.0,0.0,0.2>,0.2 
  scale <1,1,0.65> 
  pigment { color 0.2*Gray }
  normal { granite scale 0.1 }
  scale 0.3
}
