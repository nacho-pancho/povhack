#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare WandTexOld = texture {
  pigment { rgb .5 }
  finish
  { specular 2 roughness .02 metallic
    reflection { .2, .8 }
  }
}

#declare WandTex = texture {
  pigment { color <0.95,0.85,0.70> } 
  finish
  { 
    diffuse 0.75
    brilliance 2
    specular 1.0 roughness 0.001
//    phong 1.0 phong_size 100    
  }
}

#declare Wand0 = julia_fractal
{ <-.4, .2, .3, -.2>
  quaternion sqr
  max_iteration 8
  precision 50
}

#declare Wand = intersection {
  object { Wand0 scale <0.9,0.025,0.025>}
  sphere { <0.5,0,0>,0.5 }
  scale 0.7
  rotate -90*y
  rotate 20*x+20*z
  translate 0.2*z
  texture { WandTex }
}
