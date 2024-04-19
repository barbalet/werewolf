
// This is a contant

let STARTING_X = 5
let STARTING_Y = 5

// This is a comment

func findRadius(centerX: Int, centerY: Int, radius: Int, pointX: Int, pointY: Int) -> Int {
    let multiX = centerX - pointX
    let multiY = centerY - pointY
    var multiXY = 0
    multiXY = (multiX * multiX) + (multiY * multiY)
    if (multiXY >= (radius * radius)) {
        return 0
    }
    return 1
}

func partialFind(centerX: Int, centerY: Int, radius: Int) {
    var loop = 0;
    while (loop < radius) {
        var radiusValue = 0
        radiusValue = findRadius(centerX: centerX, centerY: centerY, radius: radius, pointX: centerX - loop, pointY: centerY - loop)
        print("Some command")
        print(radiusValue)
        loop += 1 + radiusValue
    }
}

partialFind(centerX:STARTING_X, centerY: STARTING_Y, radius: 5)

