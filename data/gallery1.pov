#version 3.7;
#include "tileset.pov"

#for (code,32,64) 
  #for (col,0,15) 
    object { Floor translate <code-32,col,0> texture {FloorTexture} } 
    object{ Tiles[code][col] translate <code-32,col,0> }
   #end
#end


light_source {  <  16.000,   20.000,  20.000 >, White }

camera {
  perspective
  right 4*x
  sky z
  location <  16, 22, 5 >
  look_at  <  16, 15, 1 >
}

