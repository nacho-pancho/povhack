#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Ladder = union {
  cylinder {<-0.2,-0.2,-1.2*RoomHeight>,<-0.2,-0.2,1.2*RoomHeight>,0.05}
  cylinder {< 0.2, 0.2,-1.2*RoomHeight>,< 0.2, 0.2,1.2*RoomHeight>,0.05}
  #for (step,-5,5)
    #declare Y = RoomHeight*(step/5)+0.1 ;
    cylinder {< -0.19, -0.19, Y>,< 0.19, 0.19, Y>,0.03 }
  #end  
  texture { DMFDarkOak scale 0.1 }
}
