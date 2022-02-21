#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"
#include "textures.inc"

#declare Shield00 = prism { 
  bezier_spline
  -0.3,0.3
  12,
  < -1, 0>,
  < -1, 0>,
  <  1, 0>,
  <  1, 0>,

  <  1, 0>,
  <  1,-1>,
  <  1,-2.5>,
  <  0,-3>,

  <  0,-3>,
  < -1,-2.5>,
  < -1,-1>,
  < -1, 0>
  translate 1.5*z
}
#declare Shield01 = difference {
	cylinder { <0,0,2>,<0,0,-2>,1.1 scale <1,0.2,1> translate 0.1*y }
	cylinder { <0,0,2>,<0,0,-2>,1.1 scale <1,0.2,1> translate -0.1*y }
}
#declare Shield10 = prism { 
  bezier_spline
  -0.5,0.5
  12,
  < -0.8, -0.2>,
  < -0.8, -0.2>,
  <  0.8, -0.2>,
  <  0.8, -0.2>,

  <  0.8, -0.2>,
  <  0.8, -1.5>,
  <  0.7, -2.5>,
  <  0.0, -2.8>,

  <  0.0, -2.8>,
  < -0.7, -2.5>,
  < -0.8, -1.5>,
  < -0.8, -0.2>
  translate 1.5*z
}

#declare Shield1 = intersection  { 
  object { Shield10 }
  object { Shield01 }
}

#declare Shield0 = 
  difference {
    intersection { 
      object { Shield00 } 
      object { Shield01 }  
    }
    object { Shield1 scale <1,2,1> }
}

#declare ShieldTex = texture {
  checker
  texture { pigment { color Blue } finish { phong 0.8 phong_size 100 brilliance 5 } }
  texture { pigment { color Yellow  }   finish { phong 0.8 phong_size 100 brilliance 5 } }
  scale 2
  translate 0.25*z
}

#declare Shield = union { 
  object { Shield0 texture { pigment { color Gray } finish { Metal } } }
  object { Shield1 texture { ShieldTex } }
}
