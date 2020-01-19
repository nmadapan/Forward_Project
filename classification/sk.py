
import numpy
import sys
import os

def allFiles(directory, ext):
    all_files = []
    for root, dirs, files in os.walk(directory):
        for filename in files:
            fname = filename.lower()
            if fname.endswith(ext):
                full_fname = os.path.join(root, filename)
                all_files.append(full_fname)
    return all_files
 
def data_process():
	fwx = open("suturing/experiment/Suturing_X.txt", "w")
	fwy = open("suturing/experiment/Suturing_y.txt", "w")
	kindata = allFiles("suturing/kinematics/AllGestures", ".txt")
	count = 0
	for file in kindata:
		filename = str(file).split("/")[-1]

		with open("suturing/kinematics/AllGestures/"+filename) as f:
			
			kinematic_data = []
			for line in f:
				line = line.replace("\n", "")
				line = line.replace("\r", "")
				line = line.strip()
				parts = line.split(" ")
				newline = ""
				for elem in parts:
					if elem != "":
						newline = newline + " " + elem
				newline = newline.strip()
				#print(len(newline.split(" ")))
				
				kinematic_data.append(newline)
		with open("suturing/transcriptions/"+filename) as f:
			for line in f:
				line = line.replace("\n", "")
				line = line.replace("\r", "")
				line = line.strip()
				start, end, ges = line.split(" ")
				label = int(ges.replace("G",""))
				start = int(start)
				end = int(end)
				for i in range(start,end+1):
					kdata = kinematic_data[i-1]
					fwx.write(kdata+"\n")
					fwy.write(str(label)+"\n")
		count += 1
		print("File read = "+str(count))
	fwx.close()
	fwy.close()
	
