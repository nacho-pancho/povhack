#include "colors.inc"
#include "stones.inc"
#include "shapes.inc"

#declare CrystalMat = material {
  texture { 
    finish { 
      specular 1
      roughness 0.001
      diffuse 0.3
      reflection 0.1
    } 
    pigment { color 0.7*White filter 0.8  }
  } 
  interior { ior 1.4 } 
}
#declare DN = 10.0;
#declare DA = 2.0*pi/DN;

#declare Diamond0 = prism {
	  linear_spline
	  conic_sweep 
	    0, 1, DN+1,
//#for (a,0,DN)	    
//	      <cos(a*DA),sin(a*DA)>,
//#end	      
	      <cos(0),sin(0)>,
	      <cos(DA),sin(DA)>,
	      <cos(2*DA),sin(2*DA)>,
	      <cos(3*DA),sin(3*DA)>,
	      <cos(4*DA),sin(4*DA)>,
	      <cos(5*DA),sin(5*DA)>,
	      <cos(6*DA),sin(6*DA)>,
	      <cos(7*DA),sin(7*DA)>,
	      <cos(8*DA),sin(8*DA)>,
	      <cos(9*DA),sin(9*DA)>,
	      <cos(10*DA),sin(10*DA)>
      }


#macro Gem(COL) 
union {
  object { Diamond0 translate -y  }
  difference { 
    object { 
      Diamond0 
      translate -y 
    } 
    plane { y,-0.3 } 
    rotate 180*x 
  }
  rotate 90*x+10*z
  material { 
    CrystalMat 
    texture { pigment { color COL filter 1.0 } }
  }
  scale <0.2,0.2,0.25>
  scale 0.6
  translate 0.3*z
}
#end
