## Default python libraries
import os, sys, time, copy
from os.path import basename, dirname, splitext, join, isdir, isfile
from threading import Thread
import numpy as np
import cv2

## Custom
from helpers import VideoRecord
from CustomSocket import Client

## Real sense API
import pyrealsense2 as rs

'''
How to run:
	* 
'''

##########################
### GLOBAL PARAMETERS ####
##########################

SUBJECT_ID = 'S8'
TRIAL_ID = 'T5'

BASE_WRITE_PATH = '/home/natalia/Desktop/Forward_Project/DataCollection/Taurus/data'

DEPTH_SIZE = (640, 480)
RGB_SIZE = (1920, 1080)
FPS = 30

TCP_IP = 'localhost'
PORT = 5000

class SaveData:
	def __init__(self, debug = True):

		## Instance variables
		self.debug = debug

		## Initialize the Client
		self.client = Client(TCP_IP, PORT, buffer_size = 1000000)
		if(not self.client.connect_status): self.client.init_socket()		

		## Make directory
		self.dir_path = join(BASE_WRITE_PATH, SUBJECT_ID)
		if(not isdir(self.dir_path)): os.makedirs(self.dir_path)

		## Printing for debugging purposes
		if(self.debug):
			print 'Subject ID: ', SUBJECT_ID
			print 'Trial ID: ', TRIAL_ID
			print 'RGB Image size: ', RGB_SIZE[0], ' x ', RGB_SIZE[1]
			print 'Depth Image size: ', DEPTH_SIZE[0], ' x ', DEPTH_SIZE[1]
			print 'Writing data to ', self.dir_path
			print 'Press q/e on image window to save and exit !'

		## Figure out paths
		self.f_suffix = SUBJECT_ID + '_' + TRIAL_ID
		self.rgb_path = join(self.dir_path, self.f_suffix + '_color.avi')
		self.depth_path = join(self.dir_path, self.f_suffix + '_depth.avi')

		flag = isfile(self.rgb_path) or isfile(self.depth_path)
		if(flag):
			raw_str = str(raw_input("OVER WRITING files. Are you sure ?"))
			if(raw_str in ['n', 'N', 'e', 'E', 'q', 'Q']): 
				sys.exit('Quitting')
		
		## Create custom objects for recording data
		self.rgb_obj = VideoRecord(self.rgb_path, isColor = True)
		self.depth_obj = VideoRecord(self.depth_path, isColor = False)
		
		## Realsense object
		self.pipeline = rs.pipeline()
		config = rs.config()
		config.enable_stream(rs.stream.depth, DEPTH_SIZE[0], DEPTH_SIZE[1], rs.format.z16, FPS)
		config.enable_stream(rs.stream.color, RGB_SIZE[0], RGB_SIZE[1], rs.format.bgr8, FPS)
		self.pipeline.start(config)

	def rgb_depth_recording(self):
		while(True):
			frames = self.pipeline.wait_for_frames()
			depth = frames.get_depth_frame()
			rgb = frames.get_color_frame()

			## Get timestamp from the socket 
			ts = self.client.send_data('ts')			
			
			if(depth):
				## Note: TODO: Need to convert to np.uint8 because VideoRecorder can NOT save np.uint16 grayscale images. 
				depth_img = np.asanyarray(depth.get_data()).astype(np.uint8)
				self.depth_obj.update(depth_img, ts)
				if(self.debug):
					depth_img = cv2.putText(depth_img, 'Press q/e to save & exit',(DEPTH_SIZE[0]/5, 100), \
						cv2.FONT_HERSHEY_SIMPLEX, 1, (120,0,255), 2, cv2.LINE_AA)
					cv2.imshow('Depth Image', cv2.resize(depth_img, None, fx = 0.75, fy = 0.75))
					if cv2.waitKey(1) & 0xFF == ord('q'):
						cv2.destroyAllWindows()
						break

			if(rgb):
				rgb_img = np.asanyarray(rgb.get_data())
				self.rgb_obj.update(rgb_img, ts)
				if(self.debug):
					rgb_img = cv2.putText(rgb_img, 'Press q/e to save & exit',(RGB_SIZE[0]/5, 200), \
						cv2.FONT_HERSHEY_SIMPLEX, 3, (180,0,100), 4, cv2.LINE_AA)
					cv2.imshow('RGB Image', cv2.resize(rgb_img, None, fx = 0.4, fy = 0.4))
					if cv2.waitKey(1) & 0xFF == ord('q'):
						cv2.destroyAllWindows()
						break

	def close(self):
		self.rgb_obj.close()
		self.depth_obj.close()

if __name__ == '__main__':
	sd = SaveData()
	rgb_depth_thread = Thread(name = 'rgb_depth_thread', target = sd.rgb_depth_recording)
	rgb_depth_thread.start()
	rgb_depth_thread.join()
	sd.close()
	print 'DONE !!!'
