## Tkinter modules
import Tkinter as tk
import tkMessageBox as tkmsg

## Default python modules
import os, sys, glob, time
from os.path import join, basename, dirname, splitext
from copy import copy, deepcopy
from threading import Thread
import argparse

## External libraries
import cv2
import numpy as np

'''
Requirements:
	* Python 2.7
	* OpenCV: 3.2.0

How to run:
	* Option 1:
		python AnnotationTool.py -v {path to video} -w {folder to place annotation files}
	* Option 2:
		Change the paths in this file in the main function :)

Description:
	* Running this file opens two windows: tkinter window (tk) and opencv display window (cv).
	* tk window consists of the buttons to control and annotate the video displayed on cv window.

	** tk window functions **
	* Pause - pause the video
	* Select - to select the start and end frames
	* 0.5x - reduce the fps by half
	* 2x - increase the fps by 2
	* Forward - increment the video by one second
		- You can also use ']' key to go forward
	* Backward - decrement the video by one second
		- You can also use '[' key to go backward
	* Save - Save the data (It is recommended to click it after every surgeme)
	* Delete - Delete a particular surgeme

	** Surgemes **
	* S1, S2, ..., S8 are the surgemes
	* S/F - success or failure of the surgeme (gray color indicates success)

How to annotate:
	* Run this file with appropriate command line arguments.
	* Click 'Select' when you want to select the starting point of the surgeme.
	* Click 'Select' again when you want to select the ending point of a surgeme.
	* Click one of the surgemes {S1, S2, ..., S8}
	* If it is a successful surgeme, make sure that color of 'S/F' button is gray
	* Now click 'Save'

How to delete a surgeme:
	* Use 'Forward' or 'Backward' to get in between the frame ids of a particular surgeme.
	* Hit 'Pause' when you are in the middle and the surgeme is visible
	* Hit 'Delete'
	* Hit 'Pause' again to play and continue
'''

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
		self.slow = '0.5x'
		self.fast = '2x'
		self.forward = 'Forward'
		self.backward = 'Backward'
		self.save = 'Save'
		self.clear = 'Delete'
		self.vid_control_buttons = [self.pause, self.select, self.slow, self.fast, self.forward, self.backward, self.save, self.clear]
		self.button_obj_vid_list = [] # Updated by a call to run()

		############################
		### Surgeme button names ###
		############################
		self.surgeme_buttons = ['S1', 'S2', 'S3', 'S4', 'S5', 'S6', 'S7', 'S8']
		self.success_button = 'S/F'
		self.success_button_obj = None
		self.button_obj_surg_list = [] # Updated by a call to run()
		self.all_buttons = self.vid_control_buttons + self.surgeme_buttons + [self.success_button]

		## Status dictionary
		self.status_dict = {}
		for key in self.all_buttons:
			self.status_dict[key] = False

		self.button_obj_dict = {} # Updated by a call to run()
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
		# Assign each button to a callback function
		button.config(command = lambda: self.button_callback(button))
		return button

	def button_callback(self, bobj):
		name = bobj.config('text')[-1]
		try:
			# If raised (High), make it sunken (Low)
			if(bobj.config('relief')[-1] == 'raised'):
				bobj.config(relief = 'sunken')
				# Update status dictionary
				self.status_dict[name] = True
				# Change the background of pause and S/F to gray
				if(bobj.config('text')[-1] in [self.pause, self.success_button]):
					bobj.config(bg = 'Gray')
			else:
				# If sunken (Low), make it raised (High)
				bobj.config(relief = 'raised')
				# Update status dictionary
				self.status_dict[name] = False
				# Change the background of pause and S/F to white
				if(bobj.config('text')[-1] in [self.pause, self.success_button]):
					bobj.config(bg = 'white')
		except Exception as exp:
			print exp

	def get_button_status(self, bobj):
		# bobj: button object
		# Return the name of the button and its state
		return bobj.config('text')[-1], bobj.config('relief')[-1]

	def toggle(self, but_name):
		# Given a button name, it toggles it's state.
		# but_name is a string.
		bobj = self.button_obj_dict[but_name]
		try:
			# If raised (High), make it sunken (Low)
			if(bobj.config('relief')[-1] == 'raised'):
				bobj.config(relief = 'sunken')
				# Change the background of pause and S/F to gray
				if(bobj.config('text')[-1] in [self.pause, self.success_button]):
					bobj.config(bg = 'Gray')
			else:
				# If sunken (Low), make it raised (High)
				bobj.config(relief = 'raised')
				# Change the background of pause and S/F to white
				if(bobj.config('text')[-1] in [self.pause, self.success_button]):
					bobj.config(bg = 'white')
		except Exception as exp:
			print exp

	def turn_off(self, but_name, change_bg = True):
		# Turns off the button. Meaning, it changes its state to 'sunken'
		# but_name - string (button name)
		# change_bg - If true, change the background color
		bobj = self.button_obj_dict[but_name]
		try:
			bobj.config(relief = 'sunken')
			# If change_bg is True, the background is changed to gray
			if(change_bg): bobj.config(bg = 'Gray')
		except Exception as exp:
			print exp
			print 'Proceed'
		# Update status dictionary
		self.status_dict[but_name] = True

	def get_status(self):
		# Returns the status dictionary of the tkinter window. It has the states
		# of all the buttons in the tk window.
		# keys are the button names and values are state of the buttons
		dt = {}
		for bobj in self.button_obj_vid_list:
			name, status = self.get_button_status(bobj)
			dt[name] = status == 'sunken'
		for bobj in self.button_obj_surg_list:
			name, status = self.get_button_status(bobj)
			dt[name] = status == 'sunken'
		name, status = self.get_button_status(self.success_button_obj)
		dt[name] = status == 'sunken'
		return dt

	def run(self):
		## Create all the buttons ##
		## Video control buttons
		for idx, but_name in enumerate(self.vid_control_buttons):
			button = self.create_button(but_name, row = idx, column = 0)
			self.button_obj_vid_list.append(button)
			self.button_obj_dict[but_name] = button
		## Surgeme buttons
		for idx, but_name in enumerate(self.surgeme_buttons):
			button = self.create_button(but_name, row = idx, column = 1)
			self.button_obj_surg_list.append(button)
			self.button_obj_dict[but_name] = button
		## Success/Failure button
		button = self.create_button(self.success_button, row = idx+1, column = 1)
		self.success_button_obj = button
		self.button_obj_dict[self.success_button] = button
		## Activate the tkinter
		self.master.mainloop()
		print 'Came out of mainloop'

