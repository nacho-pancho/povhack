#!/usr/bin/bash
povray +Ldata +L../data +i$1 +o${1/pov/png} +w1920 +h1080 +a0.1 $2 $3 $4 $5 $6 $7 $8 $9
