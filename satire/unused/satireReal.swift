
// This is a contant 

let STARTING_X = 5
let STARTING_Y = 5
let TWO_PI: Float = 6.0

var radiusValue = [Int](repeating: 0, count: 5)

// Function to find radius
func findRadius(centerX: Int, centerY: Int, radius: Int, pointX: Int, pointY: Int) -> Int {
    let multiX = centerX - pointX
    let multiY = centerY - pointY
    let multiXY = (multiX * multiX) + (multiY * multiY)
    if multiXY >= (radius * radius) {
        return 0
    }
    return 1
}

// Function to calculate area
func areaOperator(findX: Int, findY: Int, loop: Int) -> Float {
    var valueOut: Float = 0.0
    valueOut = (TWO_PI * Float(loop)) / (Float(findX * findX) + Float(findY * findY))
    return valueOut
}

// Function to find partial radius
func partialFind(centerX: Int, centerY: Int, radius: Int) {
    let INCREMENTER = 1
    var loop = 0
    var localRadius = [Int](repeating: 0, count: 5)
    var localFloat = [Float](repeating: 0.0, count: 5)
    while loop < radius {
        let findX = centerX - loop
        let findY = centerY - loop
        radiusValue[loop] = findRadius(centerX: centerX, centerY: centerY, radius: radius, pointX: findX, pointY: findY)
        localFloat[loop] = areaOperator(findX: findX, findY: findY, loop: loop)
        localRadius[loop] = radiusValue[loop] + INCREMENTER
        print("Some command")
        print(radiusValue[loop])
        print(localFloat[loop])
        loop += localRadius[loop]
    }
}

// Main function
partialFind(centerX: STARTING_X, centerY: STARTING_Y, radius: 5)
