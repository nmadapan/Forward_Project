## Tkinter modules
import Tkinter as tk
import tkMessageBox as tkmsg

## Default python modules
import os, sys, glob, time
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
		self.pause = 'Pause'
		self.select = 'Select'
		self.forward = 'Forward'
		self.backward = 'Backward'
		self.save = 'Save'
		self.vid_control_buttons = [self.pause, self.select, self.forward, self.backward, self.save]
		self.button_obj_vid_list = [] # Updated by a call to run()

		############################
		### Surgeme button names ###
		############################
		self.surgeme_buttons = ['S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8']
		self.success_button = 'S/F'
		self.success_button_obj = None
		self.button_obj_surg_list = [] # Updated by a call to run()

		self.num_buttons = len(self.surgeme_buttons) + len(self.vid_control_buttons)

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
			if(bobj.config('text')[-1] in [self.pause]):
				bobj.config(bg = 'Gray')
		else:
			bobj.config(relief = 'raised')
			if(bobj.config('text')[-1] in [self.pause]):
				bobj.config(bg = 'white')
		# print self.get_button_status(bobj)

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
		button = self.create_button(self.success_button, row = idx+1, column = 1)
		self.success_button_obj = button
		self.master.mainloop()

class VideoDisplay(object):
	def __init__(self, vpath, write_fpath, title = 'Frame', fowrard_const = 30):
		self.vpath = video_path
		self.write_fpath = write_fpath
		self.window_title = title
		cv2.namedWindow(self.window_title)
		self.fconst = fowrard_const

		self.vcap = cv2.VideoCapture(self.vpath)
		self.num_frames = self.vcap.get(cv2.CAP_PROP_FRAME_COUNT)

		self.ann_obj = Annotator()

		tk_thread = Thread(name = 'tkinter_thread', target = self.ann_obj.run) # , args = (parser, )
		tk_thread.start()
		time.sleep(0.5)

		self.run()

	def refresh(self):
		try:
			self.vcap.release()
		except Exception as exp:
			pass
		self.vcap = cv2.VideoCapture(self.vpath)

	def move(self, value):
		cur_frame_count = self.vcap.get(cv2.CAP_PROP_POS_FRAMES)
		cur_frame_count += value
		if(cur_frame_count > self.num_frames): cur_frame_count = self.num_frames
		elif(cur_frame_count < 0): cur_frame_count = 0
		self.vcap.set(cv2.CAP_PROP_POS_FRAMES, cur_frame_count)

	def run(self, title = 'Frame'):
		close_flag = True
		pause_flag = False
		prev_status = None
		fl_forward = 0
		fl_backward = 0
		frame_count = 0

		time_to_select_surgeme = False

		selected_frame_ids = []
		selected_surgemes = []

		while close_flag:
			## Check Tkinter status
			try:
				status = self.ann_obj.get_status()
				if(len(status) < self.ann_obj.num_buttons): continue
				if(prev_status is None): prev_status = deepcopy(status)
			except Exception as exp:
				print exp
				print 'Tkinter closed. Exiting !!'
				close_flag = False
				continue

			if(not pause_flag):
				ret, frame = self.vcap.read()
				if(ret):
					frame = cv2.resize(frame, None, fx = 0.5, fy = 0.5)
					if(fl_forward > 0):
						cv2.putText(frame, '>>>>', (frame.shape[1]/5, 50), \
						cv2.FONT_HERSHEY_SIMPLEX, 0.5, (120,50,220),2,cv2.LINE_AA)
						fl_forward -= 1
					if(fl_backward > 0):
						cv2.putText(frame, '<<<<', (frame.shape[1]/5, 50), \
						cv2.FONT_HERSHEY_SIMPLEX, 0.5, (120,50,220),2,cv2.LINE_AA)
						fl_backward -= 1
					cv2.imshow(self.window_title, frame)
				else:
					self.vcap.set(cv2.CAP_PROP_POS_FRAMES, 0)

			## Pause flag
			pause_flag = status[self.ann_obj.pause]
			if(time_to_select_surgeme): pause_flag = True

			## Forward flag
			if(status[self.ann_obj.forward] != prev_status[self.ann_obj.forward]):
				self.move(self.fconst)
				fl_forward = self.fconst / 2

			## Backward flag
			if(status[self.ann_obj.backward] != prev_status[self.ann_obj.backward]):
				self.move(-1 * self.fconst)
				fl_backward = self.fconst / 2

			if((len(selected_frame_ids)%2 == 0) and (len(selected_frame_ids) > 0) \
			and (len(selected_surgemes) < len(selected_frame_ids)/2)):
				time_to_select_surgeme = True

			if(time_to_select_surgeme):
				## Surgeme selection
				flags = []
				for sgb in self.ann_obj.surgeme_buttons:
					flags.append(status[sgb] != prev_status[sgb])
				if(np.max(flags)):
					selected_surgemes.append(self.ann_obj.surgeme_buttons[np.argmax(flags)])
					time_to_select_surgeme = False
					print selected_surgemes

				cv2.putText(frame, 'Select a surgeme', (frame.shape[1]/5, 50), \
				cv2.FONT_HERSHEY_SIMPLEX, 1, (120,50,220),1,cv2.LINE_AA)
				cv2.imshow(self.window_title, frame)

			if(not time_to_select_surgeme):
				## Select flag
				select_flag = status[self.ann_obj.select] != prev_status[self.ann_obj.select]
				if(select_flag): selected_frame_ids.append(frame_count)
				## Save flag
				save_flag = status[self.ann_obj.save] != prev_status[self.ann_obj.save]
				if(save_flag):
					## TODO: Write the success or failure to the file
					print 'Writing to ', self.write_fpath
					with open(self.write_fpath, 'w') as fp:
						wstr = ','.join(map(str, map(int,selected_frame_ids))) + '\n'
						wstr += ','.join(selected_surgemes)
						fp.write(wstr)

			## Quitting condition
			key = cv2.waitKey(1000/30)
			if(key == 91):
				fl_backward += 10
				self.move(-1 * 10)
			if(key == 93):
				fl_forward += 10
				self.move(10)

			if key == ord('q'):
				cv2.destroyAllWindows()
				close_flag = False
				self.vcap.release()

			frame_count = self.vcap.get(cv2.CAP_PROP_POS_FRAMES)
			prev_status = deepcopy(status)

if __name__ == '__main__':
	video_path = r'E:\AHRQ\AHRQ-Gesture-Videos\AHRQ_Study_II_RGB_Videos\Subject_1.mp4'
	write_fpath = 'sample.txt'
	vid_obj = VideoDisplay(video_path, write_fpath)
