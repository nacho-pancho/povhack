#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

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
  scale 0.7
  rotate 90*x+25*z 
}
