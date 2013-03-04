#!/bin/bash
FILES=../../AutoPlates/jing*.jpg
#FILES1=./images/*.JPG

for f in $FILES
do
    echo "Detecting rectangle in $f file..."
    # take action on each file. $f store current file name
    ../Debug/DetectRectangle $f
done

#for f in $FILES1
#do
#    echo "Processing $f file..."
#    # take action on each file. $f store current file name
#    ./build/ANPR $f
#done
