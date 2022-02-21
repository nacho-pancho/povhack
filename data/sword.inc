#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

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
