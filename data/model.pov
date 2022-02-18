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

#declare Amulet = union {
  cylinder { <0,0,0>,<0,-0.05,0.0>,0.4 scale <0.7,0.5,1> translate 0.5*z texture { Silver_Texture } }
  difference {
    sphere { <0,0,0>,0.325 scale <0.7,0.2,1> translate 0.5*z texture { Ruby_Glass } }
    plane  { y,0.001 }
  }
  rotate 15*z+20*x
}

#declare Ring = torus { 0.4,0.05
  scale <0.5,1,0.5> translate 0.5*z texture { Gold_Texture } 
  rotate 20*x+15*z
}

// PENDING
// armor  simple for all types: shield
// scroll 
// chain  ignore for now
// coin   leave at $
// food   chicken drum
// potion spline lathe 
// tool   for all types: wrench
// wand   a stick with a handle
// weapon for all types: sword

#declare DN = 10.0;
#declare DA = 2.0*pi/DN;

#declare Diamond0 = prism {
	  linear_spline
	  conic_sweep 
	    0, 1, DN+1,
//#for (a,0,DN)	    
//	      <cos(a*DA),sin(a*DA)>,
//#end	      
	      <cos(0),sin(0)>,
	      <cos(DA),sin(DA)>,
	      <cos(2*DA),sin(2*DA)>,
	      <cos(3*DA),sin(3*DA)>,
	      <cos(4*DA),sin(4*DA)>,
	      <cos(5*DA),sin(5*DA)>,
	      <cos(6*DA),sin(6*DA)>,
	      <cos(7*DA),sin(7*DA)>,
	      <cos(8*DA),sin(8*DA)>,
	      <cos(9*DA),sin(9*DA)>,
	      <cos(10*DA),sin(10*DA)>
      }

#declare Gem = union {
  object { Diamond0 translate -y  }
  difference { object { Diamond0 translate -y } plane { y,-0.3 } rotate 180*x }
  rotate 90*x+10*z
  texture { Yellow_Glass  } interior { ior 1.4 } 
  scale <0.2,0.2,0.25>
  translate 0.5*z
}

object { Wall translate  <5,-1,0>  } 
object { Floor translate <5,-1,0> } 
object { Floor translate <5,0,0> texture {FloorTexture} } 
object { Gem translate <5,0,0> }


light_source {  <  10.000,   15.000,  25.000 >, White }

camera {
  perspective
  right (1920/1080)*x
  sky z
  location <  5, 5, 3 >
  look_at  <  5, 0, 0.5 >
  angle 40
}

