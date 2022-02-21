#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare FountainDish =  union {
  cylinder  { <0,0,0>,<0,0.0,0.1>,0.28 texture {Water} }
  cylinder  { <0,0,0>,<0,0.0,0.08>,0.35 texture { White_Marble } }
}

#declare Fountain =  union {
  cylinder  { <0,0,0>,<0,0.0,0.9>,0.05 texture { White_Marble } }
  object { FountainDish }
  object { FountainDish scale <0.5,0.5,0.8> translate 0.6*z }
}
