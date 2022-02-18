#!/bin/bash
# requires ffmpeg
# you may use some other converter
# argument is the prefix of the files
ffmpeg -i ${1}_%08d.ppm ${1}.mp4
