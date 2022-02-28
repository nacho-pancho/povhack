#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

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
      pigment { color <0.2,0.1,0.05> } 
    }
  }
  object { Bone rotate  90*x 
    pigment  { color <1.0,0.9,0.8> } 
  }
  scale 0.4
}

