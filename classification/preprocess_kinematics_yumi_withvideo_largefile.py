
import time
import datetime
import numpy as np
import sys
import math

def process_for_video():
	fwx = open("data/video_yumi_kinematics_feature_X.txt", "w")
	fwy = open("data/video_yumi_kinematics_feature_y.txt", "w")
	count = 0
	
	prev_label = "0"
	with open("yumi_kinematics.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			count += 1
			Xfeature = ""
			yfeature = str(parts[41]).replace("S","")# take only the integer number
			# print(parts[39])
			# print(len(parts))
			if prev_label != parts[0]:
				count = 1 
			prev_label = parts[0]

			if parts[40] == "True": #and parts[42] == "True": #valid data point
				for i in range(2, 40):
					if i == 11: # take care of  inconsistency in the data collection process
						val = parts[i]
						if "-" in val:
							# print("minus")
							if val.startswith("-"):
								print ("- here at start should not print ")
							fs = str(val).split("-")

							if len(fs) == 2:
								val = val
								f1 = fs[0]
								f2 = "-"+fs[1]
								Xfeature = Xfeature + " " + f1 + " " + f2
							else:
								print("first feature also negative")
						else:
							print("not minus")
					elif i == 30: # take care of  inconsistency in the data collection process
						val = parts[i]
						if "-" in val:
							# print("minus")
							if val.startswith("-"):
								print ("- here at start should not print ")
							fs = str(val).split("-")

							if len(fs) == 2:
								val = val
								f1 = fs[0]
								f2 = "-"+fs[1]
								Xfeature = Xfeature + " " + f1 + " " + f2
							else:
								print("first feature also negative")
						else:
							print("not minus")
					else:
						Xfeature = Xfeature + " " + parts[i]
				
				Xfeature = Xfeature.strip()
				fwx.write(str(parts[0])+"_color_frame"+str(count)+".jpg" + " "+Xfeature+"\n")
				fwy.write(str(parts[0])+"_color_frame"+str(count)+".jpg" + " "+yfeature+"\n")

			
	fwx.close()
	fwy.close()

def merge_many_files(filenames, robot, filetype, ext):
	# filenames = ['file1.txt', 'file2.txt', ...]
	with open('/scratch/gilbreth/rahman64/dataset/surgeme/'+robot+'/'+filetype+'_sorted_nodiff_all_1'+ext, 'w') as outfile:
		for fname in filenames:
			with open(fname) as infile:
				for line in infile:
					outfile.write(line)

def get_features(filetype, ext, robot):
	# filenames = ['/scratch/gilbreth/rahman64/dataset/surgeme/'+robot+'/frameid_s3_5_nosorted_diff.txt']
	filenames = []
	count = 999
	# frameid_sorted_diff_21999.txt
	for i in range(51):
		file2 = '/scratch/gilbreth/rahman64/dataset/surgeme/'+robot+'/'+filetype+'_sorted_nodiff_'+str(count)+ext
		count += 1000
		print (file2)
		filenames.append(file2)
		# merge_two(file1, file2) 
	# last file leftover
	# file2 = '/scratch/gilbreth/rahman64/dataset/surgeme/'+robot+'/'+filetype+'_sorted_nodiff_'+str(22329)+ext
	filenames.append(file2) 
	print(filenames)
	merge_many_files(filenames)
def merge_kinematic_video():
	fwx = open("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_video_feature_1_X.txt", "w")
	fwy = open("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_video_feature_1_y.txt", "w")
	count = 0
	# sample_rate = 20
	prev_label = "0"
	kindata = {}
	with open("data/video_yumi_kinematics_feature_X.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			kf = ""
			for i in range(1, len(parts)):
				kf = kf +" "+ parts[i]
			kf = kf.strip()
			kindata[parts[0]] = kf
	
	# with open("data/video_yumi_kinematics_feature_y.txt") as f:
	# 	for line in f:
	# 		line = line.strip()
	# 		line = line.replace("\n", "")
	# 		line = line.replace("\r", "")
	# 		parts = line.split(" ")
			
			# yT_kin.append(int(float(parts[1])))
	# print(kindata['S1_T1_color_frame4883.jpg'])
	# print(len(kindata))
	# print(len(kindata['S1_T1_color_frame4883.jpg'].split(" ")))
	

	# using loadtxt
	# print("Loading video features")
	# features = np.loadtxt("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/features_sorted_nodiff_all_1.cnn", delimiter=' ')
	
	# optimized for large file
	# filepath = "/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/features_sorted_nodiff_all_1.cnn"
	# featurelist = []
	# with open(filepath) as f:
	# 	line = line.strip()
	# 	line = line.replace("\n", "")
	# 	line = line.replace("\r", "")
	# 	featurelist.append(line.split(" "))
	# 	if len(featurelist)%1000 == 0:
	# 		print("Completed: "+str(len(featurelist)))
	# print(len(featurelist), len(featurelist[0]))
	numparts = 70
	robot = "Yumi" # Taurus, Yumi, Taurus_sim
	filetype = "frameid" # features, frameid
	ext = ".txt"
	# features = np.asarray([]) # empty array
	# frame id
	filenames = []
	count = 999
	# frameid_sorted_diff_21999.txt
	for i in range(numparts):
		file2 = '/scratch/gilbreth/rahman64/dataset/surgeme/'+robot+'/'+filetype+'_sorted_nodiff_'+str(count)+ext
		count += 1000
		print (file2)
		filenames.append(file2)
		# merge_two(file1, file2) 
	# last file leftover
	# file2 = '/scratch/gilbreth/rahman64/dataset/surgeme/'+robot+'/'+filetype+'_sorted_nodiff_'+str(22329)+ext
	# filenames.append(file2) 
	print(filenames)
	merge_many_files(filenames, robot, filetype, ext)

	frameids = []
	with open("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/frameid_sorted_nodiff_all_1.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			frameid = (line.split("/")[-1]).strip()
			frameids.append(frameid)
	print(len(frameids))
	filetype = "features" # features, frameid
	if filetype == "features":
		ext = ".cnn"
	else:
		ext = ".txt"
	filenames = []
	count = 999

	file2 = '/scratch/gilbreth/rahman64/dataset/surgeme/'+robot+'/'+filetype+'_sorted_nodiff_'+str(count)+ext
	count += 1000
	print (file2)
	features = np.loadtxt(file2, delimiter=' ')

	for i in range(numparts-1):
		file2 = '/scratch/gilbreth/rahman64/dataset/surgeme/'+robot+'/'+filetype+'_sorted_nodiff_'+str(count)+ext
		count += 1000
		print (file2)
		parts = np.loadtxt(file2, delimiter=' ')
		features = np.concatenate((features, parts))
		print(len(features), len(features[0]))

	print("Spliting to X, y")
	# features = np.asarray(featurelist)
	XT, yT = features[:, :-1], features[:, -1]
	print("Applying pca")

	from sklearn.decomposition import PCA
	pca = PCA(n_components=30)
	pca.fit(XT) # need to do it only on train data to simulate that we are using training on pca approach. verify?
	# print(pca.explained_variance_ratio_)  
	# print(pca.singular_values_)  
	XT = pca.transform(XT) 
	print(len(XT), len(XT[0]))

	assert(len(frameids) == len(XT))
	
	for frameid in frameids:
		if frameid in kindata:
			kinfeature = kindata[frameid]
		# else:
		# 	print("Key not found: "+str(frameid))

			vdata = XT[frameids.index(frameid)]
			label = yT[frameids.index(frameid)]
			vfeature = ""
			for val in vdata.tolist():
				vfeature = vfeature + " " + str(val)
			vfeature = vfeature.strip()
			
			# print(len(kinfeature.split(" ")))

			fwx.write(kinfeature+" "+vfeature+"\n") 
			fwy.write(str(label)+"\n")
		else:
			print("Skipping, Key not found: "+str(frameid))
	# all kinematic
	# for frameid in kindata:
	# 	kinfeature = kindata[frameid]
	# 	label = yT_kin[kindata.index(frameid)]
	# 	if frameid in frameids:
	# 	# else:
	# 	# 	print("Key not found: "+str(frameid))
	# 		vdata = XT[frameids.index(frameid)]
	# 		# label = yT[frameids.index(frameid)]
	# 		vfeature = ""
	# 		for val in vdata.tolist():
	# 			vfeature = vfeature + " " + str(val)
	# 		vfeature = vfeature.strip()
			
	# 		# print(len(kinfeature.split(" ")))

	# 		fwx.write(kinfeature+" "+vfeature+"\n") 
	# 		fwy.write(str(label)+"\n")
	# 	else:
	# 		print("Skipping, Key not found, but adding 0 image feature: "+str(frameid))
	# 		fwx.write(kinfeature+" "+("0"*30).strip()+"\n") 
	# 		fwy.write(str(label)+"\n")


	fwx.close()
	fwy.close()
def totimestamp(timestr):
	timeformat = str(timestr).replace(".", ",")

	# print (time.mktime(datetime.datetime.strptime(s, "%d/%m/%Y").timetuple()))
	#1322697600.0
	# myDate = "2019-01-23 08:28:42,468"
	myDate = "2019-01-30" + " " + timeformat

	timestamp = time.mktime(datetime.datetime.strptime(myDate, "%Y-%m-%d %H:%M:%S,%f").timetuple())
	# print (timestamp)

	dt = datetime.datetime.strptime(myDate, "%Y-%m-%d %H:%M:%S,%f")
	tm = time.mktime(dt.timetuple()) + (dt.microsecond / 1000000.0)
	return tm
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
def merge_unify_kin_video(video=True, kin=True):
	ignore_angles = False # True, False
	normalize = False
	A = np.loadtxt("/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_video_feature_1_X.txt", delimiter = ' ')
	print(A.shape)
	
	A1 = A[:, :20] # add int for python 3?
	A2 = A[:, 20:40]
	A3 = A[:, 40:]
	print(A1.shape, A2.shape, A3.shape)
	print(A1[1], A2[1], A3[1])
	# sys.exit()
	# A1 = np.concatenate((A1[:, -7:-1], np.reshape([A1[:, -1]], (-1, 1))), axis = 1)
	# A2 = np.concatenate((A2[:, -7:-1], np.reshape([A2[:, -1]], (-1, 1))), axis = 1)
	T1 = A1[:, 7:10] # Get the translation of the left hand
	T1[:,-1] = -1 * T1[:, -1] # Switch the z axis because Taurus is left handed coordinate system.
	G1 = A1[:, -1].reshape(-1, 1) # Get the gripper status of left

	if(not ignore_angles):
		R1 = convert_all(A1[:, 10:-1]) # Get the rotation as 9 elements. convert 9 elements to 3 (yaw pitch and roll)
		A1 = np.concatenate((T1, R1, G1), axis = 1)
	else:
		A1 = np.concatenate((T1, G1), axis = 1)

	## Right hand
	T2 = A2[:, 7:10]
	T2[:, -1] = -1 * T2[:, -1]
	G2 = A2[:, -1].reshape(-1, 1)

	if(not ignore_angles):
		R2 = convert_all(A2[:, 10:-1])
		A2 = np.concatenate((T2, R2, G2), axis = 1)
	else:
		A2 = np.concatenate((T2, G2), axis = 1)


	print(len(A[0]))
	print(A1.shape, A2.shape, A3.shape)
	print(A1[1], A2[1], A3[1])
	if video == True and kin == True:
		A_new = np.concatenate((A1, A2, A3), axis = 1)
		print(A_new.shape)
		if(normalize):
			A_new = (A_new - np.min(A_new, axis = 0))/(np.max(A_new, axis = 0)-np.min(A_new, axis = 0))
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_video_feature_1_X_unify.txt', A_new, delimiter=' ')
	elif video == False and kin == True:
		A_new = np.concatenate((A1, A2), axis = 1)
		print(A_new.shape)
		if(normalize):
			A_new = (A_new - np.min(A_new, axis = 0))/(np.max(A_new, axis = 0)-np.min(A_new, axis = 0))
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_kinematics_only_feature_1_X_unify.txt', A_new, delimiter=' ')
	elif video == True and kin == False:
		A_new = A3
		print(A_new.shape)
		if(normalize):
			A_new = (A_new - np.min(A_new, axis = 0))/(np.max(A_new, axis = 0)-np.min(A_new, axis = 0))
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Yumi/yumi_video_only_feature_1_X_unify.txt', A_new, delimiter=' ')

if __name__ == '__main__':
	# scount = 8
	# tcount = 6
	# for sc in range(3, scount+1):
	# 	for tc in range(1, tcount+1):
	# 		ts2frame("data/Yumi/S"+str(sc), "S"+str(sc)+"_T"+str(tc), "left")
	# 		ts2frame("data/Yumi/S"+str(sc), "S"+str(sc)+"_T"+str(tc), "right")
	# 		#filterdata("dataset/Taurus/S"+str(sc)+"/S"+str(sc)+"_T"+str(tc)+"_kinematics_left_frame_ts.txt")
	# merge_left_right("yumi_kinematics_left.txt", "yumi_kinematics_right.txt")
	# process_for_sk()
	# print (totimestamp("20:56:10.234"))
	# print (totimestamp("20:56:10.275"))
	# print (totimestamp("20:56:11.018"))
	# merge_kinematic_visual_location()
	# process_kin_loc_for_sk()
	# merge_only_visual_location()
	# process_for_sk()
	# process_kin_loc_for_sk()
	
	# **** kinematic and video ****
	# process_for_video()
	merge_kinematic_video()
	# merge_unify_kin_video(video=True, kin=True)
	# merge_unify_kin_video(video=True, kin=False)
	# merge_unify_kin_video(video=False, kin=True)



