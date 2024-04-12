
#This is a contant  

STARTING_X = 5  
STARTING_Y = 5  

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
	loop = 0    
	while (loop < radius):  
		radiusValue = 0    
		incrementNumber = 1  
		radiusValue = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop) 
		print("Some command")   
		print(radiusValue)     
		loop += incrementNumber + radiusValue 

partialFind(STARTING_X, STARTING_Y, 5) 

