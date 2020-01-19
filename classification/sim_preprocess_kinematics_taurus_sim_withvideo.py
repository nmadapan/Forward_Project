
import time
import datetime
import numpy as np

def findclosesttimestamp(target, kin):
	timestamp = target # large number
	mindiff = target
	for key in kin:
		#print (key)
		diff = abs(target - key)
		if diff < mindiff: #update timestamp
			timestamp = key
			mindiff = diff
	return timestamp
def ts2frame(dir, ST, arm):
	fwc_left = open("data/taurus_kinematics_left.txt", "a") # append
	fwc_right = open("data/taurus_kinematics_right.txt", "a")
	
	fcolor_annot = dir+"/"+ST+"_color_annot.txt"
	fcolor_ts = dir+"/"+ST+"_color_ts.txt"
	fkin_arm = dir+"/"+ST+"_"+arm+"_kinematics.txt"
	#fkin_right = dir+"/"+ST+"_right_kinematics.txt"
	kinematics_data = {}
	with open(fkin_arm) as f:
		for line in f:
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			values = line.split(" ")
			timestamp = float(values[0])
			timestamp = round(timestamp, 2) #match with the video frame timestamp
			if timestamp not in kinematics_data:
				kinematics_data[timestamp] = line
	print (len(kinematics_data))
	frame2annotation = {}
	with open(fcolor_annot) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")

			print (line)
			sf, ef, surgeme, sucess = line.split(" ")
			sf = int(sf)
			ef = int(ef)
			for i in range(sf, ef+1):
				frame2annotation[i] = str(surgeme) + " " + str(sucess)
	# left arm
	fw = open(dir+"/"+ST+"_kinematics_"+arm+"_frame_ts.txt", "w")
	framecount = 0
	label = ""
	with open(fcolor_ts) as f:
		for line in f:
			framecount += 1
			if framecount in frame2annotation:
				label = frame2annotation[framecount]
				valid = "True"
			else: 
				label = "NA NA"
				valid = "False"

			line = line.replace("\n", "")
			line = line.replace("\r", "")
			timestamp = float(line)
			timestamp = round(timestamp, 2) #match with the video frame timestamp, in case it's not formated to two decimal
			# find closest timestamp
			timestamp = findclosesttimestamp(timestamp, kinematics_data)
			if timestamp in kinematics_data:
				frame = kinematics_data[timestamp]

				fw.write(ST+" "+frame + " " +str(valid)+ " " +label+ "\n")
				if arm == "left":
					fwc_left.write(ST+" "+frame + " " +str(valid)+ " " +label+ "\n")
				else:
					fwc_right.write(ST+" "+frame + " " +str(valid)+ " " +label+ "\n")
			else:
				print("Timestamp missing: "+ str(timestamp))
	fw.close()
	fwc_left.close()
	fwc_right.close()
	print ("Done")
def filterdata(dfile):
	unique_ts = []
	fw = open("filtered/"+"dfile"+"_filtered.txt", "w")
	with open(dfile) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")

			#print line
			parts = line.split(" ")
			ts = int(float(parts[1]))
			if ts not in unique_ts:
				fw.write(line+"\n")
				unique_ts.append(ts)
	fw.close()
def merge_left_right(fleft, fright):
	fw = open("data/taurus_kinematics.txt", "w")
	left = []
	right = []
	with open(fleft) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			left.append(line)
	with open(fright) as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			right.append(line)
	print (len(left))
	print (len(right))
	for i in range(0, len(left)):
		lelem = left[i]
		relem = right[i]
		lparts = lelem.split(" ")
		rparts = relem.split(" ")
		melem = ""
		if (lparts[0] == rparts[0]) and (lparts[1] == rparts[1]): # check trial id and timestamp for left and right arm
			for i in range(0, len(lparts)-3):
				melem = melem + " " + lparts[i]
			for i in range(2, len(rparts)): # with surgeme annotation
				melem = melem + " " + rparts[i] 
		else:
			print("Miss match - should not happen, check left right file carefully")
		melem = melem.strip()
		if melem != "":
			fw.write(melem+"\n")
		else:
			print("Miss match during writing- should not happen, check left right file carefully")
	fw.close()


