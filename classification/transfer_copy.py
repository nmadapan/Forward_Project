from __future__ import print_function
import numpy as np
from helpers import *
import pickle
import os, sys, time, copy
from sklearn.model_selection import KFold
from matplotlib import pyplot as plt
from os.path import basename, dirname, splitext, join

class Transfer:
	def __init__(self, domain_one_files, domain_two_files, use_sequence = True, dlim = ' ', \
		json_param_path = 'param.json', debug = True):
		'''
		Description:
			Organizes the data to testing the transfer learning from doman one to domain two.
			For instance, domain one can be simulated data and domain two can be real data.
		Input arguments:
			* domain_one_files: (input_filename, output_filename)
			* domain_two_files: (input_filename, output_filename)
				input_filename: In this file, each row corresponds to an instance. Each instance is a list of feature values. It is a space separated file.
				output_filename: In this file, each row has the class ID (only one element). Class ID is expected to range from 1 to number of classes.
			* dlim: delimiter present in the file (input_filename)
			* json_param_path: path to the json file consisting of parameters.
			* debug: if true, prints everything.
		'''
		## Initialize variables from json param path
		param_dict = json_to_dict(json_param_path)
		for key, value in param_dict.items(): setattr(self, key, value)
		self.use_sequence = use_sequence

		## Loading the files
		print('Loading files')
		input_file_one, output_file_one = domain_one_files
		input_file_two, output_file_two = domain_two_files
		## Load domain one files
		self.x_one = np.loadtxt(input_file_one, delimiter = dlim, dtype = 'double')
		y_one = np.loadtxt(output_file_one, delimiter = dlim, dtype = 'int')
		y_one -= np.min(y_one) # so that ids start from 0
		self.y_one = np.eye(np.max(y_one)+1)[y_one, :]
		## Load domain two files
		self.x_two = np.loadtxt(input_file_two, delimiter = dlim, dtype = 'double')
		y_two = np.loadtxt(output_file_two, delimiter = dlim, dtype = 'int')
		y_two -= np.min(y_two) # so that ids start from 0. In theory, domain one and domain two should have the same set of IDs.
		self.y_two = np.eye(np.max(y_two)+1)[y_two, :]

		## Precomputed variables
		self.debug = debug
		label_to_name = json_to_dict(self.commands_filepath)
		self.class_labels = sorted(label_to_name.keys(), cmp = class_str_cmp)
		self.id_to_label = {idx: self.class_labels[idx] for idx in range(self.y_one.shape[1])}
		self.num_classes = len(self.id_to_label)
		self.num_features = self.x_one.shape[1]

		## Updated by the call to self.create_sequential_data()
		self.x_one_seq = None
		self.y_one_seq = None
		self.x_two_seq = None
		self.y_two_seq = None
		if(self.use_sequence): self.create_sequential_data()

		self.accuracy_keys = ['train_acc', 'valid_acc', 'top1', 'train_real_acc', 'valid_real_acc', 'top1_real']
		self.plot_accuracy_keys = ['train_acc', 'valid_acc', 'top1', 'top1_real']
		self.accuracy_key_labels = ['Training accuracy', 'Simulator + Real $\\rightarrow$ Simulator + Real', 'Simulator + Real $\\rightarrow$ Real', 'Real $\\rightarrow$ Real']

		self.print_stats()

	def print_stats(self):
		print('---- Domain One ----')
		if(self.use_sequence):
			print('No. of classes: ', self.y_one_seq.shape[1])
			print('No. of instances: ', self.y_one_seq.shape[0])
		else:
			print('No. of classes: ', self.y_one.shape[1])
			print('No. of instances: ', self.y_one.shape[0])

		print('---- Domain Two ----')
		if(self.use_sequence):
			print('No. of classes: ', self.y_two_seq.shape[1])
			print('No. of instances: ', self.y_two_seq.shape[0])
		else:
			print('No. of classes: ', self.y_two.shape[1])
			print('No. of instances: ', self.y_two.shape[0])
		print('--------------------')

	def is_right_dominant(self, left, right):
		'''
		Description:
			Determines what hand is dominant. It returns dominant feature type IDs followed by nondominant ones.
			If none of them are dominant, it returns RIGHT feature type IDs followed by LEFT ones
			Threshold to determine dominancy is ('dominant_first_thresh' = 0.08m). It is found by observation.
		Input arguments:
			* 'left': numpy.ndarray (_ x ~)
			* 'right': numpy.ndarray (_ x ~)
		Return:
			list of all ids. Dominant ids followed by nondominant ids.
		'''
		left_std = np.max(np.std(left, axis = 0))
		right_std = np.max(np.std(right, axis = 0))
		if((left_std - right_std) >= self.dominant_first_thresh): return False, left, right
		else: return True, right, left

	def sequence_features(self, data_in, data_out):
		'''
			* data_in: 2D numpy array. Each row is a frame. Each row consists of features.
			* data_out: 1D numpy array consisting of values ranging from 0 to num_classes - 1
		'''
		## Identify the border ids of the instances.
		data_out = data_out.flatten()
		border_ids, = np.nonzero(np.diff(data_out))
		border_ids += 1 # diff() reduces the length by one.
		border_ids = np.append(0, border_ids)

		## Obtain the instances with NON UNIFORM length
		instances = {}
		for idx in range(self.num_classes):
			instances[idx] = {}
			instances[idx]['left'] = []
			instances[idx]['right'] = []
		for idx in range(len(border_ids)-1):
			st_idx = border_ids[idx]
			end_idx = border_ids[idx+1]
			class_id = data_out[st_idx]
			instances[class_id]['left'].append(data_in[st_idx:end_idx, :self.num_features/2])
			instances[class_id]['right'].append(data_in[st_idx:end_idx, self.num_features/2:])

		# for class_id in range(self.num_classes):
		#   cinsts = instances[class_id]['left']
		#   temp = []
		#   for cinst in cinsts:
		#     temp.append(cinst.shape[0])

		## Obtain the instances with UNIFORM length
		## Interpolation
		new_instances = {}
		for idx in range(self.num_classes):
			new_instances[idx] = []
		for idx in range(len(border_ids)-1):
			st_idx = border_ids[idx]
			end_idx = border_ids[idx+1]
			class_id = data_out[st_idx]

			left = data_in[st_idx:end_idx, :self.num_features/2]
			right = data_in[st_idx:end_idx, self.num_features/2:]

			left = interpn(left, self.fixed_num_frames)
			right = interpn(right, self.fixed_num_frames)

			if(self.dominant_first):
				_, dom, ndom = self.is_right_dominant(left, right)
			else:
				dom, ndom = right, left

			## Dominant hand part goes here.
			overall = np.concatenate((dom, ndom), axis = 0)
			overall = overall.flatten()
			new_instances[class_id].append(overall)

		## Generate I/O
		new_data_in = []
		new_data_out = []
		for class_id in range(self.num_classes):
			new_data_in += new_instances[class_id]
			new_data_out += [class_id]*len(new_instances[class_id])

		return np.array(new_data_in), np.eye(self.num_classes)[new_data_out]

	def create_sequential_data(self):
		self.x_one_seq, self.y_one_seq = self.sequence_features(self.x_one, np.argmax(self.y_one, axis = 1))
		self.x_two_seq, self.y_two_seq = self.sequence_features(self.x_two, np.argmax(self.y_two, axis = 1))

	def run_same_domain(self, domain = 'one', per_transfer = 0.80, clf_type = 'svm'):
		if(self.use_sequence):
			x_same, y_same = getattr(self, 'x_'+domain+'_seq'), getattr(self, 'y_'+domain+'_seq')
		else:
			x_same, y_same = getattr(self, 'x_'+domain), getattr(self, 'y_'+domain)

		## Randomize the data in domain two
		temp = np.random.permutation(y_same.shape[0])
		x_same = x_same[temp]
		y_same = y_same[temp]

		K = int(x_same.shape[0] * per_transfer)

		## Only real
		tr_x = x_same[:K,:]
		tr_y = y_same[:K,:]

		ts_x = x_same[K+1:, :]
		ts_y = y_same[K+1:, :]

		res_real = run_clf_kfold(tr_x, tr_y, clf_type = clf_type, test_input = ts_x, test_output = ts_y, train_per = 0.6, kernel = 'linear', id_to_labels = self.id_to_label)
		return res_real

	def transfer(self, per_transfer = 0.10, clf_type = 'svm'):
		if(self.use_sequence):
			x_one, y_one = self.x_one_seq, self.y_one_seq
			x_two, y_two = self.x_two_seq, self.y_two_seq
		else:
			x_one, y_one = self.x_one, self.y_one
			x_two, y_two = self.x_two, self.y_two

		## Randomize the data in domain two
		temp = np.random.permutation(y_two.shape[0])
		x_two = x_two[temp]
		y_two = y_two[temp]

		K = int(x_one.shape[0] * per_transfer)

		tr_x = np.concatenate((x_one, x_two[:K,:]), axis = 0)
		tr_y = np.concatenate((y_one, y_two[:K,:]), axis = 0)

		## Only real
		tr_real_x = x_two[:K,:]
		tr_real_y = y_two[:K,:]

		ts_x = x_two[K+1:, :]
		ts_y = y_two[K+1:, :]

		res = run_clf_kfold(tr_x, tr_y, clf_type = clf_type, test_input = ts_x, test_output = ts_y, train_per = 0.6, kernel = 'linear', id_to_labels = self.id_to_label)

		if(K != 0):
			res_real = run_clf_kfold(tr_real_x, tr_real_y, clf_type = clf_type, test_input = ts_x, test_output = ts_y, train_per = 0.6, kernel = 'linear', id_to_labels = self.id_to_label)
			res['train_real_acc'] = res_real['train_acc']
			res['valid_real_acc'] = res_real['valid_acc']
			res['top1_real'] = res_real['top1']
		else:
			res['train_real_acc'] = [0.0]
			res['valid_real_acc'] = [0.0]
			res['top1_real'] = [0.0]

		print('Top-1 test Acc - %.02f\t%0.02f'%(np.mean(res['top1']), np.mean(res['top1_real'])))

		return res

	## This method is not sound. No. of folds depends on the percentage of transfer.
	def transfer_fold(self, per_transfer = 0.10, max_num_folds = 5, clf_type = 'svm'):
		'''
		Description:
			Assess transfer using k-fold.
			No. of folds depends on the percentage of transfer. It is not sound in that sense.
		Input arguments:
			per_transfer: percentage of data to be transferred from domain two to domain one.
			max_num_folds: maximum no. of folds. This value depends on the per_transfer.
		Return:
		'''
		## No K-Fold if there is no transfer (per_transfer = 0)
		if(per_transfer == 0.0):
			out = self.transfer(per_transfer)
			return out

		if(self.use_sequence):
			x_one, y_one = self.x_one_seq, self.y_one_seq
			x_two, y_two = self.x_two_seq, self.y_two_seq
		else:
			x_one, y_one = self.x_one, self.y_one
			x_two, y_two = self.x_two, self.y_two

		## Randomize the data in domain two
		temp = np.random.permutation(y_two.shape[0])
		x_two = x_two[temp]
		y_two = y_two[temp]

		## Identify the no. of folds based on the per_transfer
		K = int(x_one.shape[0] * per_transfer)
		n_splits = np.max([int(x_two.shape[0] / K), 2])

		## Perform kfold.
		kf = KFold(n_splits = n_splits)
		kf.get_n_splits(x_two)

		out = {}
		out['train_acc'], out['valid_acc'], out['top1'] = [], [], []
		fold_idx = 0
		for test_index, train_index in kf.split(x_two):
			if(fold_idx > max_num_folds): break
			tr_x = np.concatenate((x_one, x_two[train_index]), axis = 0)
			tr_y = np.concatenate((y_one, y_two[train_index]), axis = 0)
			ts_x = x_two[test_index]
			ts_y = y_two[test_index]
			res = run_clf_kfold(tr_x, tr_y, clf_type = clf_type, test_input = ts_x, test_output = ts_y, train_per = 0.8, kernel = 'linear', id_to_labels = self.id_to_label)
			out['train_acc'] += res['train_acc']
			out['valid_acc'] += res['valid_acc']
			out['top1'] += res['top1']
			fold_idx += 1
		return out

	def run(self):
		if(self.use_sequence):
			suffix = '_sequence'
			y_one, y_two = self.y_one_seq, self.y_two_seq
		else:
			y_one, y_two = self.y_one, self.y_two
			suffix = '_framewise'
		stat_one = plot_class_hist(np.argmax(y_one, axis = 1), self.class_labels, out_path = 'test\\simulator'+suffix+'_hist.png')
		stat_two = plot_class_hist(np.argmax(y_two, axis = 1), self.class_labels, out_path = 'test\\real'+suffix+'_hist.png')

	def write_to_csv(self, res_pkl_path, ext = 'top1', max_per = 0.1, downsample = 1):
		with open(res_pkl_path, 'rb') as fp:
			out = pickle.load(fp)

		dirp = dirname(res_pkl_path)
		fname, _ = splitext(basename(res_pkl_path))
		out_path = join(dirp, fname + '_' + ext + '.csv')

		res = []

		for index, key, label in zip(range(len(self.plot_accuracy_keys)), self.plot_accuracy_keys, self.accuracy_key_labels):
			if(key not in out): continue
			per_key_list = sorted(out[key].keys())
			for _idx, per_key in enumerate(per_key_list):
				if(len(out[key][per_key]) == 0 or per_key > max_per or _idx%downsample !=0): continue
				if(key == 'top1'):
					res.append([per_key, np.mean(out[key][per_key]), np.std(out[key][per_key])])
		np.savetxt(out_path, res, delimiter = ',', fmt = '%.02f')

	def plot_results(self, res_pkl_path, title = '', save = True):
		with open(res_pkl_path, 'rb') as fp:
			out = pickle.load(fp)

		legends = []
		for index, key, label in zip(range(len(self.plot_accuracy_keys)), self.plot_accuracy_keys, self.accuracy_key_labels):
			if(key not in out): continue
			acc_values = []
			per_key_list = sorted(out[key].keys())
			for per_key in per_key_list:
				if(len(out[key][per_key]) == 0): continue
				acc_values.append(np.mean(out[key][per_key]))
			if(len(acc_values) != 0):
				legends.append(label)
				plt.plot(per_key_list, acc_values)
		plt.legend(legends, fontsize = 12)
		plt.xlabel('Percentage of real data present in training. ', fontsize = 12)
		plt.ylabel('Accuracy', fontsize = 12)
		plt.title(title, fontsize = 14)
		plt.grid()
		if(save):
			dirp = dirname(res_pkl_path)
			fname, _ = splitext(basename(res_pkl_path))
			plt.savefig(join(dirp, fname + '.png'))
		else:
			plt.show()

