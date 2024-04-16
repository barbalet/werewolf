use std::collections::HashMap;

static mut LOCAL: [i32; 2] = [0; 2];

fn math_random() -> i32 {
    let mut tmp0 = 7656;
    let mut tmp1 = 37651;
    let mut run_it = true;

    unsafe {
        tmp0 = LOCAL[0];
        tmp1 = LOCAL[1];

        LOCAL[0] = tmp1;
    }

    if (tmp0 & 7) == 0 {
        unsafe {
            LOCAL[0] = tmp1 ^ (tmp0 >> 3) ^ 23141;
        }
        run_it = false;
    }
    if (tmp0 & 7) == 3 {
        unsafe {
            LOCAL[1] = tmp0 ^ (tmp1 >> 1) ^ 53289;
        }
        run_it = false;
    }
    if (tmp0 & 7) == 5 {
        unsafe {
            LOCAL[1] = tmp1 ^ (tmp0 >> 2) ^ 44550;
        }
        run_it = false;
    }
    if run_it {
        unsafe {
            LOCAL[1] = tmp0 ^ (tmp1 >> 1);
        }
    }
    tmp1
}

fn resolve() {
    let mut count: HashMap<usize, i32> = HashMap::new();
    let mut loop_index = 0;

    while loop_index < 65536 {
        count.insert(loop_index, 0);
        loop_index += 1;
    }

    unsafe {
        LOCAL[0] = 7656;
        LOCAL[1] = 37651;
    }

    loop_index = 0;
    while loop_index < 655360 {
        let random_value = math_random() as usize & 65535;
        *count.entry(random_value).or_insert(0) += 1;
        loop_index += 1;
    }

    loop_index = 0;
    while loop_index < 65536 {
        println!("{}: {}", loop_index, count[&loop_index]);
        loop_index += 1;
    }
}

fn main() {
    resolve();
}


