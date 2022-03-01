#!/usr/bin/bash
povray +Ldata +L../data +i$1 +fp  +w1920 +h1080 +a0.1 -d $2 $3 $4 $5 $6 $7 $8 $9 2>&1 | grep -A4 -i -E "error|warning" 
