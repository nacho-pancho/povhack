#!/bin/bash
# requires ffmpeg
# you may use some other converter
# argument is the prefix of the files
ffmpeg -r 5 -i ${1}_%06d.ppm ${1}.mp4
