#!/bin/bash
# requires ImageMagick
out1=${1/map/frm}
composite -compose Lighten ${1/map/txt} ${1} -quality 90 ${out1/ppm/jpg}
