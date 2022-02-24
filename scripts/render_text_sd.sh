#!/usr/bin/bash
# see the file data/text.pov to see where the strange image size comes from
#
povray +Ldata +L../data +i$1 +fp +o${1/pov/ppm} +a +w640 +h360 +q3 -d $2 $3 $4 $5 $6 $7 $8 $9 2>&1 | grep -A2 -i -E "error|warning" 
