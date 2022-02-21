#version 3.7;
#include "tileset.pov"

#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Draft = cylinder { <0,0,0>,<0,0,1>,0.2 pigment { color Red }}

#declare Meat = ovus {
  0.3, 0.1
  scale <0.35,0.5,0.3>
}

#declare Bone = union {
  intersection { 
    object { Hyperboloid_Y scale <0.25,1.0,0.25> }
    sphere { <0,0,0>,1 }
    scale 0.125
    translate 0.2*y
  }
  sphere { <0,0,0>,0.04 scale <1,0.6,1> translate 0.08*y }
  scale <0.8,1,0.8>
}

#declare Drumstick = union { 
  object { Meat 
    rotate -90*x 
    translate 0.45*z 
    texture { 
      pigment { color 0.3*Red } 
      finish { phong 0.5 phong_size 10 brilliance 2 }
    }
  }
  object { Bone rotate  90*x 
    pigment  { color <1.0,0.9,0.8> } 
  }
  scale 1.2
}

object { Wall translate  <0,-1,0>  } 
object { Floor translate <0,-1,0> } 
object { Floor translate <0,0,0> texture {FloorTexture} } 
object { Drumstick rotate 15*z+15*x }

light_source {  <  2.000,   0.000,  5.000 >, 0.25*White }

camera {
  perspective
  right (1920/1080)*x
  sky z
  location <  0, 4, 2 >
  look_at  <  0, 0, 0.5 >
  angle 40
}


