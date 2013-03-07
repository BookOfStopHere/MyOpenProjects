#!/bin/bash

#SHOULD BE EXECUTE IN IMAGES DIR
FILES=./*.jpg

#FILES=../autoplates/*.jpg
#FILES=../nonautoplates/*.jpg

i=0
for f in $FILES
do
    echo "Rename $f file to $i.jpg"
    # take action on each file. $f store current file name
    mv $f $i.jpg
    i=`expr $i + 1`
done
