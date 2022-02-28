#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare RockTexture = texture {
  pigment { color 0.2*Gray }
  normal { granite scale 0.1 }
}

#declare Rock = sphere { 
  <0.0,0.0,0.2>,0.2 
  scale <1,1,0.65> 
  texture { RockTexture }
  scale 0.3
}
