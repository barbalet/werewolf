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
static mut TOPOGRAPHY: [i32; 2 * MAP_DIMENSION * MAP_DIMENSION] = [0; 2 * MAP_DIMENSION * MAP_DIMENSION];

fn math_random() -> i32 {
    unsafe {
        let tmp0 = GENETICS[0];
        let tmp1 = GENETICS[1];

        GENETICS[0] = tmp1;
        match tmp0 & 7 {
            0 => {
                GENETICS[0] = tmp1 ^ (tmp0 >> 3) ^ 23141;
            },
            3 => {
                GENETICS[1] = tmp0 ^ (tmp1 >> 1) ^ 53289;
            },
            5 => {
                GENETICS[1] = tmp1 ^ (tmp0 >> 2) ^ 44550;
            },
            _ => {
                GENETICS[1] = tmp0 ^ (tmp1 >> 1);
            }
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
    unsafe { TOPOGRAPHY[(buffer * MAP_DIMENSION * MAP_DIMENSION) + tiles_non_planet(lx, ly)] }
}

fn tiles_set_topography(buffer: usize, lx: i32, ly: i32, value: i32) {
    unsafe {
        TOPOGRAPHY[(buffer * MAP_DIMENSION * MAP_DIMENSION) + tiles_non_planet(lx, ly)] = value;
    }
}

fn tiles_swap_topography() {
    for loop_idx in 0..(MAP_DIMENSION * MAP_DIMENSION) {
        unsafe {
            TOPOGRAPHY[(MAP_DIMENSION * MAP_DIMENSION) + loop_idx] = TOPOGRAPHY[loop_idx];
        }
    }
}

fn title_pack_topography() {
    for loop_idx in 0..(MAP_DIMENSION * MAP_DIMENSION) {
        unsafe {
            TOPOGRAPHY[loop_idx] = 128;
        }
    }
}

fn tile_round() {
    let local_tile_dimension = 1 << 9;
    for span_minor in 0..6 {
        for py in 0..local_tile_dimension {
            for px in 0..local_tile_dimension {
                let mut sum = 0;
                for ty in -1..=1 {
                    for tx in -1..=1 {
                        sum += tiles_topography((span_minor & 1) as usize, px + tx, py + ty);
                    }
                }
                tiles_set_topography((span_minor & 1) as usize ^ 1, px, py, sum / 9);
            }
        }
    }
}

fn tile_patch(refine: i32) {
    let local_tiles = 2;
    let span_minor = (64 >> ((refine & 7) ^ 7)) as i32;
    let span_major = (1 << ((refine & 7) ^ 7)) as i32;
    for tile_y in 0..local_tiles {
        for tile_x in 0..local_tiles {
            for py in 0..span_minor {
                for px in 0..span_minor {
                    let tseed = math_random();
                    let val1 = (px << 2) + (py << 10);
                    for ty in 0..4 {
                        for tx in 0..4 {
                            let val2 = (tseed >> (tx | (ty << 2))) & 1;
                            let val3 = ((val2 << 1) - 1) * 20;
                            let val2 = tx | (ty << 8);

                            for my in 0..span_major {
                                for mx in 0..span_major {
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
                                }
                            }
                        }
                    }
                }
            }
        }
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
    title_pack_topography();
    for refine in 0..7 {
        tile_patch(refine);
        tile_round();
        tiles_swap_topography();
    }
}

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
