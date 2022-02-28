#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"
#include "nhcolors.inc"

#declare DoorThickness = 0.03;
#declare DoorSize = 0.25;

#declare DoorTexture = texture {
  pigment {
    bozo
    color_map {
      [0.0 0.4 color 0.3*Brown1 color 0.3*Brown1 ]
      [0.4 1.01 color 0.4*Brown3 color 0.4*Brown3 ]
    }
    scale <4, 0.05, 0.05>
  }
  rotate y*45
  scale 0.1
  } 
  texture {
  finish {
    brilliance 2
    diffuse 0.6
  }
  pigment {
    wood
    turbulence 0.025
    
    color_map {
      [0.0  0.30 color 0.3*Brown2 color 0.3*Brown4 ]
      [0.30 0.40 color 0.3*Brown4 color 0.3*Brown5 ]
      [0.40 0.50 color 0.3*Brown5 color 0.3*Brown4 ]
      [0.85 1.01 color 0.3*Brown4 color 0.3*Brown2 ]
    }
    
    scale <3.5, 1, 1>
    translate -50*z
    rotate 1.5*y
  }
  rotate 45*z
  scale 0.3
}

#declare Door = union {
  box { <-DoorSize, -DoorThickness, 0.0 >,< DoorSize, DoorThickness, 0.6*RoomHeight>}
  cylinder { <0.0, -DoorThickness, 0.6*RoomHeight>,<0.0, DoorThickness, 0.6*RoomHeight>,DoorSize }
  texture { DoorTexture }
}

#declare DoorHole = union {
  box { <-DoorSize, -1, -0.05 >,< DoorSize, 1, 0.6*RoomHeight>}
  cylinder { <0.0, -1, 0.6*RoomHeight>,<0.0, 1, 0.6*RoomHeight>,DoorSize }
}

#declare BrokenDoor = difference {
  object { Wall }
  object { DoorHole }
  texture { WallTexture }
}

#declare ClosedDoor = union {
  object { BrokenDoor }
  object { Door texture { DoorTexture } }
}

#declare OpenDoor = union {
  object { BrokenDoor }
  object { Door texture { DoorTexture } rotate 90*z translate <-DoorSize,-DoorSize,0> }
}
