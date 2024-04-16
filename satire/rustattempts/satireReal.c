#include <stdio.h>

// This is a contant

const int STARTING_X = 5;
const int STARTING_Y = 5;

const float TWO_PI = 6;

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

float areaOperator(int findX, int findY, int loop) {
	float valueOut = 0;
	valueOut = (TWO_PI * loop) / ((findX * findX) + (findY * findY));
	return valueOut;
}

void partialFind(int centerX, int centerY, int radius) {
	const int INCREMENTER = 1;
	int loop = 0;
	int localRadius[5];
	float localFloat[5];
	while (loop < radius) {
		int findX = centerX - loop;
		int findY = centerY - loop;
		radiusValue[loop] = findRadius(centerX, centerY, radius, findX, findY);
		localFloat[loop] = areaOperator(findX, findY, loop);
		localRadius[loop] = radiusValue[loop] + INCREMENTER;
		printf("Some command\n");
		printf("%d\n", radiusValue[loop]);
		printf("%f\n", localFloat[loop]);
		loop += localRadius[loop];
	}
}

int main (void) {
	partialFind(STARTING_X, STARTING_Y, 5);
	return 1;
}
