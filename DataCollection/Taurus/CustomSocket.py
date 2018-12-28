from __future__ import print_function
import numpy as np
import os, sys
import random, socket
from threading import Thread
import time
# import cv2

## Global Static Variables
INITIAL_MESSAGE = 'Handshake'

class Server():
	def __init__(self, tcp_ip = 'localhost', tcp_port = 5000, max_clients = 1, buffer_size = 1024):
		self.tcp_ip = tcp_ip
		self.tcp_port = tcp_port
		self.max_clients = max_clients
		self.buffer_size = buffer_size

		## Socket Initialization
		self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM) # Opening the socket
		self.sock.bind((self.tcp_ip, self.tcp_port))
		self.sock.listen(self.max_clients)
		self.connect_status = False # Connection not yet established.

		self.client, self.addr = None, None
		## Waiting for establishing the connection between server and client
		# self.wait_for_connection() # In realtime, call wait_for_connection from outside.

	def run(self, only_once = True):
		########################
		# Receives a data string from a client, prints it, sends True/False to the client
		# If only_once is True, it will receive only one data string. Otherwise, it will receive infinitely.
		########################
		print('--------- Server ---------')
		while True:
			if(not self.connect_status): self.wait_for_connection()
			try:
				data = self.client.recv(self.buffer_size)
				print(data)
				if(len(data) != 0): self.client.send(b'True')
				else: self.client.send(b'False')
				time.sleep(0.5)
			except Exception as exp:
				print(exp)
				print('Connection Closed')
				self.connect_status = False
				self.client.close()
				if(only_once): break

	def wait_for_connection(self):
		######################
		# This function established connection with the client.
		######################
		print('Server: Waiting for connection:')
		self.client, self.addr = (self.sock.accept())
		data = self.client.recv(self.buffer_size)

		if data == INITIAL_MESSAGE.encode('utf-8'):
			print('Success: Received: ', data)
			self.connect_status = True
			self.client.send(b'True')
		else:
			print('Failure: Received: ', data)
			self.connect_status = False
			self.client.send(b'False')

	def send_data(self, data):
		###################
		# Purpose: sends data to the client.
		# If connect_status is True, return True. Else, return None
		###################
		if self.connect_status:
			self.client.send(data)
			return True
		else:
			return None

	def recv_data(self):
		###################
		# Purpose: receive data from the client.
		# It will return timeout exception on timeout.
		###################
		return self.client.recv(self.buffer_size)

class Client():
	def __init__(self, tcp_ip, port = 6000, buffer_size = 1024):
		# socket.setdefaulttimeout(10.0) # this time has to set based on the time taken by synapse. If less time is set exception is raised
		self.connect_status = False
		self.TCP_IP = tcp_ip
		self.TCP_PORT = port
		self.buffer_size = buffer_size
		# self.init_socket(timeout = 10) # In realtime, call wait_for_connection from outside.

	def sock_connect(self, timeout = 30):
		######################
		# Description:
		#   If already connected, return True
		#   Else, keep trying to connect forever until timeout.
		#   This functions sends 'Handshake' to the server.
		######################
		print('Client: Connecting to server .', end= '')
		if(self.connect_status):
			print('Connected!')
			return True

		start = time.time()
		while(not self.connect_status):
			try:
				self.sock = socket.socket()
				self.sock.connect((self.TCP_IP, self.TCP_PORT)) ## Blocking call. Gives time out exception on time out.
				self.connect_status = True
				self.sock.send(INITIAL_MESSAGE)
				print('. ', end= '')
				time.sleep(0.5)
			except Exception as exp:
				print('. ', end= '')
				time.sleep(0.5)
			if(time.time()-start > timeout):
				print('Connection Failed! Waited for more than ' + str(timeout) + ' seconds.')
				sys.exit(0)

	def sock_recv(self, timeout = 30, display = True):
		######################
		# Description:
		#   Infinitely wait for a delivery message after sending 'Handshake' to the server
		#   It releases when server sends 'True' back
		######################
		if(display): print('\nWaiting for delivery message: .', )
		data = None
		data_received = False
		start = time.time()
		while(not data_received):
			try:
				data = self.sock.recv(self.buffer_size) # Blocking call # Gives time out exception
				if(display): print('Received: ', data)
				if data:
					if(display): print('Success!')
					data_received = True
					break
				if(display): print('. ', end= '')
			except Exception as exp:
				if(display):print('. ', end= '')
				time.sleep(0.5)
			if(time.time()-start > timeout):
				print('No delivery message! Waited for more than ' + str(timeout) + ' seconds.')
				sys.exit(0)

		return data

	def init_socket(self, timeout = 30):
		## After creating instance of Server class. Call init_socket() for establishing the connection.
		self.sock_connect()
		self.sock_recv()

	def send_data(self, data):
		###################
		# Purpose: sends data to the server.
		# If connect_status is True, return the delivery message of the server. Else, return None
		###################
		if self.connect_status:
			self.sock.send(data)
			return self.sock.recv(self.buffer_size)
		else:
			return None

	def close(self):
		## Close the connection with the server.
		self.sock.close()

if __name__ == '__main__':
	tcp_ip = 'localhost'
	tcp_port = 5000

	server = Server(tcp_ip, tcp_port, buffer_size = 1000000)
	server_thread = Thread(name='server_thread', target=server.run)

	client = Client(tcp_ip, tcp_port, buffer_size = 1000000)
	# client_thread = Thread(name='client_thread', target=client.run)

	server_thread.start()

	idx = 0
	while(idx < 3):
		print('idx: ', idx)
		if(not client.connect_status): client.init_socket()
		try:
			## When you call server.example_run_image
			# A = np.random.randint(0, 255, (80, 80, 3))
			# astr = '_'.join(map(str, list(A.shape) + A.flatten().tolist()))
			# flag = client.send_data(astr)

			## When you call server.run
			flag = client.send_data('Hello World')
			print("RECEIVED: ", flag)
		except Exception as exp:
			print('raising exception', exp)
			client.connect_status = False
			break
		idx += 1

	print('Closing the client')
	client.close()