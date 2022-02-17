#version 3.7;
#include "tileset.pov"

object { Floor translate <0,0,0> texture {FloorTexture} } 
object { Wall translate <0,0,0> }

object { Floor translate <1,0,0> texture {FloorTexture} } 
object { Corner translate <1,0,0> }

object { Trap translate <2,0,0> }

object { Floor translate <3,0,0> texture {FloorTexture} } 
object { Ladder translate <3,0,0> }

object { Hallway translate <4,0,0> texture {FloorTexture} } 

object { Floor translate <5,0,0> texture {FloorTexture} } 
object { Door translate <5,0,0> }

object { Floor translate <6,0,0> texture {FloorTexture} } 
object { OpenDoor translate <6,0,0> }

object { Floor translate <7,0,0> texture {FloorTexture} } 
object { ClosedDoor translate <7,0,0> }

object { Floor translate <8,0,0> texture {FloorTexture} } 
object { BrokenDoor translate <8,0,0> }

object { Floor translate <9,0,0> texture {FloorTexture} } 
object { Fountain translate <9,0,0> }


light_source {  <  5.000,   5.000,  20.000 >, White }

camera {
  perspective
  right 4*x
  sky z
  location <  5, 5, 5 >
  look_at  <  5, 0, 1 >
}

