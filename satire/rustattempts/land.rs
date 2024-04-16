
// Import required libraries
use rand::Rng;

// Constants
const MAP_DIMENSION: usize = 512;

// Global variables
let mut genetics: [u32; 2] = [0; 2];
let mut topography: Vec<u8> = vec![0; 2 * MAP_DIMENSION * MAP_DIMENSION];
let mut date: u32 = 0;
let mut time: u32 = 0;
let mut tide_level: u32 = 0;
let mut seed: [u32; 2] = [0; 2];

fn math_random() -> u32 {
    let mut tmp0 = 7656;
    let mut tmp1 = 37651;
    let mut run_it = 1;

    tmp0 = seed[0];
    tmp1 = seed[1];

    seed[0] = tmp1;
    if (tmp0 & 7) == 0 {
        seed[0] = (tmp1 ^ (tmp0 >> 3) ^ 23141);
        run_it = 0;
    }
    if (tmp0 & 7) == 3 {
        seed[1] = (tmp0 ^ (tmp1 >> 1) ^ 53289);
        run_it = 0;
    }
    if (tmp0 & 7) == 5 {
        seed[1] = (tmp1 ^ (tmp0 >> 2) ^ 44550);
        run_it = 0;
    }
    if run_it == 1 {
        seed[1] = (tmp0 ^ (tmp1 >> 1));
    }
    tmp1
}

fn tiles_non_planet(lx: i32, ly: i32) -> usize {
    let converted_x = (lx + MAP_DIMENSION as i32) & (MAP_DIMENSION as i32 - 1);
    let converted_y = (ly + MAP_DIMENSION as i32) & (MAP_DIMENSION as i32 - 1);
    (converted_x as usize) | ((converted_y as usize) * MAP_DIMENSION)
}

fn tiles_topography(lx: i32, ly: i32) -> u8 {
    topography[tiles_non_planet(lx, ly)]
}

fn tiles_set_topography(lx: i32, ly: i32, value: u8) {
    topography[tiles_non_planet(lx, ly)] = value;
}

fn tile_patch(refine: u32) {
    let span_minor = (64 >> ((refine & 7) ^ 7)) as i32;
    let span_major = (1 << ((refine & 7) ^ 7)) as i32;

    let mut py = 0;
    while py < span_minor {
        let mut px = 0;
        while px < span_minor {
            let mut val1 = ((px << 2) + (py << 10)) as u32;
            seed[0] = genetics[0];
            seed[1] = genetics[1];
            let tseed = math_random() & 65535;

            let mut ty = 0;
            while ty < 4 {
                let mut tx = 0;
                while tx < 4 {
                    let mut val2 = (tseed >> (tx | (ty << 2))) as u32;
                    let mut val3 = (((((val2 & 1) << 1) - 1) * 20) as i32) as u8;
                    val2 = (tx | (ty << 8)) as u32;

                    let mut my = 0;
                    while my < span_major {
                        let mut mx = 0;
                        while mx < span_major {
                            let pnt = ((mx | (my << 8)) + (span_major * (val1 + val2))) as u32;
                            let mut pntx = (pnt & 255) as i32;
                            let mut pnty = ((pnt >> 8) as i32);

                            if (refine & 2) == 2 {
                                let pntx_tmp = pntx + pnty;
                                pnty = pntx - pnty;
                                pntx = pntx_tmp;
                            }

                            let mut local_map_pnt = (tiles_topography(pntx, pnty) as i32) + val3 as i32;

                            if local_map_pnt < 0 {
                                local_map_pnt = 0;
                            }
                            if local_map_pnt > 255 {
                                local_map_pnt = 255;
                            }
                            tiles_set_topography(pntx, pnty, local_map_pnt as u8);
                            mx += 1;
                        }
                        my += 1;
                    }
                    tx += 1;
                }
                ty += 1;
            }
            px += 1;
        }
        py += 1;
    }
}

fn tile_round() {
    let local_tile_dimension = MAP_DIMENSION as i32;
    let mut span_minor = 0;
    while span_minor < 6 {
        let mut py = 0;
        while py < local_tile_dimension {
            let mut px = 0;
            while px < local_tile_dimension {
                let mut sum = 0;
                let mut ty = -1;
                while ty < 2 {
                    let mut tx = -1;
                    while tx < 2 {
                        sum += tiles_topography(px + tx, py + ty) as i32;
                        tx += 1;
                    }
                    ty += 1;
                }
                tiles_set_topography(px, py, (sum / 9) as u8);
                px += 1;
            }
            py += 1;
        }
        span_minor += 1;
    }
}

fn tile_swap_topography() {
    let mut loop_counter = 0;
    while loop_counter < MAP_DIMENSION {
        topography[MAP_DIMENSION + loop_counter] = topography[loop_counter];
        loop_counter += 1;
    }
}

fn land_init() {
    let mut refine = 0;
    while refine < 7 {
        tile_patch(refine);
        tile_round();
        tile_swap_topography();
        refine += 1;
    }
}

fn resolve() {
    let mut count = vec![0; 65536];
    let mut loop_counter = 0;

    seed[0] = 7656;
    seed[1] = 37651;

    loop_counter = 0;
    while loop_counter < 655360 {
        count[(math_random() & 65535) as usize] += 1;
        loop_counter += 1;
    }

    loop_counter = 0;
    while loop_counter < 65536 {
        println!("{}", loop_counter);
        println!("{}", count[loop_counter]);
        loop_counter += 1;
    }
}

fn main() {
    land_init();
}

