#version 3.7;
#include "tileset.pov"

#declare Draft = cylinder { <0,0,0>,<0,0,1>,0.2 pigment { color Red }}


object { Wall translate  <5,-1,0>  } 
object { Floor translate <5,-1,0> } 
object { Floor translate <5,0,0> texture {FloorTexture} } 
object { Chest translate <5,0,0> }


light_source {  <  10.000,   15.000,  25.000 >, White }

camera {
  perspective
  right (1920/1080)*x
  sky z
  location <  5, 4, 2 >
  look_at  <  5, 0, 0.5 >
  angle 40
}

// PENDING
// armor  simple for all types: shield
// scroll 
// food   chicken drum
// tool   for all types: wrench
// weapon for all types: sword
// rock   *

