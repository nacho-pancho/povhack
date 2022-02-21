#version 3.7;
#include "tileset.pov"

#declare Draft = cylinder { <0,0,0>,<0,0,1>,0.2 pigment { color Red }}

#declare Blade0 = prism { 
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
}

// hull

// PENDING
// food   chicken drum
// tool   for all types: wrench
// weapon for all types: sword


object { Wall translate  <0,-1,0>  } 
object { Floor translate <0,-1,0> } 
object { Floor translate <0,0,0> texture {FloorTexture} } 
object { Blade0 rotate 180*x scale <0.05,0.02,0.3> texture {Silver_Metal} }

light_source {  <  10.000,   15.000,  25.000 >, 0.25*White }

camera {
  perspective
  right (1920/1080)*x
  sky z
  location <  0, 4, 2 >
  look_at  <  0, 0, 0.5 >
  angle 40
}


