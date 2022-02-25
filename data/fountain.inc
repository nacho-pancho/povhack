#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare CrudeFountainDish =  union {
  cylinder  { <0,0,0>,<0,0.0,0.1>,0.28 texture {Water} }
  cylinder  { <0,0,0>,<0,0.0,0.08>,0.35 texture { White_Marble } }
}

#declare CrudeFountain =  union {
  cylinder  { <0,0,0>,<0,0.0,0.9>,0.05 texture { White_Marble } }
  object { CrudeFountainDish }
  object { CrudeFountainDish scale <0.5,0.5,0.8> translate 0.6*z }
}

#declare FountainDish = lathe {
  bezier_spline
  // 6 segments
  // 2 extremes + 2 control points each 
  // total: 24 points
  // remember, the order is: node - ctrl - ctrl - node
  24
  <7,0>,
  <8,1>,
  <8,3>,
  <7,2>,

  <7,2>,
  <6,1>,
  <3,0>,
  <2,1>,

  <2,1>,
  <1,2>,
  <1,4>,
  <2,5>,

  <2,5>,
  <3,6>,
  <3,6>,
  <4,7>,

  <4,7>,
  <5,8>,
  <5,9>,
  <4,8>,

  <4,8>,
  <3,7>,
  <1,7>,
  <0,7>
}

#declare Fountain = union {
  object { 
    FountainDish 
    texture { White_Marble scale 5 } 
  }
  cylinder  { 
    <0,0,0>,<0,2,0>,7 texture {Water scale 20 } 
  }
  cylinder  { 
    <0,7,0>,<0,8,0>,4 texture {Water scale 20 } 
  }
  scale 0.05
  rotate 90*x
}
