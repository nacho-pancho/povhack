#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Ceiling =  box {
  <-0.51,-0.51, RoomHeight>,<0.51,0.51,RoomHeight+0.01>
  texture { pigment { color White } }
  no_image
  no_shadow
}
