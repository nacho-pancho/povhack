#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"

#declare IronBall = sphere {
  <0,0,0.2>,0.2
  texture {
    pigment { color <0.1,0.13,0.14> }
    finish { 
      reflection 0.25
      diffuse 0.75 
      phong 1.0
      phong_size 150
      brilliance 7
      metallic
    } 
    normal { granite 0.1 }
    scale 0.1
  }
}
