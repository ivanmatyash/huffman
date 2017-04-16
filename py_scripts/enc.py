#!/usr/bin/env python

import pylab
bzip2_array = []
gzip_array = []
huff_array = []
lha_array = []

bzip2_array_av = []
gzip_array_av = []
huff_array_av = []
lha_array_av = []

f = open('bzip2C.log')
for line in f:
	bzip2_array.append(float(line))

f = open('gzipC.log')
for line in f:
	gzip_array.append(float(line))

f = open('huffC.log')
for line in f:
	huff_array.append(float(line))

f = open('lhaC.log')
for line in f:
	lha_array.append(float(line))

bzip2_average = sum(bzip2_array) / len(bzip2_array)
gzip_average = sum(gzip_array) / len(gzip_array)
huff_average = sum(huff_array) / len(huff_array)
lha_average = sum(lha_array) / len(lha_array)

for i in range(100):
	bzip2_array_av.append(bzip2_average)
	gzip_array_av.append(gzip_average)
	huff_array_av.append(huff_average)
	lha_array_av.append(lha_average)

pylab.title('Archiving, file #5')
pylab.ylabel('archiving time, seconds')
pylab.plot(bzip2_array, label = 'bzip2', color = 'blue')
pylab.plot(bzip2_array_av, 'k--', color = 'blue') 
pylab.plot(gzip_array, label = 'gzip', color = 'green')
pylab.plot(gzip_array_av, 'k--', color = 'green');
pylab.plot(lha_array, label = 'lha', color = 'cyan')
pylab.plot(lha_array_av, 'k--', color = 'cyan')
pylab.plot(huff_array, label = 'huffman', color = 'red')
pylab.plot(huff_array_av, 'k--', color = 'red')
pylab.legend(loc = 'best')
pylab.show()
