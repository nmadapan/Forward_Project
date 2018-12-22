## Tkinter modules
import Tkinter as tk
import tkMessageBox as tkmsg

## Default python modules
import os, sys, glob
from os.path import join, basename, dirname, splitext
from copy import copy, deepcopy
from threading import Thread

## External libraries
import cv2
import numpy as np

class Annotator(object):
	def __init__(self):
		###################
		## Initialize Tk ##
		###################
		self.master = tk.Tk()
		tk.Grid.rowconfigure(self.master, 0, weight = 1)
		tk.Grid.columnconfigure(self.master, 0, weight = 1)
		self.frame = tk.Frame(self.master) #, height = 400, width = 800
		self.frame.grid(row = 0, column = 0, sticky = tk.N+tk.S+tk.E+tk.W)
		# Button parameters
		self.buttonparams = {'fg' : 'Black', 'bg' : 'white', 'font':("Helvetica", 22), \
			'activebackground' : 'Green', 'bd' : 2}

		##################################
		### Video control button names ###
		##################################
		self.vid_control_buttons = ['Play', 'Select', 'Forward', 'Backward', 'Save']
		self.button_obj_vid_list = [] # Updated by a call to run()

		############################
		### Surgeme button names ###
		############################
		self.surgeme_buttons = ['S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8', 'S/F']
		self.button_obj_surg_list = [] # Updated by a call to run()

	def create_button(self, label, row = 0, column = 0):
		'''
		Description:
			Function to create a button
		Input arguments:
			label: string. Name of the button.
			row: row id
			column: column id
		'''
		tk.Grid.rowconfigure(self.frame, row, weight = 1)
		tk.Grid.columnconfigure(self.frame, column, weight = 1)
		button = tk.Button(self.frame, text = label, **self.buttonparams)
		button.grid(row = row, column = column, sticky=tk.N+tk.S+tk.E+tk.W)
		button.config(command = lambda: self.button_callback(button))
		return button

	def button_callback(self, bobj):
		if(bobj.config('relief')[-1] == 'raised'):
			bobj.config(relief = 'sunken')
			bobj.config(bg = 'Gray')
		else:
			bobj.config(relief = 'raised')
			bobj.config(bg = 'white')
		print self.get_button_status(bobj)

	def get_button_status(self, bobj):
		return bobj.config('text')[-1], bobj.config('relief')[-1]

	def get_status(self):
		dt = {}
		for bobj in self.button_obj_vid_list:
			name, status = self.get_button_status(bobj)
			dt[name] = status == 'sunken'
		for bobj in self.button_obj_surg_list:
			name, status = self.get_button_status(bobj)
			dt[name] = status == 'sunken'
		return dt

	def run(self):
		for idx, but_name in enumerate(self.vid_control_buttons):
			button = self.create_button(but_name, row = idx, column = 0)
			self.button_obj_vid_list.append(button)
		for idx, but_name in enumerate(self.surgeme_buttons):
			button = self.create_button(but_name, row = idx, column = 1)
			self.button_obj_surg_list.append(button)
		self.master.mainloop()

class VideoDisplay(object):
	def __init__(self, vpath):
		self.vpath = video_path
		self.ann_obj = Annotator()

		tk_thread = Thread(name = 'tkinter_thread', target = self.ann_obj.run) # , args = (parser, )
		tk_thread.start()

		self.run()

	def run(self):
		vcap = cv2.VideoCapture(self.vpath)
		close_flag = True
		while close_flag:
			ret, frame = vcap.read()
			if(ret):
				try:
					vid_status_dict = self.ann_obj.get_status()
				except Exception as exp:
					print 'Tkinter closed. Exiting !!'
					close_flag = False

				frame = cv2.resize(frame, None, fx = 0.5, fy = 0.5)
				cv2.imshow('Frame', frame)
				if cv2.waitKey(3) == ord('q'):
					cv2.destroyAllWindows()
					close_flag = False
			else:
				close_flag = False

if __name__ == '__main__':
	video_path = r'E:\AHRQ\AHRQ-Gesture-Videos\AHRQ_Study_II_RGB_Videos\Subject_1.mp4'
	vid_obj = VideoDisplay(vpath = video_path)
