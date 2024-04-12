#include <stdio.h>

// This is a contant

const int STARTING_X = 5;
const int STARTING_Y = 5;

int radiusValue[5];

// This is a comment

int findRadius(int centerX, int centerY, int radius, int pointX, int pointY) {
	int multiX = centerX - pointX;
	int multiY = centerY - pointY;
	int multiXY = 0;
	multiXY = (multiX * multiX) + (multiY * multiY);
	if (multiXY >= (radius * radius)) {
		return 0;
	}
	return 1;
}

void partialFind(int centerX, int centerY, int radius) {
	const int INCREMENTER = 1;
	int loop = 0;
	int localRadius[5];
	while (loop < radius) {
		radiusValue[loop] = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop);
		localRadius[loop] = radiusValue[loop] + INCREMENTER;
		printf("Some command\n");
		printf("%d\n", radiusValue[loop]);
		loop += localRadius[loop];
	}
}

int main (void) {
	partialFind(STARTING_X, STARTING_Y, 5);
	return 1;
}
