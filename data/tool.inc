#include "colors.inc"

// red tool box
#declare Box = difference {
  superellipsoid { <0.1, 0.1> scale <0.2,0.12,0.1> }
  box { <-0.3,-0.3,0.040>,<0.3,0.3,0.055> }
  pigment { Red }
  finish { phong 1 phong_size 200 brilliance 2 }
  translate 0.1*z
}

#declare Handle = torus { 
  0.07,0.015
  scale <1,1,0.5>
  translate 0.2*z
  texture { Chrome_Metal }
}

#declare BoxInside = box {
  <-0.19,-0.11,0.04>,<0.19,0.11,0.055>
  translate 0.1*z
  pigment { White } 
}

#declare Toolbox = union {
  object { Box }
  object { Handle }
  object { BoxInside }
}

