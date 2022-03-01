#!/bin/bash
PREFIX=$1
for i in ${PREFIX}_map_????????.pov
do
	if [[ ! -f ${i/pov/ppm} ]]
	then
          echo "Rendering $i"
	  scripts/render_fhd.sh $i
	fi
done

for i in ${PREFIX}_txt_????????.pov
do
	if [[ ! -f ${i/pov/ppm} ]]
	then
	  echo "Rendering $i"
          scripts/render_text_fhd.sh $i
	fi
done

for i in ${PREFIX}_map_????????.ppm
do
	echo "Merging $i"
	scripts/merge_txt_and_map.sh $i
done
scripts/mkvideo.sh $1

