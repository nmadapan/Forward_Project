from __future__ import print_function
import numpy as np
import os, sys
import random, socket, time
from threading import Thread
import time

from CustomSocket import Server

class ChildServer(Server):
	def __init__(self, tcp_ip, port):
		Server.__init__(self, tcp_ip, port)
		
	def run(self, only_once = False):
		print('--------- Server ---------')
		while True:
			if(not self.connect_status): self.wait_for_connection()
			try:
				data = self.client.recv(self.buffer_size)
				# print(data)
				ts = str(time.time())
				if(len(data) != 0): self.client.send(ts.encode('ASCII'))
				else: self.client.send(b'False')
				# time.sleep(0.5)
			except Exception as exp:
				print(exp)
				print('Connection Closed')
				self.connect_status = False
				self.client.close()
				if(only_once): break

if(__name__ == '__main__'):
	tcp_ip = 'localhost'
	tcp_port = 5000
	server = ChildServer(tcp_ip, tcp_port)
	server.run()
