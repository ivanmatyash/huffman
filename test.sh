#!/bin/bash

PATH=/usr/bin/time
PATH_TO_IN_FILE=tests/ifile
PATH_TO_OUT_FILE=out.bin

n=1
m=100
while [ $((i=n++)) -le $m ]
do
  $PATH -f "%E" -a --output=time.log ./huff $PATH_TO_IN_FILE -c $PATH_TO_OUT_FILE
done
