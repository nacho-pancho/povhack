#version 3.7;
#include "tileset.pov"

#declare Draft = cylinder { <0,0,0>,<0,0,1>,0.2 pigment { color Red }}

#declare Blade = prism { 
  conic_sweep
  bezier_spline
  0.0,1.0
  12,
  < -1, 0>,
  < -1, 0>,
  <  1, 0>,
  <  1, 0>,

  <  1, 0>,
  <  1,-3>,
  <  1,-2.5>,
  <  0,-3>,

  <  0,-3>,
  < -1,-2.5>,
  < -1,-3>,
  < -1, 0>
  translate -1.0*y 
  scale <0.05,0.02,0.3>  
  rotate 180*x translate 0.4*z 
  texture { Chrome_Metal pigment { color 0.3*White } } 
}

#declare Handle1 = box {
  <-0.5, -0.5, -0.5>,< 0.5, 0.5, 0.5>
  rotate 45*y
  scale <1.0,0.1,0.3>
  rotate 90*x
  scale 0.25
}
// hull

// PENDING
// food   chicken drum
// tool   for all types: wrench
// sink (needs code for detection)
// grating (needs code for detection)
// 

#declare Handle2 = intersection {
  box { 
    <-0.5, -0.5, -0.5>,< 0.5, 0.5, 0.5> 
  }
  sphere { <0,0,0>,1 scale <0.1,1.0,0.1> translate 0.25*y }
  translate 0.5*y
  rotate 90*x
  scale <0.4,0.4,0.3>
}

#declare Handle3 = sphere { 
  <0,0,0>,0.1
  translate 0.1*z
  scale 1.2*<0.4,0.4,0.3>
}
#declare Handle = union {
  object { Handle1 translate 0.4*z pigment { color  Yellow } texture { Silver_Metal } }
  object { Handle2 translate 0.1*z pigment { color  0.3*Red } }
  object { Handle3 translate 0.05*z pigment { color  Blue } texture { Silver_Metal } }
}

#declare Sword = union {
  object { Blade }
  object { Handle } 
  scale 0.7
}
object { Wall translate  <0,-1,0>  } 
object { Floor translate <0,-1,0> } 
object { Floor translate <0,0,0> texture {FloorTexture} } 
object { Sword rotate 20*z+10*x }

light_source {  <  2.000,   0.000,  5.000 >, 0.25*White }

camera {
  perspective
  right (1920/1080)*x
  sky z
  location <  0, 4, 2 >
  look_at  <  0, 0, 0.5 >
  angle 40
}


