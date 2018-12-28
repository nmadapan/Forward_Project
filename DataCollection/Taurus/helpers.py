#!/usr/bin/env python

import numpy as np
import cv2
import os, sys, time, copy
from os.path import basename, dirname, join, splitext

class VideoRecord():
	def __init__(self, avi_path, width = None, height = None, fps = 30, isColor = True):
		## Instance variables
		self.avi_path = avi_path
		self.width = width
		self.height = height
		self.fps = fps
		self.isColor = isColor

		## Figuring out the timestmaps file path
		fname, _ = splitext(basename(avi_path))
		self.ts_path = join(dirname(avi_path), fname + '_ts' + '.txt')

		## Initialize the Video Writer object
		self.fourcc = cv2.VideoWriter_fourcc(*'XVID')
		if(width is not None and height is not None):
			self.vobj = cv2.VideoWriter(self.avi_path, self.fourcc, self.fps, (self.width, self.height), isColor = isColor)
		else:
			self.vobj = None

		self.tsobj = open(self.ts_path, 'w')

	def update(self, frame, ts, title = 'Visualization'):
		if(self.vobj is None):
			self.height, self.width = frame.shape[0], frame.shape[1]
			self.vobj = cv2.VideoWriter(self.avi_path, self.fourcc, self.fps, (self.width, self.height), isColor = self.isColor)

		self.vobj.write(frame)
		self.tsobj.write(str(ts)+'\n')

		return True

	def close(self):
		self.vobj.release()
		self.tsobj.flush()
		self.tsobj.close()

if __name__ == '__main__':
	vr = VideoRecord('test/hello.avi', isColor = True)

	cap = cv2.VideoCapture('test/drop.avi')
	while(cap.isOpened()):
		ret, frame = cap.read()
		if(ret): status = vr.update(frame)
		else: break
		if(not status): break

	vr.close()