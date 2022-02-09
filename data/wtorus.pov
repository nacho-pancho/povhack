// This work is licensed under the Creative Commons Attribution 3.0 Unported License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View,
// California, 94041, USA.

// Persistence Of Vision raytracer version 3.5 sample file.
// A single wooden torus
// Illustrates what is possible with POV-Ray wood textures.
// File by Dan Farmer Jan 1992
//
// -w320 -h240
// -w800 -h600 +a0.3
#version  3.7;
global_settings { assumed_gamma 2.2 }

#include "shapes.inc"
#include "colors.inc"
#include "textures.inc"

#declare Brown1 = color <.3,.25,.2>;
#declare Brown2 = color <.4,.35,.3>;
#declare Brown3 = color <.5,.45,.4>;
#declare Brown4 = color <.6,.55,.5>;

// Wooden torus
#declare TorusTexture = texture {
      pigment {
         bozo
         color_map {
            [0.0 0.4 color Brown1 color Brown1 ]
            [0.4 1.01 color Brown3 color Brown3 ]
         }
         scale <4, 0.05, 0.05>
      }
   } texture {
      finish {
         phong 1
         phong_size 100
         brilliance 3
         ambient 0.2
         diffuse 0.8
      }
      pigment {
         wood
         turbulence 0.025

         color_map {
            [0.0 0.15 color Brown2 color Brown4 ]
            [0.15 0.40 color Brown4 color Clear ]
            [0.40 0.80 color Clear  color Brown4 ]
            [0.80 1.01 color Brown4 color Brown2 ]
         }

         scale <3.5, 1, 1>
         translate -50*y
         rotate 1.5*z
      }
   }

torus { 7.0 3.0 texture { TorusTexture } }

// Main light source
light_source { <-50.0, 100, -80.0> colour White }

// Dim side light to fill shadows
light_source { <250.0, 25.0, -100.0> colour DimGray }


camera {
   location <0.0, 20.0, -15.0>
   angle 65 
   right     x*image_width/image_height
   look_at <0, 0, 0>
}
