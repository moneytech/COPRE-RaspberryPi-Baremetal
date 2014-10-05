#!/usr/bin/python
######################################################
# CT6COPRE Project
# By:
# 	Niall Frederick Weedon (nweedon)
# 	Tim Stanley (Spike71m)
######################################################

# PyPNG by drj11:
# https://github.com/drj11/pypng
import png
import argparse
import os
import struct

parser = argparse.ArgumentParser(description='Convert PNG Image files to raw binaries')
parser.add_argument('--image', dest='image', help='The image file to convert')
args = parser.parse_args()

if args.image:
	r = png.Reader(args.image)

	data = list(r.read()[2])
	filename = args.image.split('/');
	filename = filename[len(filename) - 1]

	# Check output directory exists
	if not os.path.exists(os.path.join(os.getcwd(), 'bin')):
		os.mkdir('bin')

	# Open file for writing
	f = open("bin/" + filename + ".bin", 'wb')

	# Pixels are stored in ARGB format
	for x in data:
		for i in range(0, len(x), 3):
			raw = (255 << 24) + (x[i + 2] << 16) + (x[i + 1] << 8) + x[i]
			f.write(struct.pack('I', raw))

	f.close()
else:
	print "Error: Image name not provided."
	quit(1)