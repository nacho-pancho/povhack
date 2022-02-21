#!/usr/bin/bash
povray +Ldata +L../data +i$1 +fj +o${1/pov/jpg} +w1280 +h720 +a0.3 -d $2 $3 $4 $5 $6 $7 $8 $9 2>&1 | grep -i -E "error|warning" 
