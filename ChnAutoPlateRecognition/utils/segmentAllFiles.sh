#!/bin/bash
FILES=../../AutoPlates/*.jpg
#FILES1=./images/*.JPG

for f in $FILES
do
    echo "Processing $f file..."
    # take action on each file. $f store current file name
    ../Debug/ChnAutoPlateRecognition -s $f
done