class VideoDisplay(object):
	def __init__(self, vpath, write_fpath, title = 'Frame', fowrard_const = 30, fps = 30):
		self.vpath = video_path # Path to the video
		self.write_fpath = write_fpath # Path where to write the annotations
		self.window_title = title # Name of the title to display the video
		self.fconst = fowrard_const ## how many frames to forward/backward when you hit 'Forward'/'Backward'
		self.fps = fps # default frames per second of the video

		self.vcap = cv2.VideoCapture(self.vpath)
		self.num_frames = self.vcap.get(cv2.CAP_PROP_FRAME_COUNT)
		cv2.namedWindow(self.window_title)

		## Instantiate the Annotator object and run the tkinter in a thread
		self.ann_obj = Annotator()
		tk_thread = Thread(name = 'tkinter_thread', target = self.ann_obj.run) # , args = (parser, )
		tk_thread.start()
		time.sleep(0.5)

		## Run the video
		self.run()

	def refresh(self):
		## Refresh the current video object
		try:
			self.vcap.release()
		except Exception as exp:
			pass
		self.vcap = cv2.VideoCapture(self.vpath)

	def move(self, value):
		## Forward/backward the video by a 'value' number of frames.
		# value is the number of frames
		cur_frame_count = self.vcap.get(cv2.CAP_PROP_POS_FRAMES)
		cur_frame_count += value
		if(cur_frame_count > self.num_frames): cur_frame_count = self.num_frames
		elif(cur_frame_count < 0): cur_frame_count = 0
		self.vcap.set(cv2.CAP_PROP_POS_FRAMES, cur_frame_count)
		return cur_frame_count

	def put_text(self, frame, print_str, font = 1, color = (120,50,220), ypos = 50, show = False):
		## Display the text on the opencv window
		cv2.putText(frame, print_str, (frame.shape[1]/5, ypos), \
		cv2.FONT_HERSHEY_SIMPLEX, 0.6, color, font,cv2.LINE_AA)
		if(show):
			cv2.imshow(self.window_title, frame)

	def put_status(self, frame, frame_id, font = 1, color = (255,50,220), ypos = 200, show = False):
		## Display the frame counter on the opencv window
		print_str = str(int(frame_id)) + ' / ' + str(int(self.num_frames))
		cv2.putText(frame, print_str, (frame.shape[1]/5, ypos), \
		cv2.FONT_HERSHEY_SIMPLEX, 0.4, color, font,cv2.LINE_AA)
		if(show): cv2.imshow(self.window_title, frame)

	def put_surgemes(self, frame, frame_id, selected_frame_ids, selected_surgemes, \
					color = (255,50,120), font = 2, ypos = 300, show = False):
		'''
		Description:
			Display the surgeme on the opencv window
			If there are no surgemes, then return and do not display anything.
		Input arguments:
			* frame - np.ndarray - image
			* selected_frame_ids - list of frame numbers
			* selected_surgemes - list of surgeme names
			* color - tuple (B, G, R). Color of the text
			* font - 1/2/3. Font of the text
			* ypos - Level at which to print on y - axis.
			* show - If true, call imshow, else you need to do it outside
		'''
		if(len(selected_surgemes) == 0 or len(selected_frame_ids) < 2): return
		sframe_ids = np.array(map(int, selected_frame_ids))
		# If there are odd number of frames, make it even by ignore the last frame id
		if(sframe_ids.size % 2 == 1): sframe_ids = sframe_ids[:-1]
		# If the no. of frame ids and no. of surgemes do not match, then return
		if(sframe_ids.size / 2 != len(selected_surgemes)): return

		# Display the surgeme corresponding to the frame id
		sframe_ids = np.reshape(sframe_ids, (-1, 2))
		flags = []
		for arr in sframe_ids:
			temp = (frame_id >= arr[0] and frame_id <= arr[1])
			flags.append(temp)
		if(len(flags) > 0):
			print_str = ' '.join(map(str, np.array(selected_surgemes)[flags].tolist()))
			cv2.putText(frame, print_str, (frame.shape[1]/5, ypos), \
			cv2.FONT_HERSHEY_SIMPLEX, 0.4, color, font,cv2.LINE_AA)
		if(show): cv2.imshow(self.window_title, frame)

	def remove_surgeme(self, frame_id, selected_frame_ids, selected_surgemes, selected_success_flags):
		'''
		Description:
			Routine to delete the surgeme, when you hit 'Delete'.
			First, look for the surgeme that corresponds to the given frame id.
			Next, remove the corresponding frame ids and the surgeme
		'''
		mod_frame_ids = selected_frame_ids
		mod_surgemes = selected_surgemes
		if(len(mod_surgemes) == 0 or len(mod_frame_ids) < 2):
			return mod_frame_ids, mod_surgemes
		sframe_ids = np.array(map(int, mod_frame_ids))
		if(sframe_ids.size % 2 == 1): sframe_ids = sframe_ids[:-1]
		sframe_ids = np.reshape(sframe_ids, (-1, 2))
		flags = []
		for arr in sframe_ids:
			temp = (frame_id >= arr[0] and frame_id <= arr[1])
			flags.append(temp)
		if(np.max(flags)):
			rem_id = np.argmax(flags)
			mod_frame_ids = np.delete(sframe_ids, rem_id, 0).flatten().tolist()
			del mod_surgemes[rem_id]
			del selected_success_flags[rem_id]
		return mod_frame_ids, mod_surgemes

	def run(self, title = 'Frame'):
		close_flag = True
		pause_flag = False
		prev_status = None

		fl_count = 0 # No. of frames to print the print_str
		print_str = '' # string to print on the image window
		frame_count = 0 # Frame counter

		time_to_select_surgeme = False

		selected_frame_ids = [] # List of frame ids (start and end ids)
		selected_surgemes = [] # list of selected surgemes
		selected_success_flags = [] # S/F flags

		while close_flag:
			frame_count = self.vcap.get(cv2.CAP_PROP_POS_FRAMES)

			## Check Tkinter status
			try:
				# status = self.ann_obj.get_status()
				status = deepcopy(self.ann_obj.status_dict)
				if(len(status) < self.ann_obj.num_buttons): continue
				if(prev_status is None): prev_status = deepcopy(status)
			except Exception as exp:
				print exp
				print 'Tkinter closed. Exiting !!'
				close_flag = False
				continue

			for key in self.ann_obj.all_buttons:
				if((key not in status) or (key not in prev_status)): continue

			if(not pause_flag):
				ret, frame = self.vcap.read()
				if(ret):
					frame = cv2.resize(frame, None, fx = 0.5, fy = 0.5)
					if(fl_count > 0):
						self.put_text(frame, print_str)
						fl_count -= 1
				else:
					self.vcap.set(cv2.CAP_PROP_POS_FRAMES, 0)
					continue

			## Pause flag
			pause_flag = status[self.ann_obj.pause]
			if(time_to_select_surgeme): pause_flag = True
			if(pause_flag and (not time_to_select_surgeme)):
				self.put_text(frame, '| |', font = 1, ypos = 80)

			## Forward flag
			if(status[self.ann_obj.forward] != prev_status[self.ann_obj.forward]):
				self.move(self.fconst)
				print_str = '>>>>'
				fl_count = self.fconst / 2

			## Backward flag
			if(status[self.ann_obj.backward] != prev_status[self.ann_obj.backward]):
				self.move(-1 * self.fconst)
				print_str = '<<<<'
				fl_count = self.fconst / 2

			## Slow flag
			if(status[self.ann_obj.slow] != prev_status[self.ann_obj.slow]):
				self.fps = np.clip(self.fps / 2, 1, 240)
				print_str = '0.5x - ' + str(self.fps) + ' fps'
				fl_count = self.fconst / 4

			## Fast flag
			if(status[self.ann_obj.fast] != prev_status[self.ann_obj.fast]):
				self.fps = np.clip(self.fps * 2, 1, 240)
				print_str = '2x - ' + str(self.fps) + ' fps'
				fl_count = self.fconst / 4

			## Clear flag
			if(status[self.ann_obj.clear] != prev_status[self.ann_obj.clear]):
				selected_frame_ids, selected_surgemes = \
				self.remove_surgeme(frame_count, selected_frame_ids, \
					selected_surgemes, selected_success_flags)

			if((len(selected_frame_ids)%2 == 0) and (len(selected_frame_ids) > 0) \
			and (len(selected_surgemes) < len(selected_frame_ids)/2)):
				time_to_select_surgeme = True
				self.ann_obj.turn_off(self.ann_obj.pause)

			if(time_to_select_surgeme):
				## Surgeme selection
				flags = []
				for sgb in self.ann_obj.surgeme_buttons:
					flags.append(status[sgb] != prev_status[sgb])
				if(np.max(flags) and (len(selected_surgemes) < len(selected_frame_ids)/2)):
					selected_surgemes.append(self.ann_obj.surgeme_buttons[np.argmax(flags)])
					time_to_select_surgeme = False
					print selected_surgemes
				# else:
				# 	time_to_select_surgeme = False
				self.put_text(frame, 'Select a surgeme', color = (255, 255, 255))

			if((not time_to_select_surgeme)):
				## Select flag
				select_flag = status[self.ann_obj.select] != prev_status[self.ann_obj.select]
				if(select_flag): selected_frame_ids.append(frame_count)
				## Save flag
				save_flag = status[self.ann_obj.save] != prev_status[self.ann_obj.save]
				if(save_flag):
					success_flag = status[self.ann_obj.success_button]
					if(len(selected_surgemes) > 0 and (len(selected_surgemes) != len(selected_success_flags))):
						selected_success_flags.append(success_flag)
					## TODO: Write the success or failure to the file
					print 'Writing to ', self.write_fpath
					with open(self.write_fpath, 'w') as fp:
						temp = np.array(map(int, selected_frame_ids))
						temp = np.reshape(temp, (-1, 2))
						wstr = ''
						print selected_surgemes, temp
						for _idx, surg in enumerate(selected_surgemes):
							pair = temp[_idx]
							wstr += ' '.join(map(str, pair.tolist()+[surg, \
									str(selected_success_flags[_idx])])) + '\n'
						fp.write(wstr)

			## Quitting condition
			key = cv2.waitKey(1000/self.fps)
			if(key == 91):
				fl_count = self.fconst / 2
				print_str = '<<'
				self.move(-1 * 10)
			if(key == 93):
				fl_count = self.fconst / 2
				print_str = '>>'
				self.move(10)

			self.put_surgemes(frame, frame_count, selected_frame_ids, selected_surgemes)
			self.put_status(frame, frame_count, show = True)
			prev_status = deepcopy(status)

			if key == ord('q'):
				cv2.destroyAllWindows()
				close_flag = False
				self.vcap.release()

if __name__ == '__main__':
	video_path = r'E:\AHRQ\AHRQ-Gesture-Videos\AHRQ_Study_II_RGB_Videos\Subject_1.mp4'
	base_write_path = r'E:\Forward_Project\DataCollection'

	##########################
	#####   PARSING       ####
	##########################
	parser = argparse.ArgumentParser()
	parser.add_argument("-v", "--video_path",
						default = video_path,
						help=("Absolute path to the video."))
	parser.add_argument("-w", "--base_write_path",
						default = base_write_path,
						help=("Base write path"))
	args = vars(parser.parse_args())
	video_path = args['video_path']
	base_write_path = args['base_write_path']

	write_fname = splitext(basename(video_path))[0] + '_annot.txt'
	write_fpath = join(base_write_path, write_fname)

	vid_obj = VideoDisplay(video_path, write_fpath)
