#include "textures.inc"
#include "nhglobals.inc"
#include "nhcolors.inc"

#macro PutVoid(X,Y) 
box {
  <-0.5,-0.5,0.0>,< 0.5, 0.5, 0.25*RoomHeight>
  translate <X,Y,0>
  texture {
    pigment { color 0.5*Brown1 }
    normal { granite scale 0.1 }
    finish { ambient 0 }
  }
}
#end
