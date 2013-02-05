#!/bin/bash
FILES=./*.jpg
#FILES1=../nonautoplates/*.jpg

i=0
for f in $FILES
do
    echo "Rename $f file..."
    # take action on each file. $f store current file name
    mv $f $i.jpg
    i=`expr $i + 1`
done

#cd ../nonautoplates
#i=0
#for f in $FILES1
#do
#    echo "Rename $f file..."
#    # take action on each file. $f store current file name
#    mv $f $i.jpg
#    i=`expr $i + 1`
#done
