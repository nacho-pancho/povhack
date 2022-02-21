#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"

#declare Grave = union {
  box      { <-0.4, 0.0,-0.1>,< 0.4, 0.7, 0.1> }
  cylinder { < 0.0, 0.0,-0.1>,< 0.0, 0.0, 0.1>,0.4 scale <1,0.75,1> translate 0.7*y }
  texture { 
    pigment { color Gray }
    normal  { agate 0.15 scale 0.25 }
    finish  { diffuse 0.3 } 
  }
  rotate 90*x
  rotate -30*z
}
