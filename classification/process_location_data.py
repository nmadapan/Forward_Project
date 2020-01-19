import os

def allFiles(directory, ext):
    all_files = []
    for root, dirs, files in os.walk(directory):
        for filename in files:
            fname = filename.lower()
            if fname.endswith(ext):
                full_fname = os.path.join(root, filename)
                all_files.append(full_fname)
    all_files = sorted(all_files)
    return all_files
def process_location_data():
	fwx = open("data/pegpole_location.txt", "w")
	# fwy = open("suturing/experiment/Suturing_y.txt", "w")
	filedir = "data/pegpole_location"
	kindata = allFiles(filedir, ".txt")
	print(kindata)
	count = 0
	for file in kindata:
		filename = str(file).split("/")[-1]
		fparts = str(filename).split("_")
		rowid = str(fparts[0])+"_"+str(fparts[1])
		# print (filename)
		if filename.endswith("cv2Tracker.txt"):
			print (filename)
			with open(filedir+"/"+filename) as f:
				kinematic_data = []
				count = 0
				next(f) # skip header
				for line in f:
					line = line.replace("\n", "")
					line = line.replace("\r", "")
					line = line.strip()
					parts = line.split(",")
					newline = ""
					for i in range(1, len(parts)): # skip frame number
						elem = parts[i]
						if elem != "":
							newline = newline + " " + elem
					newline = newline.strip()
					#print(len(newline.split(" ")))
					if count == 0:
						fwx.write(str(rowid)+" "+newline+"\n") # make first frame same as 2nd frame as first frame missing in this dataset
						fwx.write(str(rowid)+" "+newline+"\n")
					else:
						fwx.write(str(rowid)+" "+newline+"\n")
					# kinematic_data.append(newline)
					count += 1
		# print("File read = "+str(count))
	fwx.close()
	

if __name__ == '__main__':
	process_location_data()