
// This is a contant

const STARTING_X = 5;  
const STARTING_Y = 5;  
const INCREMENTER = 1;  

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

function partialFind(centerX, centerY, radius) {    
	var loop = 0;
	while (loop < radius) {
		var radiusValue = 0;
		radiusValue = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop);
		console.log("Some command");    
		console.log(radiusValue);    
		loop += INCREMENTER + radiusValue;
	}
}

partialFind(STARTING_X, STARTING_Y, 5);

