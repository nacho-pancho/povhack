#!/usr/bin/bash
for i in 1 2 3 4
#for i in 4
do
    povray +Ldata  +idata/gallery${i}.pov +odata/gallery${i}.ppm +w4360 +h1080 +a0.1 +fp +d $2 $3 $4 $5 $6 $7 $8 $9
done
