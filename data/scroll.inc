#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare ScrollPaper = difference {
  intersection { 
    object { Hyperboloid_Y scale <0.25,1.0,0.25> }
    sphere { <0,0,0>,1 }
  }
  sphere { <0,0,0>,1 scale <1,0.3,1> translate 1.2*y }
  sphere { <0,0,0>,1 scale <1,0.3,1> translate -1.2*y }
  pigment { color <0.8,0.7,0.5> } 
}

#declare ScrollPole = union {
  cylinder { <0.0,-1.1,0.0>,<0.0, 1.1,0.0>,0.1 }
  sphere   { <0.0, 0.0,0.0>,0.2 scale <1,0.6,1> translate -1.1*y }
  sphere   { <0.0, 0.0,0.0>,0.2 scale <1,0.6,1> translate  1.1*y }
  texture { DMFDarkOak scale 0.2  }
}

#declare Scroll  = union { 
  object { ScrollPole }
  object { ScrollPaper }
  scale 0.13
  rotate 90*x
  rotate -10*x-30*z
  translate 0.4*z
}