def svm_model():
	#X = [[0, 0], [1, 1], [2, 2]]
	#y = [0, 1, 2]
	# test
	# with open("data/taurus_kinematics_feature_X.txt") as f:
	# 	for line in f:
	# 		parts = line.split(" ")
	# 		print (line)
	# 		for p in parts:
	# 			print (p)
	# 			fp = float(p)
	# 			print(p, fp)
				
	# XT = numpy.loadtxt("data/taurus_kinematics_feature_X.txt", delimiter=" ")
	# yT = numpy.loadtxt("data/taurus_kinematics_feature_y.txt")

	XT = numpy.loadtxt("data/taurus_sim_kinematics_feature_X.txt", delimiter=" ")
	yT = numpy.loadtxt("data/taurus_sim_kinematics_feature_y.txt")

	print("Actual data points:"+str(len(yT)))
	countExp = 30000
	X = XT[0:countExp:1]
	y = yT[0:countExp:1]

	print("Experiment data points:"+str(len(y)))

	ntrain = int(len(X) * 0.8)
	print (ntrain)
	ntest = len(X) - ntrain
	print (ntest)

	Xtrain = X[0:ntrain:1]
	ytrain = y[0:ntrain:1]

	Xtest = X[ntrain+1:]
	ytest = y[ntrain+1:]

	#sys.exit()
	model = "SVM"
	print("svm.SVC(C=10,gamma='scale', kernel='poly')")
	clf = svm.SVC(C=10,gamma='scale', kernel='poly')
	m = clf.fit(Xtrain, ytrain)  
	print (m)
	pred = clf.predict(Xtest)
	# print ("Prediction")
	# print (pred)
	# print ("Ground Truth")
	# print(ytest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	accuracy = round((correct*1.0)/len(pred), 2)
	print (str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	
	print("Correct: "+ str(correct))
	print("Not-correct: "+ str(len(pred) - correct))
	print("Accuracy: "+ str((correct*1.0)/len(pred)))
	
	# get support vectors
	# print clf.support_vectors_


	# get indices of support vectors
	# print clf.support_ 

	# get number of support vectors for each class
	# print clf.n_support_ 

	# print ("Multi-class")
	# X = [[0], [1], [2], [3]]
	# Y = [0, 1, 2, 3]
	# clf = svm.SVC(gamma='scale', decision_function_shape='ovo')
	# clf.fit(X, Y) 

	# pred = clf.predict([[2.4]])
	# print (pred)


	# dec = clf.decision_function([[1]])
	# print dec.shape[1] # 4 classes: 4*3/2 = 6

	# clf.decision_function_shape = "ovr"
	# dec = clf.decision_function([[1]])
	# print dec.shape[1] # 4 classes

	# print ("\n LinearSVC")
	# lin_clf = svm.LinearSVC()
	# lin_clf.fit(X, Y) 

	# pred = clf.predict([[2.4]])
	# print (pred)



	# dec = lin_clf.decision_function([[1]])
	# print dec.shape[1]
def svm_model_temporal_gt():
	#X = [[0, 0], [1, 1], [2, 2]]
	#y = [0, 1, 2]
	# XT = numpy.loadtxt("data/taurus_kinematics_feature_X.txt", delimiter=" ")
	# yT = numpy.loadtxt("data/taurus_kinematics_feature_y.txt")

	XT = numpy.loadtxt("data/taurus_sim_kinematics_feature_X.txt", delimiter=" ")
	yT = numpy.loadtxt("data/taurus_sim_kinematics_feature_y.txt")

	print("Actual data points: "+str(len(yT)))
	countExp = 30000
	X = XT[0:countExp:1]
	y = yT[0:countExp:1]

	print("Experiment data points:"+str(len(y)))

	ntrain = int(len(X) * 0.8)
	print (ntrain)
	ntest = len(X) - ntrain
	print (ntest)

	Xtrain = X[0:ntrain:1]
	ytrain = y[0:ntrain:1]

	Xtrain_t = []
	for i in range(0, len(Xtrain)):
		if (i == 0) or (i == (len(Xtrain)-1)): # defualt
			xelem = Xtrain[i]
			xelem = numpy.append(xelem, [0])
		else: # last element
			xelem = Xtrain[i]
			xelem = numpy.append(xelem, [ytrain[i-1]])
		Xtrain_t.append(xelem)
	print("Train normal array")
	print(len(Xtrain))
	print(len(Xtrain[0]))
	print("Train augmented array")
	print(len(Xtrain_t))
	print(len(Xtrain_t[0]))

	Xtest_t = []
	Xtest = X[ntrain+1:]
	ytest = y[ntrain+1:]

	Xtest_t = []
	for i in range(0, len(Xtest)):
		if (i == 0) or (i == (len(Xtest)-1)): # defualt
			xelem = Xtest[i]
			xelem = numpy.append(xelem, [0])
		else: # last element
			xelem = Xtest[i]
			xelem = numpy.append(xelem, [ytest[i-1]])
		Xtest_t.append(xelem)
	print("Test normal array")
	print(len(Xtest))
	print(len(Xtest[0]))
	print("Test augmented array")
	print(len(Xtest_t))
	print(len(Xtest_t[0]))
	
	#sys.exit()
	clf = svm.SVC(gamma='scale')
	clf.fit(Xtrain_t, ytrain)  
	pred = clf.predict(Xtest_t)
	print ("Prediction")
	print (pred)
	print ("Ground Truth")
	print(ytest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	print("Correct: "+ str(correct))
	print("Not correct: "+ str(len(pred) - correct))
	print("Accuracy: "+ str((correct*1.0)/len(pred)))

def svm_model_temporal_predicted():
	print ("svm_model_temporal_predicted")
	#X = [[0, 0], [1, 1], [2, 2]]
	#y = [0, 1, 2]
	# XT = numpy.loadtxt("data/taurus_kinematics_feature_X.txt", delimiter=" ")
	# yT = numpy.loadtxt("data/taurus_kinematics_feature_y.txt")

	XT = numpy.loadtxt("data/taurus_sim_kinematics_feature_X.txt", delimiter=" ")
	yT = numpy.loadtxt("data/taurus_sim_kinematics_feature_y.txt")

	print("Actual data points: "+str(len(yT)))
	countExp = 30000
	X = XT[0:countExp:1]
	y = yT[0:countExp:1]

	print("Experiment data points:"+str(len(y)))

	perc = 0.8
	print ("Split: "+str(perc))
	ntrain = int(len(X) * perc)
	print (ntrain)
	ntest = len(X) - ntrain
	print (ntest)

	Xtrain = X[0:ntrain:1]
	ytrain = y[0:ntrain:1]

	Xtrain_t = []
	for i in range(0, len(Xtrain)):
		if (i == 0) or (i == (len(Xtrain)-1)): # defualt
			xelem = Xtrain[i]
			xelem = numpy.append(xelem, [0])
		else: # last element
			xelem = Xtrain[i]
			xelem = numpy.append(xelem, [ytrain[i-1]])
		Xtrain_t.append(xelem)
	print("Train normal array")
	print(len(Xtrain))
	print(len(Xtrain[0]))
	print("Train augmented array")
	print(len(Xtrain_t))
	print(len(Xtrain_t[0]))

	Xtest_t = []
	Xtest = X[ntrain+1:]
	ytest = y[ntrain+1:]

	# Xtest_t = []
	# for i in range(0, len(Xtest)):
	# 	if (i == 0) or (i == (len(Xtest)-1)): # defualt
	# 		xelem = Xtest[i]
	# 		xelem = numpy.append(xelem, [0])
	# 	else: # last element
	# 		xelem = Xtest[i]
	# 		xelem = numpy.append(xelem, [ytest[i-1]])
	# 	Xtest_t.append(xelem)
	# print("Test normal array")
	# print(len(Xtest))
	# print(len(Xtest[0]))
	# print("Test augmented array")
	# print(len(Xtest_t))
	# print(len(Xtest_t[0]))
	
	#sys.exit()
	clf = svm.SVC(gamma='scale')
	clf.fit(Xtrain_t, ytrain)  
	
	#pred = clf.predict(Xtest_t)
	pred = []
	for i in range(0, len(Xtest)):
		if (i == 0) or (i == (len(Xtest)-1)): # start and end
			xelem = Xtest[i]
			#xelem = numpy.append(xelem, [0])
			xelem = numpy.append(xelem, ytest[0])
			pred_elem = clf.predict([xelem])
		else: # last element
			xelem = Xtest[i]
			if i%20 == 0: # delay
				xelem = numpy.append(xelem, [ytest[i-1]])
			else:
				xelem = numpy.append(xelem, [pred[i-1]]) # previous prediction
			pred_elem = clf.predict([xelem])
		pred.append(pred_elem[0]) # first and one element
	print("Prediction len")
	print(len(pred))
	print(pred[0])

	# print ("Prediction")
	# print (pred)
	# print ("Ground Truth")
	# print(ytest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		# print gtlabel
		# print predlabel
		if predlabel == gtlabel:
			correct += 1
			# print("Correct: "+ str(correct))
		# else:
		# 	print("In Correct")
	print("Correct: "+ str(correct))
	print("In-correct: "+ str(len(pred) - correct))
	print("Accuracy: "+ str((correct*1.0)/len(pred)))

# dataset = "Taurus_sim"
	# XT = numpy.loadtxt("data/taurus_sim_kinematics_feature_X.txt", delimiter=" ")
	# yT = numpy.loadtxt("data/taurus_sim_kinematics_feature_y.txt")
	

def RandomForestClassifier_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn.ensemble import RandomForestClassifier
	model = "RandomForestClassifier"
	clf = RandomForestClassifier(bootstrap=True, n_estimators=200, max_depth=10, random_state=0)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")

	predscore_train = clf.score(Xtrain, ytrain)
	predscore_test = clf.score(Xtest, ytest)
	pred = clf.predict(Xtest)
	fw = open("rf_gt_pred_fold5.txt", "w")
	for i in range(len(ytest)):
		fw.write(str(ytest[i]) + "\t" + str(pred[i])+ "\n")
	fw.close()
	return (predscore_train, predscore_test, model, m)
def AdaBoostClassifier_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn.ensemble import AdaBoostClassifier
	model = "AdaBoostClassifier"
	clf = AdaBoostClassifier(n_estimators=200, random_state=0)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")

	predscore_train = clf.score(Xtrain, ytrain)
	predscore_test = clf.score(Xtest, ytest)
	return (predscore_train, predscore_test, model, m)
def GradientBoostingClassifier_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn.ensemble import GradientBoostingClassifier
	model = "GradientBoostingClassifier"
	clf = GradientBoostingClassifier(n_estimators=200, random_state=0)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")

	predscore_train = clf.score(Xtrain, ytrain)
	predscore_test = clf.score(Xtest, ytest)
	return (predscore_train, predscore_test, model, m)
def DecisionTreeClassifier_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn.tree import DecisionTreeClassifier
	model = "DecisionTreeClassifier"
	clf = DecisionTreeClassifier(max_depth=5, random_state=0)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")

	predscore_train = clf.score(Xtrain, ytrain)
	predscore_test = clf.score(Xtest, ytest)
	return (predscore_train, predscore_test, model, m)
def GaussianNB_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn.naive_bayes import GaussianNB
	model = "GaussianNB"
	clf = GaussianNB()

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")

	predscore_train = clf.score(Xtrain, ytrain)
	predscore_test = clf.score(Xtest, ytest)
	pred = clf.predict(Xtest)
	fw = open("GaussianNB_gt_pred.txt", "w")
	for i in range(len(ytest)):
		fw.write(str(ytest[i]) + "\t" + str(pred[i])+ "\n")
	fw.close()
	return (predscore_train, predscore_test, model, m)
def KNeighborsClassifier_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn.neighbors import KNeighborsClassifier
	model = "KNeighborsClassifier"
	clf = KNeighborsClassifier(n_neighbors=3)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")

	predscore_train = clf.score(Xtrain, ytrain)
	predscore_test = clf.score(Xtest, ytest)
	return (predscore_train, predscore_test, model, m)
def MLPClassifier_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn.neural_network import MLPClassifier
	model = "MLPClassifier"
	# activation : {‘identity’, ‘logistic’, ‘tanh’, ‘relu’}, default ‘relu’
	clf = MLPClassifier(hidden_layer_sizes=(100, ), activation='tanh', solver='adam', batch_size='auto', learning_rate='constant', learning_rate_init=0.001, max_iter=200)
	
	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")

	predscore_train = clf.score(Xtrain, ytrain)
	predscore_test = clf.score(Xtest, ytest)
	return (predscore_train, predscore_test, model, m)
def SVM_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn import svm
	model = "SVM"
	clf = svm.SVC(C=10,gamma='scale', kernel='poly')
	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")

	predscore_train = clf.score(Xtrain, ytrain)
	predscore_test = clf.score(Xtest, ytest)
	pred = clf.predict(Xtest)
	# fw = open("rf_gt_pred.txt", "w")
	# for i in range(len(ytest)):
	# 	fw.write(str(ytest[i]) + "\t" + str(pred[i])+ "\n")
	# fw.close()
	return (predscore_train, predscore_test, model, m)

def sk_model(Xtrain, ytrain, Xtest, ytest):
	from sklearn.ensemble import RandomForestClassifier
	model = "RandomForestClassifier"
	clf = RandomForestClassifier(bootstrap=True, n_estimators=200, max_depth=10, random_state=0)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")
	# print (m)
	# print(clf.feature_importances_)
	pred = clf.predict(Xtest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	accuracy = (round((correct*1.0)/len(pred), 2))*100
	print (str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	fw = open("skmodel_results_5fold.txt", "a")
	fw.write(str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy) + "\t" + str(m) +"\n")
	fw.close()

	from sklearn.ensemble import AdaBoostClassifier
	model = "AdaBoostClassifier"
	clf = AdaBoostClassifier(n_estimators=200, random_state=0)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")
	# print (m)
	# print(clf.feature_importances_)
	pred = clf.predict(Xtest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	accuracy = (round((correct*1.0)/len(pred), 2))*100
	print (str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	fw = open("skmodel_results.txt", "a")
	fw.write(str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy) + "\t" + str(m) +"\n")
	fw.close()

	from sklearn.ensemble import GradientBoostingClassifier
	model = "GradientBoostingClassifier"
	clf = GradientBoostingClassifier(n_estimators=200, random_state=0)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")
	# print (m)
	# print(clf.feature_importances_)
	pred = clf.predict(Xtest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	accuracy = (round((correct*1.0)/len(pred), 2))*100
	print (str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	fw = open("skmodel_results.txt", "a")
	fw.write(str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy) + "\t" + str(m) +"\n")
	fw.close()

	from sklearn.tree import DecisionTreeClassifier
	model = "DecisionTreeClassifier"
	clf = DecisionTreeClassifier(max_depth=5, random_state=0)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")
	# print (m)
	# print(clf.feature_importances_)
	pred = clf.predict(Xtest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	accuracy = (round((correct*1.0)/len(pred), 2))*100
	print (str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	fw = open("skmodel_results.txt", "a")
	fw.write(str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy) + "\t" + str(m) +"\n")
	fw.close()

	from sklearn.naive_bayes import GaussianNB
	model = "GaussianNB"
	clf = GaussianNB()

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")
	# print (m)
	# print(clf.feature_importances_)
	pred = clf.predict(Xtest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	accuracy = (round((correct*1.0)/len(pred), 2))*100
	print (str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	fw = open("skmodel_results.txt", "a")
	fw.write(str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy) + "\t" + str(m) +"\n")
	fw.close()

	from sklearn.neighbors import KNeighborsClassifier
	model = "KNeighborsClassifier"
	clf = KNeighborsClassifier(n_neighbors=3)

	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")
	# print (m)
	# print(clf.feature_importances_)
	pred = clf.predict(Xtest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	accuracy = (round((correct*1.0)/len(pred), 2))*100
	print (str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	fw = open("skmodel_results.txt", "a")
	fw.write(str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy) + "\t" + str(m) +"\n")
	fw.close()


	from sklearn.neural_network import MLPClassifier
	model = "MLPClassifier"
	# activation : {‘identity’, ‘logistic’, ‘tanh’, ‘relu’}, default ‘relu’
	clf = MLPClassifier(hidden_layer_sizes=(100, ), activation='tanh', solver='adam', batch_size='auto', learning_rate='constant', learning_rate_init=0.001, max_iter=200)
	
	m = clf.fit(Xtrain, ytrain)  
	m = str(m).replace("\n", " ").replace("\r", " ")
	# print (m)
	# print(clf.feature_importances_)
	pred = clf.predict(Xtest)
	correct = 0
	for i in range(0, len(pred)):
		predlabel = pred[i]
		gtlabel = ytest[i]
		if predlabel == gtlabel:
			correct += 1
	accuracy = (round((correct*1.0)/len(pred), 2))*100
	print (str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	fw = open("skmodel_results.txt", "a")
	fw.write(str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy) + "\t" + str(m) +"\n")
	fw.close()

	


	
	# from sklearn import svm
	# model = "SVM"
	# clf = svm.SVC(C=10,gamma='scale', kernel='poly')

	# m = clf.fit(Xtrain, ytrain)  
	# m = str(m).replace("\n", " ").replace("\r", " ")
	# # print (m)
	# # print(clf.feature_importances_)
	# pred = clf.predict(Xtest)
	# correct = 0
	# for i in range(0, len(pred)):
	# 	predlabel = pred[i]
	# 	gtlabel = ytest[i]
	# 	if predlabel == gtlabel:
	# 		correct += 1
	# accuracy = (round((correct*1.0)/len(pred), 2))*100
	# print (str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy))
	# fw = open("skmodel_results.txt", "a")
	# fw.write(str(dataset) + "\t" + str(model) + "\t" + str(len(yT)) + "\t" + str(len(y)) + "\t" + str(ntrain) + "\t" + str(ntest) + "\t" + str(accuracy) + "\t" + str(m) +"\n")
	# fw.close()
	
	# sys.exit()

# dataset = "Taurus_sim"
# XT = numpy.loadtxt("data/taurus_sim_kinematics_feature_X.txt", delimiter=" ")
# yT = numpy.loadtxt("data/taurus_sim_kinematics_feature_y.txt")

dataset = "Taurus"
XT = numpy.loadtxt("data/taurus_kinematics_feature_x.txt", delimiter=" ")
yT = numpy.loadtxt("data/taurus_kinematics_feature_y.txt")

# dataset = "Yumi"
# XT = numpy.loadtxt("data/yumi_kinematics_feature_x.txt", delimiter=" ")
# yT = numpy.loadtxt("data/yumi_kinematics_feature_y.txt")


# dataset = "Taurus_onlysuccess"
# XT = numpy.loadtxt("data/taurus_kinematics_onlysuccess_feature_X.txt", delimiter=" ")
# yT = numpy.loadtxt("data/taurus_kinematics_onlysuccess_feature_y.txt")

# dataset = "Taurus_onlylocation"
# XT = numpy.loadtxt("data/taurus_only_location_feature_X.txt", delimiter=" ")
# yT = numpy.loadtxt("data/taurus_only_location_feature_y.txt")


# dataset = "Taurus_kin_loc"
# XT = numpy.loadtxt("data/taurus_kinematics_location_feature_X.txt", delimiter=" ")
# yT = numpy.loadtxt("data/taurus_kinematics_location_feature_y.txt")


# feature scaling individual feature normal distribution
from sklearn.preprocessing import scale
XT = scale( XT, axis=0, with_mean=True, with_std=True, copy=True )
# filename = "skmodel_results_5fold_train_test_taurus_onlysuccess.txt"
filename = "skmodel_results_5fold_train_test_yumi_kinematics.txt"

from sklearn.decomposition import PCA
pca = PCA(n_components=8)
pca.fit(XT)
print(pca.explained_variance_ratio_)  
print(pca.singular_values_)  
XT = pca.transform(XT) 

if __name__ == '__main__':
	from sklearn.model_selection import KFold
	kf = KFold(n_splits=5)
	kf.get_n_splits(XT)

	print(kf)  
	i = 0
	for train_index, test_index in kf.split(XT):
		i += 1
		print("TRAIN:", train_index, "TEST:", test_index)
		X_train, X_test = XT[train_index], XT[test_index]
		y_train, y_test = yT[train_index], yT[test_index]
		print("Fold: "+str(i))
		# if i > 1:
		# 	break
		if i == 1:
			predscore_train, predscore_test, model, m = KNeighborsClassifier_model(X_train, y_train, X_test, y_test)
			print(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
			fw = open(filename, "a")
			fw.write(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
			fw.close()
		# predscore_train, predscore_test, model, m = RandomForestClassifier_model(X_train, y_train, X_test, y_test)
		# print(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
		# fw = open(filename, "a")
		# fw.write(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
		# fw.close()


	   # predscore_train, predscore_test, model, m = SVM_model(X_train, y_train, X_test, y_test)
	   # print(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw = open(filename, "a")
	   # fw.write(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw.close()

	   # predscore_train, predscore_test, model, m = AdaBoostClassifier_model(X_train, y_train, X_test, y_test)
	   # print(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw = open(filename, "a")
	   # fw.write(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw.close()

	   # predscore_train, predscore_test, model, m = DecisionTreeClassifier_model(X_train, y_train, X_test, y_test)
	   # print(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw = open(filename, "a")
	   # fw.write(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw.close()

	   # predscore_train, predscore_test, model, m = GradientBoostingClassifier_model(X_train, y_train, X_test, y_test)
	   # print(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw = open(filename, "a")
	   # fw.write(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw.close()
	   
	   # predscore_train, predscore_test, model, m = GaussianNB_model(X_train, y_train, X_test, y_test)
	   # print(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw = open(filename, "a")
	   # fw.write(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw.close()

		

	   # predscore_train, predscore_test, model, m = MLPClassifier_model(X_train, y_train, X_test, y_test)
	   # print(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw = open(filename, "a")
	   # fw.write(str(dataset) + "\t" + str(i) + "\t" + str(model) + "\t" + str(len(X_train)) + "\t" + str(len(X_test)) + "\t" + str(predscore_train*100.0) +"\t" + str(predscore_test*100.0) + "\t" + str(m) +"\n")
	   # fw.close()

