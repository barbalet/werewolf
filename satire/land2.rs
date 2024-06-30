// land.rs
//
// =============================================================
//
// Copyright 1996-2024 Tom Barbalet. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// This software is a continuing work of Tom Barbalet, begun on
// 13 June 1996. No apes or cats were harmed in the writing of
// this software.

const MAP_DIMENSION: usize = 512;

static mut GENETICS: [i32; 2] = [0, 0];
static mut TOPOGRAPHY: [i32; 2 * 512 * 512] = [0; 2 * 512 * 512];

fn math_random() -> i32 {
    unsafe {
        let tmp0 = GENETICS[0];
        let tmp1 = GENETICS[1];

        GENETICS[0] = tmp1;
        let temp_and_7 = tmp0 & 7;

        if temp_and_7 == 0 {
            GENETICS[0] = tmp1 ^ (tmp0 >> 3) ^ 23141;
        } else if temp_and_7 == 3 {
            GENETICS[1] = tmp0 ^ (tmp1 >> 1) ^ 53289;
        } else if temp_and_7 == 5 {
            GENETICS[1] = tmp1 ^ (tmp0 >> 2) ^ 44550;
        } else {
            GENETICS[1] = tmp0 ^ (tmp1 >> 1);
        }
        GENETICS[1]
    }
}

fn tiles_non_planet(lx: i32, ly: i32) -> usize {
    let converted_x = (lx + MAP_DIMENSION as i32) & (MAP_DIMENSION as i32 - 1);
    let converted_y = (ly + MAP_DIMENSION as i32) & (MAP_DIMENSION as i32 - 1);
    (converted_x | (converted_y * MAP_DIMENSION as i32)) as usize
}

fn tiles_topography(buffer: usize, lx: i32, ly: i32) -> i32 {
    unsafe { TOPOGRAPHY[(buffer * 512 * 512) + tiles_non_planet(lx, ly)] }
}

fn tiles_set_topography(buffer: usize, lx: i32, ly: i32, value: i32) {
    unsafe {
        TOPOGRAPHY[(buffer * 512 * 512) + tiles_non_planet(lx, ly)] = value;
    }
}

fn tiles_swap_topography() {
    let mut loop_idx = 0;
    while loop_idx < (512 * 512) {
        unsafe {
            TOPOGRAPHY[(512 * 512) + loop_idx] = TOPOGRAPHY[loop_idx];
        }
        loop_idx += 1;
    }
}

fn title_pack_topography() {
    let mut loop_idx = 0;
    while loop_idx < (512 * 512) {
        unsafe {
            TOPOGRAPHY[loop_idx] = 128;
        }
        loop_idx += 1;
    }
}

fn tile_round() {
    let local_tile_dimension = 1 << 9;
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
                        sum += tiles_topography((span_minor & 1) as usize, px + tx, py + ty);
                        tx += 1;
                    }
                    ty += 1;
                }
                tiles_set_topography((span_minor & 1) as usize ^ 1, px, py, sum / 9);
                px += 1;
            }
            py += 1;
        }
        span_minor += 1;
    }
}

fn tile_patch(refine: i32) {
    let local_tiles = 2;
    let span_minor = (64 >> ((refine & 7) ^ 7)) as i32;
    let span_major = (1 << ((refine & 7) ^ 7)) as i32;
    let mut tile_y = 0;
    while tile_y < local_tiles {
        let mut tile_x = 0;
        while tile_x < local_tiles {
            let mut py = 0;
            while py < span_minor {
                let mut px = 0;
                while px < span_minor {
                    let mut ty = 0;
                    let tseed = math_random();
                    let val1 = (px << 2) + (py << 10);
                    while ty < 4 {
                        let mut tx = 0;
                        while tx < 4 {
                            let val2 = (tseed >> (tx | (ty << 2))) & 1;
                            let val3 = ((val2 << 1) - 1) * 20;
                            let val2 = tx | (ty << 8);

                            let mut my = 0;
                            while my < span_major {
                                let mut mx = 0;
                                while mx < span_major {
                                    let point = (mx | (my << 8)) + span_major * (val1 + val2);
                                    let mut pointx = point & 255;
                                    let mut pointy = point >> 8;
                                    if (refine & 2) != 0 {
                                        let pointx_tmp = pointx + pointy;
                                        pointy = pointx - pointy;
                                        pointx = pointx_tmp;
                                    }
                                    let mut local_map_point = tiles_topography(0, pointx + (tile_x << 8), pointy + (tile_y << 8)) + val3;

                                    if local_map_point < 0 {
                                        local_map_point = 0;
                                    }
                                    if local_map_point > 255 {
                                        local_map_point = 255;
                                    }

                                    tiles_set_topography(0, pointx + (tile_x << 8), pointy + (tile_y << 8), local_map_point);
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
            tile_x += 1;
        }
        tile_y += 1;
    }
}

fn land_seed_genetics(r1: i32, r2: i32) {
    unsafe {
        GENETICS[0] = r1;
        GENETICS[1] = r2;

        GENETICS[0] = ((math_random() & 255) << 8) | (math_random() & 255);
        GENETICS[1] = ((math_random() & 255) << 8) | (math_random() & 255);

        math_random();
        math_random();
        math_random();

        GENETICS[0] = ((math_random() & 255) << 8) | (math_random() & 255);
        GENETICS[1] = ((math_random() & 255) << 8) | (math_random() & 255);
    }
}

fn land_init() {
    let mut refine = 0;
    title_pack_topography();
    while refine < 7 {
        tile_patch(refine);
        tile_round();
        tiles_swap_topography();
        refine += 1;
    }
}

// Comment out if not run with other code

fn main() {
    land_seed_genetics(5162, 14583);
    println!("Land start");
    land_init();
    println!("Land end");
    println!("Expect 159");
    unsafe { println!("{}", TOPOGRAPHY[249614]); }
    println!("Expect 97");
    unsafe { println!("{}", TOPOGRAPHY[242701]); }
    println!("Expect 151");
    unsafe { println!("{}", TOPOGRAPHY[42701]); }
    println!("Expect 99");
    unsafe { println!("{}", TOPOGRAPHY[9614]); }
}
