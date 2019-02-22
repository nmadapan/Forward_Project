import random
passes = ["R-L 1", "R-L 2", "R-L 3", "L-R 1", "L-R 2", "L-R 3"]
print("RANDOM PASSES")
for i in range(8):
	random.shuffle(passes)
	for elem in passes:
		print(elem)
print("///////////")
print("RANDOM PEGS")
for i in range(48):
	 print(random.sample(range(1,7), 3))


