
#This is a contant  

STARTING_X = 5  
STARTING_Y = 5  

TWO_PI = 6  

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

def areaOperator(findX, findY, loop):     
	valueOut = 0    
	valueOut = (TWO_PI * loop) / ((findX * findX) + (findY * findY)) 
	return valueOut 

def partialFind(centerX, centerY, radius):     
	INCREMENTER = 1  
	loop = 0    
	localRadius = [0] * 5
	localFloat = [0] * 5
	while (loop < radius):  
		findX = centerX - loop    
		findY = centerY - loop    
		radiusValue[loop] = findRadius(centerX, centerY, radius, findX, findY) 
		localFloat[loop] = areaOperator(findX, findY, loop) 
		localRadius[loop] = radiusValue[loop] + INCREMENTER 
		print("Some command")   
		print(radiusValue[loop])     
		print(localFloat[loop])     
		loop += localRadius[loop] 

partialFind(STARTING_X, STARTING_Y, 5) 

