#!/usr/bin/python
######################################################
# CT6COPRE Project
# By:
# 	Niall Frederick Weedon (nweedon)
# 	Tim Stanley (Spike71m)
######################################################
from PIL import Image
#import Image
import argparse
import os
import struct

parser = argparse.ArgumentParser(description='Convert PNG Image files to raw binaries')
parser.add_argument('--image', dest='image', help='The image file to convert')
args = parser.parse_args()

if args.image:
	try:
		im = Image.open(args.image)
	except IOError:
		print "Error: File does not exist."
		quit(1)

	data = list(im.getdata())
	filename = args.image.split('/');
	filename = filename[len(filename) - 1]

	# Check output directory exists
	if not os.path.exists(os.path.join(os.getcwd(), 'bin')):
		os.mkdir('bin')

	# Open file for writing
	f = open("bin/" + filename + ".bin", 'wb')

	# Pixels are stored in ARGB format
	for x in data:
		raw = (255 << 24) + (x[2] << 16) + (x[1] << 8) + x[0]
		f.write(struct.pack('I', raw))

	f.close()
else:
	print "Error: Image name not provided."
	quit(1)