# File: taurus_kinematics.txt
#     This file contains both left and right arms kinematic information as follows
#     Each line in this file has the following format: each value separated by space
#     // taskid ut l_rot_00 l_rot_01 l_rot_02 l_rot_10 l_rot_11 l_rot_12 l_rot_20 l_rot_21 l_rot_22 l_tr_x l_tr_y l_tr_z l_x l_y l_z l_gs r_rot_00 r_rot_01 r_rot_02 r_rot_10 r_rot_11 r_rot_12 r_rot_20 r_rot_21 r_rot_22 r_tr_x r_tr_y r_tr_z r_x r_y r_z r_gs valid surgeme_label success

#     Details:
#     1 = trial_id
#     2 = timestamp
#     3-21 = left arm kinematics
#     22-40 = right arm kinematics
#     41 = valid field; if True then valid and associated with the surgeme. If False then not associated with any surgeme task.
#     42 = surgeme_label => surgeme label (S1, S2 ...)
#     43 = success field; If True, surgeme completed successfully, if False, it's failed trial 

def process_for_svm():
	fwx = open("taurus_kinematics_feature_X.txt", "w")
	fwy = open("taurus_kinematics_feature_y.txt", "w")
	with open("taurus_kinematics.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			Xfeature = ""

			yfeature = str(parts[41]).replace("S","")# take only the integer number
			#print(parts[40])
			if parts[40] == "True": #valid data point
				for i in range(2, 40):
					Xfeature = Xfeature + " " + parts[i]
				Xfeature = Xfeature.strip()
				fwx.write(Xfeature+"\n")
				fwy.write(yfeature+"\n")
	fwx.close()
	fwy.close()
def process_for_video():
	fwx = open("data/video_taurus_sim_kinematics_feature_X.txt", "w")
	fwy = open("data/video_taurus_sim_kinematics_feature_y.txt", "w")
	count = 0
	# sample_rate = 1 # must be same as frame sampling in video
	prev_label = "0"

	with open("data/taurus_sim_kinematics.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			parts = line.split(" ")
			count += 1
			Xfeature = ""
			# print(len(parts))
			yfeature = str(parts[31]).replace("S","")# take only the integer number
			#print(parts[40])
			if prev_label != parts[0]: # new trial
				count = 1 
			prev_label = parts[0]
			if parts[30] == "True": #valid data point
				# if count % sample_rate == 0:
				for i in range(2, 30):
					Xfeature = Xfeature + " " + parts[i]
				Xfeature = Xfeature.strip()
				fwx.write(str(parts[0])+"_oscr_frame"+str(count)+".jpg" + " "+Xfeature+"\n")
				fwy.write(str(parts[0])+"_oscr_frame"+str(count)+".jpg" + " "+yfeature+"\n")
			
	fwx.close()
	fwy.close()
def merge_kinematic_video():
	fwx = open("/scratch/gilbreth/rahman64/dataset/surgeme/Taurus_sim/taurus_sim_kinematics_video_feature_all_X.txt", "w")
	fwy = open("/scratch/gilbreth/rahman64/dataset/surgeme/Taurus_sim/taurus_sim_kinematics_video_feature_all_y.txt", "w")
	count = 0
	# sample_rate = 20
	prev_label = "0"
	kindata = {}
	with open("data/video_taurus_sim_kinematics_feature_X.txt") as f:
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
	# print(kindata['S1_T1_color_frame4883.jpg'])
	# print(len(kindata))
	# print(len(kindata['S1_T1_color_frame4883.jpg'].split(" ")))
	frameids = []
	with open("/scratch/gilbreth/rahman64/dataset/surgeme/Taurus_sim/frameid_sorted_nodiff_all_1.txt") as f:
		for line in f:
			line = line.strip()
			line = line.replace("\n", "")
			line = line.replace("\r", "")
			frameid = (line.split("/")[-1]).strip()
			frameids.append(frameid)

	print("Loading video features")
	features = np.loadtxt("/scratch/gilbreth/rahman64/dataset/surgeme/Taurus_sim/features_sorted_nodiff_all_1.cnn", delimiter=' ')
	XT, yT = features[:, :-1], features[:, -1]
	print("Applying pca")

	from sklearn.decomposition import PCA
	pca = PCA(n_components=30)
	pca.fit(XT)
	# print(pca.explained_variance_ratio_)  
	# print(pca.singular_values_)  
	XT = pca.transform(XT) 
	print(len(XT), len(XT[0]))

	assert(len(frameids) == len(XT))
	for frameid in frameids:
		if frameid in kindata:
			kinfeature = kindata[frameid]
		else:
			print("Key not found: "+str(frameid))

		vdata = XT[frameids.index(frameid)]
		label = yT[frameids.index(frameid)]
		vfeature = ""
		for val in vdata.tolist():
			vfeature = vfeature + " " + str(val)
		vfeature = vfeature.strip()
		
		# print(len(kinfeature.split(" ")))

		fwx.write(kinfeature+" "+vfeature+"\n")
		fwy.write(str(label)+"\n")

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

def merge_unify_kin_video(video=True, kin=True):
	ignore_angles = False # True, False
	normalize = True

	A = np.loadtxt("/scratch/gilbreth/rahman64/dataset/surgeme/Taurus_sim/taurus_sim_kinematics_video_feature_all_X.txt", delimiter = ' ')
	A1 = A[:, :14] # add int for python 3?
	A2 = A[:, 14:28]
	A3 = A[:, 28:]
	print(A1.shape, A2.shape, A3.shape)
	if(ignore_angles):
		A1 = np.concatenate((A1[:, :3], np.reshape([A1[:, -1]], (-1, 1))), axis = 1)
		A2 = np.concatenate((A2[:, :3], np.reshape([A2[:, -1]], (-1, 1))), axis = 1)
	else:
		A1 = np.concatenate((A1[:, :6], np.reshape([A1[:, -1]], (-1, 1))), axis = 1)
		A2 = np.concatenate((A2[:, :6], np.reshape([A2[:, -1]], (-1, 1))), axis = 1)
	print(len(A[0]))
	print(A1.shape, A2.shape, A3.shape)
	if video == True and kin == True:
		A_new = np.concatenate((A1, A2, A3), axis = 1)
		print(A_new.shape)
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Taurus_sim/taurus_sim_kinematics_video_feature_all_X_unify.txt', A_new, delimiter=' ')
	elif video == False and kin == True:
		A_new = np.concatenate((A1, A2), axis = 1)
		print(A_new.shape)
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Taurus_sim/taurus_sim_kinematics_only_feature_all_X_unify.txt', A_new, delimiter=' ')
	elif video == True and kin == False:
		A_new = A3
		print(A_new.shape)
		np.savetxt('/scratch/gilbreth/rahman64/dataset/surgeme/Taurus_sim/taurus_sim_video_only_feature_all_X_unify.txt', A_new, delimiter=' ')
	
if __name__ == '__main__':
	# scount = 8
	# tcount = 6
	# for sc in range(1, scount+1):
	# 	for tc in range(1, tcount+1):
	# 		ts2frame("data/Taurus/S"+str(sc), "S"+str(sc)+"_T"+str(tc), "left")
	# 		ts2frame("data/Taurus/S"+str(sc), "S"+str(sc)+"_T"+str(tc), "right")
	# 		#filterdata("dataset/Taurus/S"+str(sc)+"/S"+str(sc)+"_T"+str(tc)+"_kinematics_left_frame_ts.txt")
	# merge_left_right("data/taurus_kinematics_left.txt", "data/taurus_kinematics_right.txt")
	# process_for_svm()
	# print (totimestamp("20:56:10.234"))
	# print (totimestamp("20:56:10.275"))
	# print (totimestamp("20:56:11.018"))
	
	# process kinematic and video data
	# process_for_video()
	merge_kinematic_video()
	# merge_unify_kin_video(video=True, kin=True)
	# merge_unify_kin_video(video=False, kin=True)
	# merge_unify_kin_video(video=True, kin=False)


