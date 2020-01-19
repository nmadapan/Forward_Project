import numpy as np

def merge_labels(gt_pred, smoothed_label):
	gt_list = []
	pred_list = []
	sm_list = []
	with open(gt_pred) as f:
		for line in f:
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			gt,pred = line.split("\t")
			gt_list.append(gt)
			pred_list.append(pred)

	with open(smoothed_label) as f:
		for line in f:
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			line = line.strip()
			sm_list.append(line)
	return gt_list, pred_list, sm_list
def split_surgeme(gt_list, pred_list, sm_list):
	prev_label = 0
	gt_parts = []
	gt_temp = []
	pred_parts = []
	pred_temp = []
	sm_parts = []
	sm_temp = []
	# print (gt_list)
	for i in range(1, len(gt_list)):
		# if gt_list[i] == "1.0" and gt_list[i-1] == "7.0": #transition
		current_label = int(float(gt_list[i]))
		prev_label = int(float(gt_list[i-1]))
		# print(current_label, prev_label)
		if current_label == 1 and prev_label == 7: #transition
			gt_parts.append(gt_temp)
			pred_parts.append(pred_temp)
			sm_parts.append(sm_temp)
			gt_temp = []
			pred_temp = []
			sm_temp = []
		gt_temp.append(gt_list[i])
		pred_temp.append(pred_list[i])
		sm_temp.append(sm_list[i])
	print(len(gt_parts), len(pred_parts), len(sm_parts))
	return gt_parts, pred_parts, sm_parts
def split_surgeme_persurgeme(gt_list, pred_list, sm_list):
	prev_label = 0
	gt_parts = []
	gt_temp = []
	pred_parts = []
	pred_temp = []
	sm_parts = []
	sm_temp = []
	# print (gt_list)
	for i in range(1, len(gt_list)):
		# if gt_list[i] == "1.0" and gt_list[i-1] == "7.0": #transition
		current_label = int(float(gt_list[i]))
		prev_label = int(float(gt_list[i-1]))
		# print(current_label, prev_label)
		# if current_label == 1 and prev_label == 7: #transition
		if current_label != prev_label: #transition
			gt_parts.append(gt_temp)
			pred_parts.append(pred_temp)
			sm_parts.append(sm_temp)
			gt_temp = []
			pred_temp = []
			sm_temp = []

		gt_temp.append(gt_list[i])
		pred_temp.append(pred_list[i])
		sm_temp.append(sm_list[i])
	print(len(gt_parts), len(pred_parts), len(sm_parts))
	return gt_parts, pred_parts, sm_parts

def eval():

	gt = []
	pred = []
	predictionfile = "rf_gt_pred.txt"
	gt_seq = []
	pred_seq = []
	with open (predictionfile) as f:
		prev_gt_label = 0.0
		prev_pred_label = 0.0
		for line in f:
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			s1,s2 = line.split("\t")
			s1 = float(s1)
			s2 = float(s2)
			if s1 != prev_gt_label:
				gt_seq.append(s1)
				prev_gt_label = s1
			if s2 != prev_pred_label:
				pred_seq.append(s2)
				prev_pred_label = s2
	
			gt.append(s1)
			pred.append(s2)
	# print(len(gt_seq))
	# print(len(pred_seq))

	# print("GT-vs-GT")
	# ld = levenshtein(gt_seq, gt_seq)
	# print(ld)

	# print("GT-vs-PRED")
	ld = levenshtein(gt_seq, pred_seq)
	print(ld)
	# string1 = ""
	# string2 = ""

	# for label in gt_seq:
	# 	string1 = string1 + str

	# print(len(gt))
	# print(len(pred))
	correct = 0
	incorrect = 0
	for i in range(0, len(gt)):
		if gt[i] == pred[i]:
			correct += 1
		else:
			incorrect += 1
	acc = (correct * 1.0)/(correct + incorrect)
	return (acc, correct, incorrect,ld)

