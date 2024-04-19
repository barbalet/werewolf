
fn math_random(local: &mut [i32; 2]) -> i32 {
    let tmp0 = local[0];
    let tmp1 = local[1];
    let mut run_it = true;

    local[0] = tmp1;
    if (tmp0 & 7) == 0 {
        local[0] = tmp1 ^ (tmp0 >> 3) ^ 23141;
        run_it = false;
    }
    if (tmp0 & 7) == 3 {
        local[1] = tmp0 ^ (tmp1 >> 1) ^ 53289;
        run_it = false;
    }
    if (tmp0 & 7) == 5 {
        local[1] = tmp1 ^ (tmp0 >> 2) ^ 44550;
        run_it = false;
    }
    if run_it {
        local[1] = tmp0 ^ (tmp1 >> 1);
    }
    tmp1
}

fn resolve() {
    let mut local = [7656, 37651];
    let mut count = [0; 65536];

    for _ in 0..655360 {
        let random_value = math_random(&mut local) as usize & 65535;
        count[random_value] += 1;
    }

    for (i, count) in count.iter().enumerate() {
        println!("{}", i);
        println!("{}", count);
    }
}

fn main() {
    resolve();
}