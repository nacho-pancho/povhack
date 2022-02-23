#!/usr/bin/bash
povray +Ldata +L../data +i$1 +fj +o${1/pov/jpg} +w640 +h360 +a0.3 -d $2 $3 $4 $5 $6 $7 $8 $9 2>&1 | grep -A3 -i -E "error|warning" 
