#version 3.7;
#include "nhcolors.inc"
global_settings { 
  assumed_gamma 1.0 
  ambient_light 1.0
}

//
// render with Output_Alpha=on or +UA on command line
// we want to fit a 120x25 text screen on top of the rendered 3D scene
// that is an aspect ratio of 24/5

// if the aspect ration of the fonts was 1:1, we would need a screen wit aspect ratio of 24/5
// however the aspect ratio of the fonts is 3/5, so we actually need an aspect ratio
// of (24/5) * (3/5) = 72/25
//
// now, the aspect ratio of a rendered screen is 16/9, much less wide than 72/25
// to obtain the equivalent number of horizontal lines (of height 1) on a 16/9 display, 
//  we need to correct for the 16/9 aspect ration:  72/(16/9) = 40.5 (~40)
//


// +fn +d +p +a +UA  +w1920 +h400 

camera {
  orthographic
  right 1.02*72*x // 72 + 2% margin
  up 1.02*40*y    // 25 + 2% margin
}

#declare dX = 1.0/1.66; // 1.66 seems actually more exact 3.0/5.0;
#declare dY = -1.0;
#declare X0 = -36;
#declare Y0 = 20.0+dY;
#declare TerminalFont = "fonts/DejaVuSansMono.ttf";
#declare X0inv = X0+80*dX;
#declare Y0inv = Y0;
#declare X0sta = X0;
#declare Y0sta = Y0+38*dY;

#macro PutStringO(S,C,X,Y)
text { 
  ttf TerminalFont
  S
  0.1 // thickness
  0.0 // shift
  translate <X0+dX*X,Y0+dY*Y,0>
  pigment { color ANSIColors[C] }
} 
#end

// with alpha
#macro PutString(S,C,A,X,Y)
text { 
  ttf TerminalFont
  S
  0.1 // thickness
  0.0 // shift
  translate <X0+dX*X,Y0+dY*Y,0>
  pigment { color ANSIColors[C] transmit A }
} 
#end
//other:
// "fonts/FreeMonoBold.ttf" 
// "fonts/VeraMoBd.ttf" ~ DejaVu
// "fonts/LiberationMono-Bold.ttf" 