if __name__ == '__main__':

	##########################
	#####   PARSING       ####
	##########################
	parser = argparse.ArgumentParser()
	parser.add_argument("-t", "--clf_type", default='svm',
						help=("Classifier type: 'svm'/'mlp'/'rf'"))
	parser.add_argument("-u", "--use_sequence", default = '1',
						help=("Whether to use sequence. 0 is false. 1 is true."))
	parser.add_argument("-o", "--out_fname", default = 'test',
						help=("Output pickle filename"))
	args = vars(parser.parse_args())
	clf_type = args['clf_type']
	use_sequence = args['use_sequence'] == '1'
	out_pkl_fname = args['out_fname']
	print('Using sequence: ', use_sequence)

	input_file_sim = r'data\taurus_sim_kinematics_feature_unified_x.txt'
	output_file_sim = r'data\taurus_sim_kinematics_feature_y.txt'
	input_file_real = r'data\taurus_kinematics_feature_unified_x.txt'
	output_file_real = r'data\taurus_kinematics_feature_y.txt'
	domain_one_files = (input_file_sim, output_file_sim)
	domain_two_files = (input_file_real, output_file_real)

	obj = Transfer(domain_one_files, domain_two_files, use_sequence = use_sequence)

	if(out_pkl_fname == 'test'):
		if(obj.use_sequence):
			out_pkl_fname = 'result_transfer_sequence_' + clf_type + '.pkl'
		else:
			out_pkl_fname = 'result_transfer_framewise_' + clf_type + '.pkl'

	# ##
	# obj.plot_results(os.path.join('test', 'result_transfer_sequence_svm.pkl'))
	# obj.run()
	# sys.exit()

	percent_list = np.linspace(0, 0.6, 60)
	out = {}
	for key in obj.accuracy_keys:
		out[key] = {}
		for percent in percent_list:
			out[key][percent] = []
	for percent in percent_list:
		st = time.time()
		for idx in range(5):
			res = obj.run_same_domain(domain = 'two', per_transfer = 0.8, clf_type = clf_type)
			# res = obj.transfer(per_transfer = percent, clf_type = clf_type)
			for acc_key in obj.accuracy_keys:
				if(acc_key in res): out[acc_key][percent] += res[acc_key]

		print('Time taken: %.02f seconds'%(time.time()-st) )
		print(percent, np.mean(out['top1'][percent]), np.std(out['top1'][percent]))
		print('')

	# 	with open(os.path.join('test', out_pkl_fname), 'wb') as fp:
	# 		pickle.dump(out, fp)

	# obj.plot_results(os.path.join('test', out_pkl_fname), title = clf_type)
