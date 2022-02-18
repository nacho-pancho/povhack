#version 3.7;
#include "tileset.pov"

#declare Draft = cylinder { <0,0,0>,<0,0,1>,0.2 pigment { color Red }}
#declare Boulder = superellipsoid {
  <0.25, 0.25>
  translate <0, 0, 1>
  scale 0.4
  texture {
    pigment { color <0.34,0.32,0.3> }
    normal { granite 1.0 }
    scale 0.2
  }
}

#declare IronBall = sphere {
  <0,0,0.2>,0.2
  texture {
    pigment { color <0.1,0.13,0.14> }
    finish { 
      reflection 0.25
      diffuse 0.75 
      phong 1.0
      phong_size 150
      brilliance 7
      metallic
    } 
    normal { granite 0.1 }
    scale 0.1
  }
}

#declare BookHole = union {
  cylinder { <0,0,-0.2>,<0,0,1>,0.16 scale <0.25,1,1> }
  box      { <0,-0.16,-0.2>,<0.6,0.16,1> }
}

#declare BookCover0 = union {
  cylinder { <0,0,0>,<0,0,0.7>,0.2 scale <0.25,1,1> }
  box      { <0,-0.2,0>,<0.5,0.2,0.7> }
}

#declare BookCover = difference { object { BookCover0 } object { BookHole } }

#declare BookPages0 = union {
  cylinder { <0,0,0.05>,<0,0,0.65>,0.16 scale <0.25,1,1> }
  box      { <0,-0.16,0.05>,<0.49,0.16,0.65> }
}

#declare BookPages = difference {
  object { BookPages0 }
  cylinder { <0,0,0.0>,<0,0,0.7>,0.17 scale <0.25,1,1> translate 0.5*x }
}

#declare CoverTex = texture {
  pigment { color <0.3,0.1,0.05> } 
  normal { agate 0.1 scale 0.1 } 
  finish { phong 0.5 phong_size 150 brilliance 2 }
}

#declare PagesTex = texture {
  pigment { color <0.8,0.7,0.5> } 
  normal  { marble scale <0.01,0.03,0.03> rotate 90*z }
}

#declare Book = union {
  object { BookCover texture { CoverTex } }
  object { BookPages texture { PagesTex } } 
}

// PENDING
// amulet oblong cylinder + oblong half-spheroid 
// armor  simple for all types: shield
// scroll (?)
// chain  ignore for now
// coin   leave at $
// food   chicken
// gem    diamond, a.k.a, two cylinder sweeps with polygon base
// potion spline lathe 
// ring   metal torus
// tool   for all types: wrench
// wand   a stick and a ball 
// weapon for all types: sword

object { Wall translate  <5,-1,0>  } 
object { Floor translate <5,-1,0> } 
object { Floor translate <5,0,0> texture {FloorTexture} } 
object { Book translate <5,0,0> }


light_source {  <  5.000,   5.000,  20.000 >, White }

camera {
  perspective
  right (1920/1080)*x
  sky z
  location <  5, 5, 3 >
  look_at  <  5, 0, 0.5 >
  angle 40
}

