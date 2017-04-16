#!/bin/bash

PATH_TO_TIME_FUNC=/usr/bin/time
PATH_TO_IN_FILE=tests/test.file
PATH_TO_OUT_FILE=out.bin

n=1
m=100
while [ $((i=n++)) -le $m ]
do
  #$PATH_TO_TIME_FUNC -f "%E" -a --output=huffC.log ./huff $PATH_TO_IN_FILE -c $PATH_TO_OUT_FILE
  #$PATH_TO_TIME_FUNC -f "%E" -a --output=huffD.log ./huff $PATH_TO_OUT_FILE -x decode.txt
  #$PATH_TO_TIME_FUNC -f "%E" -a --output=bzip2C.log bzip2 $PATH_TO_IN_FILE
  #$PATH_TO_TIME_FUNC -f "%E" -a --output=bzip2D.log bzip2 -d $PATH_TO_IN_FILE.bz2
  $PATH_TO_TIME_FUNC -f "%E" -a --output=gzipC.log gzip $PATH_TO_IN_FILE
  $PATH_TO_TIME_FUNC -f "%E" -a --output=gzipD.log gunzip $PATH_TO_IN_FILE.gz
done
