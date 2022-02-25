#!/bin/bash
PREFIX=$1
for i in ${PREFIX}_map_????????.pov
do
	echo "Rendering $i"
	scripts/render_hd.sh $i
done

for i in ${PREFIX}_txt_????????.pov
do
	echo "Rendering $i"
	scripts/render_txt_hd.sh $i
done

for i in ${PREFIX}_map_????????.ppm
do
	echo "Merging $i"
	scripts/merge_txt_and_map.sh $i
done
