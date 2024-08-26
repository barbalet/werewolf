
// This is a contant

const STARTING_X: i32 = 5;
const STARTING_Y: i32 = 5;
const INCREMENTER: i32  = 1;  

// This is a comment

fn find_radius(center_x: i32, center_y: i32, radius: i32, point_x: i32, point_y: i32) -> i32 {
    let multi_x = center_x - point_x;
    let multi_y = center_y - point_y;
    let multi_xy = (multi_x * multi_x) + (multi_y * multi_y);
    if multi_xy >= (radius * radius) {
        return 0;
    }
    1
}

fn partial_find(center_x: i32, center_y: i32, radius: i32) {
    let mut loop_count = 0;
    while loop_count < radius {
        let radius_value = find_radius(center_x, center_y, radius, center_x - loop_count, center_y - loop_count);
        println!("Some command");
        println!("{}", radius_value);
        loop_count += INCREMENTER + radius_value;
    }
}

fn main() {
    partial_find(STARTING_X, STARTING_Y, 5);
}

