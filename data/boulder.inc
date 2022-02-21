#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Boulder = superellipsoid {
  <0.25, 0.25>
  translate <0, 0, 1>
  scale 0.4
  texture {
    pigment { color <0.34,0.32,0.3> }
    normal { granite 1.0 }
    scale 0.2
  }
}
