
// This is a contant

const STARTING_X = 5;  
const STARTING_Y = 5;  

const TWO_PI = 6;  

var radiusValue = [5];

// This is a comment

function findRadius(centerX, centerY, radius, pointX, pointY) {  
	var multiX = centerX - pointX;
	var multiY = centerY - pointY;
	var multiXY = 0;
	multiXY = (multiX * multiX) + (multiY * multiY);
	if (multiXY >= (radius * radius)) {
		return 0;
	}
	return 1;
}

function areaOperator(findX, findY, loop) {   
	var valueOut = 0; 
	valueOut = (TWO_PI * loop) / ((findX * findX) + (findY * findY));
	return valueOut;
}

function partialFind(centerX, centerY, radius) {   
	const INCREMENTER = 1;  
	var loop = 0;
	var localRadius = [5];
	var localFloat = [5];
	while (loop < radius) {
		var findX = centerX - loop;
		var findY = centerY - loop;
		radiusValue[loop] = findRadius(centerX, centerY, radius, findX, findY);
		localFloat[loop] = areaOperator(findX, findY, loop);
		localRadius[loop] = radiusValue[loop] + INCREMENTER;
		console.log("Some command");    
		console.log(radiusValue[loop]);    
		console.log(localFloat[loop]);    
		loop += localRadius[loop];
	}
}

partialFind(STARTING_X, STARTING_Y, 5);

