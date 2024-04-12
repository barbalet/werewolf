
#This is a contant  

STARTING_X = 5  
STARTING_Y = 5  

radiusValue = [0] * 5

#This is a comment  

def findRadius(centerX, centerY, radius, pointX, pointY):     
	multiX = centerX - pointX    
	multiY = centerY - pointY    
	multiXY = 0    
	multiXY = (multiX * multiX) + (multiY * multiY) 
	if (multiXY >= (radius * radius)):  
		return 0 
	return 1 

def partialFind(centerX, centerY, radius):     
	INCREMENTER = 1  
	loop = 0    
	localRadius = [0] * 5
	while (loop < radius):  
		radiusValue[loop] = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop) 
		localRadius[loop] = radiusValue[loop] + INCREMENTER 
		print("Some command")   
		print(radiusValue[loop])     
		loop += localRadius[loop] 

partialFind(STARTING_X, STARTING_Y, 5) 

