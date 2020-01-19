from __future__ import print_function
import numpy as np
import os, sys, time
from copy import deepcopy
from glob import glob
import json
from scipy.interpolate import interp1d
from matplotlib import pyplot as plt
from sklearn import svm
from sklearn.metrics import confusion_matrix
from random import shuffle
import itertools
from sklearn.model_selection import KFold
import argparse
from sklearn.ensemble import RandomForestClassifier, AdaBoostClassifier
from sklearn.neural_network import MLPClassifier
import math

def json_to_dict(json_filepath):
	if(not os.path.isfile(json_filepath)):
		raise IOError('Error! Json file: '+json_filepath+' does NOT exists!')
	with open(json_filepath, 'r') as fp:
		var = json.load(fp)
	return var

def class_str_cmp(str1, str2):
	'''
	Description:
		Compare two class_strings. Format of the class_string is the following: ('S1'). Refer to commands_*.json for all available class_strings.
	Input arguments:
		* str1: a class string.
		* str2: a class string.
	Return:
		* Negative number if str2 is smaller than str1, positive number otherwise.
	Example usage:
		# class_str_cmp('S1', 'S2')
		# S2 is supposed to come after S1. So it returns a positive number
	'''
	int1 = int(str1[1:])
	int2 = int(str2[1:])
	return int1 - int2

def sync_ts(skel_ts, rgb_ts):
	'''
	Description:
		Synchronize two lists of time stamps.
	Input arguments:
		* skel_ts: list of skeleton time stamps. [t1, t2, ..., ta]
		* rgb_ts: list of rgb times tamps. [t1, t2, t3, ..., tb]
	Return:
		* A tuple (apply_on_skel, apply_on_rgb)
			apply_on_skel: What is the corresponding rgb time stamp for every skeleton time stamp.
			apply_on_rgb: What is the corresponding skeleton time stamp for every rgb time stamp.
	'''
	skel_ts = np.reshape(skel_ts, (-1, 1))
	rgb_ts = np.reshape(rgb_ts, (1, -1))
	M = np.abs(skel_ts - rgb_ts)
	apply_on_skel = np.argmin(M, axis = 0)
	apply_on_rgb = np.argmin(M, axis = 1)
	return apply_on_skel.tolist(), apply_on_rgb.tolist()

def interpn(yp, num_points, kind = 'linear'):
	'''
	Description:
		Inpterpolation the given 2D np.ndarray along the rows.
	Input arguments:
		* yp: np.ndarray of shape nr x nc
		* num_points: desired no. of rows
		* kind: type of interpolation. linear or poly. Refer to interp1d of scipy.interpolate for more types of interpolation.
	Return:
		np.ndarray with no. of rows equal to num_points and no. of columns same as 'yp'.
	'''
	# yp is a gesture instance
	# yp is 2D numpy array of size num_frames x 3 if num_joints = 1
	# No. of frames will be increased/reduced to num_points
	xp = np.linspace(0, 1, num = yp.shape[0])
	x = np.linspace(0, 1, num = num_points)
	y = np.zeros((x.size, yp.shape[1]))
	yp = np.float64(yp)
	for dim in range(yp.shape[1]):
		f = interp1d(xp, yp[:, dim], kind = kind)
		y[:, dim] = f(x)
	return y

def print_dict(dict_inst, idx = 1):
	for key, value in dict_inst.items():
		if(isinstance(value, dict)):
			print('\t'*(idx-1), key, ': ')
			print_dict(value, idx = idx+1)
		else:
			print('\t'*idx, key, ': ', end = '')
			if(isinstance(value, np.ndarray)):
				print(value.shape)
			else: print(value)

def plot_confusion_matrix(cm, classes, normalize=False, title='Confusion matrix', \
		cmap = plt.cm.Blues, display = False, write_path = ''):
	"""
	This function prints and plots the confusion matrix.
	Normalization can be applied by setting `normalize=True`.
	"""
	if normalize:
		cm = 100 * (cm.astype('float') / cm.sum(axis=1)[:, np.newaxis])
		cm = cm.astype('int')
	else:
		print('Confusion matrix, without normalization')

	# print(cm)
	plt.figure()

	np.set_printoptions(precision=0)
	plt.imshow(cm, interpolation='nearest', cmap=cmap)
	# plt.title(title, fontsize = 14)
	plt.colorbar()
	tick_marks = np.arange(len(classes))
	plt.xticks(tick_marks, classes, rotation=90)
	plt.yticks(tick_marks, classes)

	fmt = 'd' if normalize else 'd'
	thresh = cm.max() / 2.
	for i, j in itertools.product(range(cm.shape[0]), range(cm.shape[1])):
		plt.text(j, i, format(cm[i, j], fmt), horizontalalignment="center", \
			color="white" if cm[i, j] > thresh else "black")

	# plt.tight_layout()
	plt.ylabel('True label', fontsize = 14)
	plt.xlabel('Predicted label', fontsize = 14)
	if(display): plt.show()
	if(len(write_path) != 0): plt.savefig(write_path)

