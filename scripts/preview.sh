#!/usr/bin/bash
povray +Ldata +L../data +i$1 +o${1/pov/ppm} +w640 +h360 -a +q4 -d +fp  $2 $3 $4 $5 $6 $7 $8 $9 2>&1 | grep -i -E "error|warning" 
