#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"
#include "candle.inc"

#declare Table = union {
  box      { <-0.3, 0.0,-0.2>,< 0.3, 0.05, 0.2> }
  cylinder { <-0.15, 0.0, 0.0>,<-0.15, 0.3, 0.0>,0.07 }
  cylinder { < 0.15, 0.0, 0.0>,< 0.15, 0.3, 0.0>,0.07 }
  box      { <-0.4, 0.3,-0.3>,< 0.4, 0.4, 0.3> }
  texture { White_Marble scale 0.5 }
  rotate 90*x
}

#declare Cloth = union {
  box      {  <-0.4, 0.4,-0.2>, < 0.4, 0.41, 0.2> }
  cylinder {  <-0.4, 0.4,-0.2>, <-0.4, 0.4, 0.2>,0.01 }
  cylinder {  < 0.4, 0.4,-0.2>, < 0.4, 0.4, 0.2>,0.01 }
  box      {  <-0.41, 0.2,-0.2>, <-0.4, 0.4, 0.2> }
  box      {  < 0.4, 0.4,-0.2>, < 0.41, 0.2, 0.2> }
  rotate 90*x
}

#declare Candle = Candle_2 ( 1,   // Light on 
                  0,  // 1 = on ; 0 = off
                  1.4,  // Candle_Height, 
                  1.5,  // Candle_Intensity,  
                  0.6,  // Candle_Flame_Scale
                  4, //  3 ~ 5    
                  3     //   2,3,4
                );

#declare Altar = union {
  object { Table }
  object { Cloth pigment { color Red }  }
  object { Candle scale 0.1 rotate 90*x translate < 0.25, 0.0, 0.4> }
  object { Candle scale 0.1 rotate 90*x translate <-0.25, 0.0, 0.4> }
}
