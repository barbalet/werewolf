
package main

import (
	"fmt"
)

// This is a contant

const STARTING_X = 5
const STARTING_Y = 5
const TWO_PI     = 6.0

var radiusValue [5]int

// This is a comment
func findRadius(centerX, centerY, radius, pointX, pointY int) int {
	multiX := centerX - pointX
	multiY := centerY - pointY
	multiXY := 0
	multiXY = (multiX * multiX) + (multiY * multiY)
	if multiXY >= (radius * radius) {
		return 0
	}
	return 1
}

func areaOperator(findX, findY, loop int) float64 {
	valueOut := 0.0
	valueOut = (TWO_PI * float64(loop)) / (float64(findX*findX) + float64(findY*findY))
	return valueOut
}

func partialFind(centerX, centerY, radius int) {
	const INCREMENTER = 1
	loop := 0
	localRadius := [5]int{}
	localFloat := [5]float64{}
	for loop < radius {
		findX := centerX - loop
		findY := centerY - loop
		radiusValue[loop] = findRadius(centerX, centerY, radius, findX, findY)
		localFloat[loop] = areaOperator(findX, findY, loop)
		localRadius[loop] = radiusValue[loop] + INCREMENTER
		fmt.Println("Some command")
		fmt.Println(radiusValue[loop])
		fmt.Println(localFloat[loop])
		loop += localRadius[loop]
	}
}

func main() {
	partialFind(STARTING_X, STARTING_Y, 5)
}