def plot_class_hist(output_ids, class_labels, normalize = False, out_path = ''):
	'''
	Description:
		Plots the number of instances per each class as a histogram
	Input arguments:
		* feat_ext: object to the class FeatureExtractor
	Return:
		None
	'''
	## Plot Histogram - No. of instances per class
	labels = np.unique(output_ids)
	y_pos = np.arange(len(labels))
	performance = [np.sum(output_ids == label) for label in labels]
	nperformance = [np.sum(output_ids == label)/float(len(output_ids)) for label in labels]

	plt.figure()
	if(normalize): plt.bar(y_pos, nperformance, align='center', alpha=0.5)
	else: plt.bar(y_pos, performance, align='center', alpha=0.5)
	# plt.xticks(y_pos, objects)
	plt.xlabel('Class IDs', fontsize = 12)
	plt.ylabel('Ratio of no. of instances', fontsize = 12)
	plt.xticks(y_pos, class_labels)
	if(normalize): plt.title('Normalized instance histogram', fontsize = 14)
	else: plt.title('Instance histogram', fontsize = 14)
	plt.grid(True)
	if(out_path == ''): plt.show()
	else: plt.savefig(out_path)
	return {a: b for a, b in zip(labels, performance)}

def run_svm(data_input, data_output, test_input = None, test_output = None, \
	train_per = 0.8, kernel = 'linear', inst_var_name = 'clf', display = False, \
	id_to_labels = None):
	result = {}

	num_inst = data_input.shape[0]
	feat_dim = data_input.shape[1]
	num_classes = data_output.shape[1]

	## randomize
	perm = list(range(data_input.shape[0]))
	shuffle(perm)
	K = int(train_per*num_inst)
	train_input = data_input[perm[:K], :]
	train_output = data_output[perm[:K], :]
	valid_input = data_input[perm[K:], :]
	valid_output = data_output[perm[K:], :]

	# Train
	clf = svm.SVC(decision_function_shape='ova', kernel = kernel, probability = True, class_weight = 'balanced')
	clf.fit(train_input, np.argmax(train_output, axis =1 ))

	# Train Predict
	pred_train_output = clf.predict(train_input)
	train_acc = float(np.sum(pred_train_output == np.argmax(train_output, axis = 1))) / pred_train_output.size
	result['train_acc'] = [train_acc]
	print('Train Acc: %.04f'%train_acc)

	# Validation Predict
	pred_valid_output = clf.predict(valid_input)
	valid_acc = float(np.sum(pred_valid_output == np.argmax(valid_output, axis = 1))) / pred_valid_output.size
	result['valid_acc'] = [valid_acc]
	print('Valid Acc: %.04f'%valid_acc)

	if(display):
		conf_mat = confusion_matrix(np.argmax(valid_output, axis = 1), pred_valid_output)
		cname_list = []
		for idx in range(num_classes):
			if(id_to_labels is not None):
				cname_list.append(id_to_labels[idx])
			else:
				cname_list.append(str(idx))
		plot_confusion_matrix(conf_mat, cname_list, normalize = True, title = 'Validation Confusion Matrix')

	## Test Predict
	if(test_input is not None):
		pred_test_output = clf.predict(test_input)

		# ## Compute top - 3/5 accuracy
		# pred_test_prob = clf.predict_proba(test_input)
		# temp = np.sum(np.argsort(pred_test_prob, axis = 1)[:,-5:] == np.argmax(test_output, axis = 1).reshape(-1, 1), axis = 1) > 0
		# result['test_acc_top5'] = np.mean(temp)
		# print('LOSO - Top-5 Acc - ', np.mean(temp))
		# temp = np.sum(np.argsort(pred_test_prob, axis = 1)[:,-3:] == np.argmax(test_output, axis = 1).reshape(-1, 1), axis = 1) > 0
		# result['test_acc_top3'] = np.mean(temp)
		# print('LOSO - Top-3 Acc - ', np.mean(temp))

		test_acc = float(np.sum(pred_test_output == np.argmax(test_output, axis = 1))) / pred_test_output.size
		result['top1'] = [test_acc]
		print('Top-1 Test Acc - %.04f'%test_acc)
		if(display):
			conf_mat = confusion_matrix(np.argmax(test_output, axis = 1), pred_test_output)
			cname_list = []
			for idx in range(test_output.shape[1]):
				if(id_to_labels is not None):
					cname_list.append(id_to_labels[idx])
				else:
					cname_list.append(str(idx))
			plot_confusion_matrix(conf_mat, cname_list, normalize = True, title = 'LOSO Confusion Matrix')

	result['clf'] = deepcopy(clf)

	return result

