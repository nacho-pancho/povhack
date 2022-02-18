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
  pigment { color <1.0,0.0,0.0,0.7>  } // needs to be changed
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

#declare Potion = union { 
  object {PotionFlask material { FlaskTex } } 
  object {PotionCork texture { Cork scale 0.5} } 
  object {PotionLiquid material { LiquidTex } }
}
#declare ChestTex0 = texture { 
  DMFDarkOak scale 0.1
  finish { ambient 0 diffuse 0.3 phong 0.3 phong_size 10 }
}
#declare ChestTex1 = texture { 
  Rusty_Iron
  finish { ambient 0 diffuse 0.3 }
}

#declare Chest0 = union {
  box { <-0.3,0.0,-0.25>,<0.3,0.25,0.25> }
  cylinder { <-0.3,0.0,0.0>,<0.3,0.0,0.0>,0.25 scale <1,0.5,1> translate 0.25*y }
}

#declare Chest10 = union {
  box { <-0.31,0.0,-0.26>,<-0.28,0.25,0.26> }
  cylinder { <-0.31,0,0.0>,<-0.28,0.0,0.0>,0.26 scale <1,0.5,1> translate 0.25*y }
}

#declare Chest11 = box {
  <-0.31,0.0,-0.26>,<0.32,0.03,-0.23>
}

#declare Chest12 = box {
  <-0.31,0.0,-0.26>,<-0.28,0.03, 0.26>
}

#declare Chest1 = union {
  object { Chest10 }
  object { Chest10  translate 0.2*x }
  object { Chest10  translate 0.4*x }
  object { Chest10  translate 0.6*x }
  object { Chest11  translate -0.01*x }
  object { Chest11  translate 0.5*z }
  object { Chest11  translate 0.25*y }
  object { Chest11  translate 0.5*z+0.25*y }
  object { Chest12  translate -0.01*x }
  object { Chest12  translate 0.61*x }
  object { Chest12  translate 0.61*x+0.25*y }
  object { Chest12  translate -0.01*x+0.25*y }
}

#declare Chest = union {
  object { Chest0 texture { ChestTex0 } scale <1.075,1,1> }
  object { Chest1 } texture { ChestTex1 }
  rotate 90*x+25*z 
}

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