def levenshtein(seq1, seq2):  
    size_x = len(seq1) + 1
    size_y = len(seq2) + 1
    matrix = np.zeros ((size_x, size_y))
    for x in range(size_x):
        matrix [x, 0] = x
    for y in range(size_y):
        matrix [0, y] = y

    for x in range(1, size_x):
        for y in range(1, size_y):
            if seq1[x-1] == seq2[y-1]:
                matrix [x,y] = min(
                    matrix[x-1, y] + 1,
                    matrix[x-1, y-1],
                    matrix[x, y-1] + 1
                )
            else:
                matrix [x,y] = min(
                    matrix[x-1,y] + 1,
                    matrix[x-1,y-1] + 1,
                    matrix[x,y-1] + 1
                )
    # print (matrix)
    return (matrix[size_x - 1, size_y - 1])
def compress_label(list1):
	compressed_list = []
	compressed_list.append(list1[0])
	for i in range(1, len(list1)):
		if list1[i] != list1[i-1]:
			compressed_list.append(list1[i])
	return compressed_list
def eval_ld(gt_list, pred_list, sm_list):
	pred_ld = levenshtein(gt_list, pred_list)
	sm_ld = levenshtein(gt_list, sm_list)
	return pred_ld, sm_ld

if __name__ == '__main__':
	# print (eval())
	gt_list, pred_list, sm_list = merge_labels("rf_gt_pred_fold5.txt", "rf_gt_pred_fold5_smoothed_label.txt")
	# gt_list, pred_list, sm_list = merge_labels("perframe_prediction_taurus_epoch_10.txt", "perframe_prediction_taurus_epoch_10_smoothed_label.txt")
	
	# cl_gt_list = compress_label(gt_list)
	# cl_pred_list = compress_label(pred_list)
	# cl_sm_list = compress_label(sm_list)
	# print(cl)
	# gt_parts, pred_parts, sm_parts = split_surgeme(gt_list, pred_list, sm_list)
	gt_parts, pred_parts, sm_parts = split_surgeme_persurgeme(gt_list, pred_list, sm_list)

	total_pred_ld = 0
	total_smoothed_pred_ld = 0
	total_pred_ld_norm = 0
	total_smoothed_pred_ld_norm = 0
	for i in range(len(gt_parts)):
		# ncompress
		cl_gt_list = compress_label(gt_parts[i])
		cl_pred_list = compress_label(pred_parts[i])
		cl_sm_list = compress_label(sm_parts[i])

		# non-compress
		# cl_gt_list = gt_parts[i]
		# cl_pred_list = pred_parts[i]
		# cl_sm_list = sm_parts[i]

		# print(cl_gt_list)
		# print (str(i)+" "+str(eval_ld(gt_parts[i], pred_parts[i], sm_parts[i])))
		print (str(i)+" "+str(eval_ld(cl_gt_list, cl_pred_list, cl_sm_list)))
		# print(cl_gt_list)
		# print(cl_pred_list)
		# print(cl_sm_list)

		# print(len(cl_gt_list))
		# print(len(cl_pred_list))
		# print(len(cl_sm_list))
		pred_ld, smoothed_pred_ld = eval_ld(cl_gt_list, cl_pred_list, cl_sm_list)
		
		total_pred_ld_norm += pred_ld / len(cl_gt_list)
		total_smoothed_pred_ld_norm += smoothed_pred_ld / len(cl_gt_list)
		print(pred_ld, pred_ld / len(cl_gt_list), len(cl_gt_list), len(cl_pred_list), len(cl_sm_list))

		total_pred_ld += pred_ld
		total_smoothed_pred_ld += smoothed_pred_ld

	avg_pred_norm = total_pred_ld_norm / len(gt_parts)
	avg_smoothed_pred_norm = total_smoothed_pred_ld_norm / len(gt_parts)

	avg_pred = total_pred_ld/len(gt_parts)
	avg_smoothed_pred = total_smoothed_pred_ld/len(gt_parts)
	print("Test tiral= "+str(len(gt_parts)))
	
	print(avg_pred, avg_smoothed_pred)
	print(avg_pred_norm, avg_smoothed_pred_norm)
	# eval using surgeme only, not on frame


