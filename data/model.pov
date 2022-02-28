#version 3.7;
#include "tileset.pov"

#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

light_source {  <  2.000,   2.000,  5.000 >, 0.25*White }

camera {
  perspective
  right (1920/1080)*x
  sky z
  location <  0, 4, 2 >
  look_at  <  0, 0, 0.5 >
  angle 40
}
PutFloor(0, 0)
PutFloor(0,-1)
object { Wall translate -y }
object { Toolbox } 
