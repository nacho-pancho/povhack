#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

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

#declare PagesTex = texture {
  pigment { color <0.8,0.7,0.5> } 
  normal  { marble scale <0.01,0.03,0.03> rotate 90*z }
}

#macro Book(COL) 
union {
  object { 
    BookCover 
    texture {
      pigment { color COL } 
      normal { agate 0.1 scale 0.1 } 
      finish { phong 0.5 phong_size 150 brilliance 2 }
    }
  }
  object { BookPages texture { PagesTex } } 
  scale 0.3
  rotate <20,0,15>
  translate 0.4*z
}
#end
