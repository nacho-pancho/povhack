#!/usr/bin/bash
povray +Ldata +L../data +i$1 +o${1/pov/ppm} +w960 +h540 -a +q4 -d +fp  $3 $4 $5 $6 $7 $8 $9
