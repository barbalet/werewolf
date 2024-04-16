// Importing necessary libraries
use std::io;

// This is a constant
const STARTING_X: i32 = 5;
const STARTING_Y: i32 = 5;

static mut RADIUS_VALUE: [i32; 5] = [0; 5];

// This is a comment

fn find_radius(center_x: i32, center_y: i32, radius: i32, point_x: i32, point_y: i32) -> i32 {
    let multi_x = center_x - point_x;
    let multi_y = center_y - point_y;
    let multi_xy = multi_x * multi_x + multi_y * multi_y;
    if multi_xy >= (radius * radius) {
        0
    } else {
        1
    }
}

fn partial_find(center_x: i32, center_y: i32, radius: i32) {
    const INCREMENTER: i32 = 1;
    let mut loop_var = 0;
    let mut local_radius = [0; 5];
    while loop_var < radius {
        unsafe {
            RADIUS_VALUE[loop_var] = find_radius(center_x, center_y, radius, center_x - loop_var, center_y - loop_var);
            local_radius[loop_var] = RADIUS_VALUE[loop_var] + INCREMENTER;
        }
        println!("Some command");
        unsafe {
            println!("{}", RADIUS_VALUE[loop_var]);
        }
        loop_var += local_radius[loop_var];
    }
}

fn main() {
    partial_find(STARTING_X, STARTING_Y, 5);
}


