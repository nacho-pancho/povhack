#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare PotionFlask = lathe {
  bezier_spline
  8
  // bottom
  <0.2    ,0.0    >, // p0
  <0.2+0.2,0.0+0.2>, // c0
  <0.1,0.6-0.15>, // c1
  <0.1,0.6    >, // p1
  // top
  <0.1     ,0.6>, // p2
  <0.1     ,0.6+0.1>, // c2
  <0.1+0.1 ,0.8>, // c3
  <0.1     ,0.8> // p3
  rotate 90*x
}

#declare FlaskTex = material {
  texture { 
    finish { 
    specular 1
    roughness 0.001
    diffuse 0.3
    reflection 0.1
    } 
    pigment { color <0.8,0.9,0.95,0.8> } 
  }
    interior { ior 1.02 }
}

#declare PotionLiquid = difference {
  object { PotionFlask scale 0.9 }
  plane  { -z,-0.4 }
  translate 0.01*z
}

#declare LiquidTex = material {
  texture {
  pigment { color Red  } 
  finish { 
    specular 1
    roughness 0.001
    diffuse 0.3
    reflection 0.1
    } 
  }
  interior { ior 1.1 }
}

#declare PotionCork = cone {
  <0.0,0.75,0.0>,0.09,
  <0.0,0.85,0.0>,0.11
  rotate 90*x
}

#macro Potion(COL)
  union { 
  object {PotionFlask material { FlaskTex } } 
  object {PotionCork texture { Cork scale 0.5} } 
  object {
    PotionLiquid 
    material { 
      LiquidTex  
      texture { pigment { COL filter 0.8 } }
    }
  }
  scale 0.3
  translate 0.1*z
}
#end
