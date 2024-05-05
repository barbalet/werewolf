#include <stdio.h>

// This is a contant

const int STARTING_X = 5;
const int STARTING_Y = 5;
const int INCREMENTER = 1;

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
	int loop = 0;
	while (loop < radius) {
		int radiusValue = 0;
		radiusValue = findRadius(centerX, centerY, radius, centerX - loop, centerY - loop);
		printf("Some command\n");
		printf("%d\n", radiusValue);
		loop += INCREMENTER + radiusValue;
	}
}

int main (void) {
	partialFind(STARTING_X, STARTING_Y, 5);
	return 1;
}
