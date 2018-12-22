import cv2

cv2.namedWindow('Hello')

while True:
	ax = cv2.waitKey(1)
	if(ax != -1):
		print ax
	if(ax == ord('q')):
		cv2.destroyAllWindows()
		break
