
// This is a contant

const STARTING_X = 5;  
const STARTING_Y = 5;  

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
	let loop = 0;
	while (loop < radius) {
		let radiusValue = 0;
		const incrementNumber = 1;  
		radiusValue = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop);
		console.log("Some command");    
		console.log(radiusValue);    
		loop += incrementNumber + radiusValue;
	}
}

partialFind(STARTING_X, STARTING_Y, 5);

