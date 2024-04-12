
// This is a contant

const STARTING_X = 5;  
const STARTING_Y = 5;  

var radiusValue = [5];

// This is a comment

function findRadius(centerX, centerY, radius, pointX, pointY) {  
	let multiX = centerX - pointX;
	let multiY = centerY - pointY;
	let multiXY = 0;
	multiXY = (multiX * multiX) + (multiY * multiY);
	if (multiXY >= (radius * radius)) {
		return 0;
	}
	return 1;
}

function partialFind(centerX, centerY, radius) {   
	const INCREMENTER = 1;  
	let loop = 0;
	var localRadius = [5];
	while (loop < radius) {
		radiusValue[loop] = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop);
		localRadius[loop] = radiusValue[loop] + INCREMENTER;
		console.log("Some command");    
		console.log(radiusValue[loop]);    
		loop += localRadius[loop];
	}
}

partialFind(STARTING_X, STARTING_Y, 5);

