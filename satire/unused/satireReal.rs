
// Constants
const STARTING_X: i32 = 5;
const STARTING_Y: i32 = 5;
const TWO_PI: f32 = 6.0;

// Global variable
static mut RADIUS_VALUE: [i32; 5] = [0; 5];

// Function to find radius
fn find_radius(center_x: i32, center_y: i32, radius: i32, point_x: i32, point_y: i32) -> i32 {
    let multi_x = center_x - point_x;
    let multi_y = center_y - point_y;
    let multi_xy = multi_x * multi_x + multi_y * multi_y;
    if multi_xy >= radius * radius {
        0
    } else {
        1
    }
}

// Function to calculate area
fn area_operator(find_x: i32, find_y: i32, loop_var: i32) -> f32 {
    TWO_PI * loop_var as f32 / ((find_x * find_x + find_y * find_y) as f32)
}

// Function to partially find values
fn partial_find(center_x: i32, center_y: i32, radius: i32) {
    const INCREMENTER: i32 = 1;
    let mut loop_var = 0;
    let mut local_radius = [0; 5];
    let mut local_float = [0.0; 5];
    while loop_var < radius {
        let find_x = center_x - loop_var;
        let find_y = center_y - loop_var;
        unsafe {
            RADIUS_VALUE[loop_var as usize] = find_radius(center_x, center_y, radius, find_x, find_y);
            local_float[loop_var as usize] = area_operator(find_x, find_y, loop_var);
            local_radius[loop_var as usize] = RADIUS_VALUE[loop_var as usize] + INCREMENTER;
        }
        println!("Some command");
        unsafe {
            println!("{}", RADIUS_VALUE[loop_var as usize]);
            println!("{}", local_float[loop_var as usize]);
        }
        loop_var += local_radius[loop_var as usize];
    }
}

// Main function
fn main() {
    partial_find(STARTING_X, STARTING_Y, 5);
}


