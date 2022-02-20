#version 3.7;
#include "tileset.pov"

#declare Draft = cylinder { <0,0,0>,<0,0,1>,0.2 pigment { color Red }}


// hull

// PENDING
// food   chicken drum
// tool   for all types: wrench
// weapon for all types: sword

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

object { Wall translate  <5,-1,0>  } 
object { Floor translate <5,-1,0> } 
object { Floor translate <5,0,0> texture {FloorTexture} } 
object { Grave translate <5,0,0> }


light_source {  <  10.000,   15.000,  25.000 >, White }

camera {
  perspective
  right (1920/1080)*x
  sky z
  location <  5, 4, 2 >
  look_at  <  5, 0, 0.5 >
  angle 40
}


