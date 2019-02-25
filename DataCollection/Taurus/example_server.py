from __future__ import print_function
import numpy as np
import os, sys
import random, socket, time
from threading import Thread
import time

from CustomSocket import Server

SUBJECT_ID = 'S10'
TRIAL_ID = 'T6'

class ChildServer(Server):
	def __init__(self, tcp_ip, port):
		Server.__init__(self, tcp_ip, port)
		
	def run(self, only_once = False):
		print('--------- Server ---------')
		print("SUBJECT:", SUBJECT_ID, TRIAL_ID)
		while True:
			if(not self.connect_status): self.wait_for_connection()
			try:
				data = self.client.recv(self.buffer_size)
				if(data == 'info'):
					info = ','.join([SUBJECT_ID, TRIAL_ID])
					self.client.send(info)
				else:
					ts = str(time.time())
					if(len(data) != 0 and data == 'ts'): self.client.send(ts.encode('ASCII'))
					else: self.client.send(b'False')
			except Exception as exp:
				print(exp)
				print('Connection Closed')
				self.connect_status = False
				self.client.close()
				if(only_once): break

if(__name__ == '__main__'):
	tcp_ip = '192.168.1.118'
	tcp_port = 10000
	server = ChildServer(tcp_ip, tcp_port)
	server.run()
