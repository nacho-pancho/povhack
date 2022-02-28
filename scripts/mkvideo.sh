#!/bin/bash
# requires ffmpeg
# you may use some other converter
# argument is the prefix of the files
ffmpeg -i ${1}_frm_%08d.jpg -c:v libx264 -b:v 4000k -an -pass 1 -f null /dev/null
ffmpeg -i ${1}_frm_%08d.jpg -c:v libx264 -b:v 4000k -an -pass 2 ${1}.mp4

