
#This is a contant  

$STARTING_X = 5  
$STARTING_Y = 5  
$INCREMENTER = 1  

$TWO_PI = 6  

$radiusValue = Array.new(5)

#This is a comment  

def findRadius(centerX, centerY, radius, pointX, pointY)      
	multiX = centerX - pointX    
	multiY = centerY - pointY    
	multiXY = 0    
	multiXY = (multiX * multiX) + (multiY * multiY) 
	if (multiXY >= (radius * radius))   
		return 0 
	end  
	return 1 
end  

def areaOperator(findX, findY, loop)      
	valueOut = 0    
	valueOut = ($TWO_PI * loop) / ((findX * findX) + (findY * findY)) 
	return valueOut 
end  

def partialFind(centerX, centerY, radius)      
	loop = 0    
	localRadius = Array.new(5)
	localFloat = Array.new(5)
	while (loop < radius)   
		findX = centerX - loop    
		findY = centerY - loop    
		$radiusValue[loop] = findRadius(centerX, centerY, radius, findX, findY) 
		localFloat[loop] = areaOperator(findX, findY, loop) 
		localRadius[loop] = $radiusValue[loop] + $INCREMENTER 
		puts "Some command"    
		puts $radiusValue[loop]      
		puts localFloat[loop]      
		loop += localRadius[loop] 
	end  
end  

partialFind($STARTING_X, $STARTING_Y, 5) 
  

