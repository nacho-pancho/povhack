#!/usr/bin/bash
povray +Ldata +L../data +imodel.pov +omodel.ppm +w1920 +h1080 +a +fp +d +p  $2 $3 $4 $5 $6 $7 $8 $9 2>&1 | grep -A2 -i -E "error|warning" 