def run_clf_kfold(data_input, data_output, clf_type = 'svm', test_input = None, test_output = None, \
	train_per = 0.8, kernel = 'linear', inst_var_name = 'clf', display = False, \
	id_to_labels = None, num_folds = 5, write_path = ''):

	data_input = deepcopy(data_input)
	data_output = deepcopy(data_output)

	result = {}
	result['train_acc'] = []
	result['valid_acc'] = []
	result['top1'] = []
	result['clf'] = []

	num_inst = data_input.shape[0]
	feat_dim = data_input.shape[1]
	num_classes = data_output.shape[1]

	## randomize
	perm = np.random.permutation(data_input.shape[0])
	data_input = data_input[perm]
	data_output = data_output[perm]

	## Perform kfold.
	kf = KFold(n_splits = num_folds)
	kf.get_n_splits(data_input)

	for train_index, valid_index in kf.split(data_input):
		train_input = data_input[train_index, :]
		train_output = data_output[train_index, :]

		valid_input = data_input[valid_index, :]
		valid_output = data_output[valid_index, :]

		# Train
		if(clf_type.lower() in ['adb', 'adaboost']):
			clf = AdaBoostClassifier(n_estimators=200, random_state=0)
		elif(clf_type.lower() in ['rf', 'randomforest']):
			clf = RandomForestClassifier(bootstrap=True, n_estimators=200, max_depth=10, random_state=0, class_weight = 'balanced')
		elif(clf_type.lower() in ['mlp']):
			clf = MLPClassifier(hidden_layer_sizes=(100, ), activation='tanh', solver='adam', batch_size='auto', learning_rate='constant', learning_rate_init=0.001, max_iter=400)
		else:
			clf = svm.SVC(decision_function_shape='ova', kernel = kernel, probability = False, class_weight = 'balanced')

		clf.fit(train_input, np.argmax(train_output, axis = 1 ))
		result['clf'].append(clf)

		# Train Predict
		pred_train_output = clf.predict(train_input)
		train_acc = float(np.sum(pred_train_output == np.argmax(train_output, axis = 1))) / pred_train_output.size
		result['train_acc'].append(train_acc)

		# Validation Predict
		pred_valid_output = clf.predict(valid_input)
		valid_acc = float(np.sum(pred_valid_output == np.argmax(valid_output, axis = 1))) / pred_valid_output.size
		result['valid_acc'].append(valid_acc)

	## Test Predict
	if(test_input is not None):
		best_clf_id = np.argmax(result['valid_acc'])
		clf = result['clf'][best_clf_id]
		pred_test_output = clf.predict(test_input)

		test_acc = float(np.sum(pred_test_output == np.argmax(test_output, axis = 1))) / pred_test_output.size
		result['top1'].append(test_acc)
		# print('Top-1 Test Acc - %.04f'%test_acc)

		conf_mat = confusion_matrix(np.argmax(test_output, axis = 1), pred_test_output)
		cname_list = []
		for idx in range(test_output.shape[1]):
			if(id_to_labels is not None):
				cname_list.append(id_to_labels[idx])
			else:
				cname_list.append(str(idx))
		plot_confusion_matrix(conf_mat, cname_list, normalize = True, title = 'Test Confusion Matrix', display = display, write_path = write_path)

	return result

# Calculates rotation matrix to euler angles
# The result is the same as MATLAB except the order
# of the euler angles ( x and z are swapped ).
def rotationMatrixToEulerAngles(R) :
	# assert(isRotationMatrix(R))
	sy = math.sqrt(R[0,0] * R[0,0] +  R[1,0] * R[1,0])
	singular = sy < 1e-6
	if  not singular :
		x = math.atan2(R[2,1] , R[2,2])
		y = math.atan2(-R[2,0], sy)
		z = math.atan2(R[1,0], R[0,0])
	else :
		x = math.atan2(-R[1,2], R[1,1])
		y = math.atan2(-R[2,0], sy)
		z = 0
	return z, y, -1*x
	# x, -1*y, z is kinda good but still random

def convert_all(RMat):
	'''
		RMat is a 2D np.ndarray. Each row is a flattened version of the rotation matrix.
	'''
	res = []
	for row in RMat:
		# print(len(row))
		# print(row)
		R = np.reshape(row, (3, 3))
		res.append(rotationMatrixToEulerAngles(R))
	return np.array(res)
