#version 3.7;
#include "tileset.pov"

//plane { <0,0,1>,0 pigment { color <0.9,0.9,0.9,0.5> } finish { brilliance 0.5 phong 0.5 phong_size 100 } } 
//plane { <1,0,0>,0 pigment { color <0.9,0.9,0.9,0.5> } finish { brilliance 0.5 phong 0.5 phong_size 100 } } 
//plane { <0,1,0>,0 pigment { color <0.9,0.9,0.9,0.95> } }
cylinder { -2*x,2*x,0.01 pigment { color Gray  }}
cylinder { -2*y,2*y,0.01 pigment { color Gray  }}
cylinder { -2*z,2*z,0.01 pigment { color Gray  }}
text { ttf TileFont "x",0.1,0 scale 0.25 rotate 90*x translate 2.1*x pigment { color White } } 
text { ttf TileFont "y",0.1,0 scale 0.25 rotate 90*x translate 2.1*y pigment { color White } } 
text { ttf TileFont "z",0.1,0 scale 0.25 rotate 90*x translate 2.1*z pigment { color White } } 
object{ Tiles[64][14] translate <0,0,0> }


light_source {  <  2.000,   10.000,  10.000 >, White }

#declare CamLocation = < 3, 3, 3>;
#declare CamLookAt   = < 0, 0, 0.5>;
#declare CamAngle    = 70;
#declare RightLen    = 1920/1080;
//
// some geometry to find out image plane
//
#declare Direction   = 0.5*RightLen/tan(CamAngle/2)*vnormalize(CamLookAt-CamLocation);
#declare DirYZ       = vnormalize(Direction - vdot(Direction,x)*x);
#declare CameraAngleX = 180.0/pi*acos(vdot(DirYZ,-y));
#declare Dir1 = vaxis_rotate(DirYZ,x,-CameraAngleX);
#declare Dir2 = vaxis_rotate(vnormalize(Direction),x,-CameraAngleX); // should be aligned with X-Y plane
#declare CameraAngleZ = 180.0/pi*acos(vdot(Dir2,y));
#declare Dir3 = vaxis_rotate(Dir2,z,-CameraAngleZ); // should give (0,1,0) or something like that

#declare Right = RightLen*vaxis_rotate(vaxis_rotate(x,z,180+CameraAngleZ),x,0);
#declare Up = vnormalize(vcross(Direction,Right));

#warning concat("Dir  :",vstr(3,Direction,", ",8,3))
#warning concat("DirYZ:",vstr(3,DirYZ,", ",8,3))
#warning concat("AX   :",str(CameraAngleX,8,3))
#warning concat("AZ   :",str(CameraAngleZ,8,3))
#warning concat("Dir1 :",vstr(3,Dir1,", ",8,3))
#warning concat("Dir2 :",vstr(3,Dir2,", ",8,3))
#warning concat("Dir3 :",vstr(3,Dir3,", ",8,3))
#warning concat("Right :",vstr(3,Right,", ",8,3))
#warning concat("Up :",vstr(3,Up,", ",8,3))

camera {
  perspective
  right RightLen*x
  sky z
  location CamLocation
  angle 60
  look_at  CamLookAt
}

cylinder { <0,0,0>,-Direction,0.02 pigment { color Green  }}
cylinder { <0,0,0>,DirYZ,0.02 pigment { color Yellow  }} // should be on YZ plane
cylinder { <0,0,0>,Dir1,0.02 pigment { color Orange  }} //should be aligned with -Y 
cylinder { <0,0,0>,-Dir2,0.02 pigment { color Red }}    // shoul be on XY plane
cylinder { <0,0,0>,-Dir3,0.02 pigment { color Magenta }} // should be on top of ORange
cylinder { <0,0,0>,Right,0.02 pigment { color Cyan }}
cylinder { <0,0,0>,Up,0.02 pigment { color Blue }}

sphere { <0,0,0>,0.02
       translate CamLocation+Direction 
	pigment { color Yellow } 
}

text { ttf TileFont 
	"---------1---------2---------3---------4---------5---------6---------7---------8",0.1,0 
	scale 0.05 
	rotate 90*x 
	rotate CameraAngleX*x
	rotate (180+CameraAngleZ)*z 
	translate CamLocation+Direction-0.5*Right+0.5*Up
	pigment { color White } } 


text { ttf TileFont 
	"---------1---------2---------3---------4---------5---------6---------7---------8",0.1,0 
	scale 0.05 
	rotate 90*x 
	rotate CameraAngleX*x
	rotate (180+CameraAngleZ)*z 
	translate CamLocation+Direction-0.5*Right-0.5*Up
	pigment { color White } } 
