// land.c
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



const MAP_DIMENSION = 512;  

var genetics = [2];

var topography = [2 * 512 * 512];

function math_random() {   
	var tmp0 = 0;
	var tmp1 = 0;
	var runIt = 1;

	tmp0 = genetics[0];
	tmp1 = genetics[1];

	genetics[0] = tmp1;
	var tempAnd7 = tmp0 & 7;

	if (tempAnd7 == 0) {
		genetics[0] = ( tmp1 ^ ( tmp0 >> 3 ) ^ 23141 );
		runIt = 0;
	}
	if (tempAnd7 == 3) {
		genetics[1] = ( tmp0 ^ ( tmp1 >> 1 ) ^ 53289 );
		runIt = 0;
	}
	if (tempAnd7 == 5) {
		genetics[1] = ( tmp1 ^ ( tmp0 >> 2 ) ^ 44550 );
		runIt = 0;
	}
	if (runIt == 1) {
		genetics[1] = ( tmp0 ^ ( tmp1 >> 1 ) );
	}
	return genetics[1];
}

function tiles_non_planet( lx, ly ) {   
	var converted_x = 0;
	var converted_y = 0;
	converted_x = ( lx + MAP_DIMENSION ) & ( MAP_DIMENSION - 1 );
	converted_y = ( ly + MAP_DIMENSION ) & ( MAP_DIMENSION - 1 );
	return ( converted_x | ( converted_y * MAP_DIMENSION ) );
}

function tiles_topography( buffer, lx, ly ) {   
	return topography[(buffer * 512 * 512) + tiles_non_planet( lx, ly )];
}

function tiles_set_topography( buffer, lx, ly, value ) {    
	topography[(buffer * 512 * 512) + tiles_non_planet( lx, ly )] = value;
}

function tiles_swap_topography() {    
	var loop = 0;
	while (loop < (512 * 512)) {
		topography[(512 * 512) + loop] = topography[loop];
		loop += 1;
	}
}

function title_pack_topography() {    
	var loop = 0;
	while (loop < (512 * 512)) {
		topography[loop] = 128;
		loop += 1;
	}
}

function tile_round() {    
	var local_tile_dimension = 1 << 9;
	var span_minor = 0;
	while ( span_minor < 6 ) {
		var py = 0;
		while ( py < local_tile_dimension ) {
			var px = 0;
			while ( px < local_tile_dimension ) {
				var sum = 0;
				var ty = -1;
				while ( ty < 2 ) {
					var tx = -1;
					while ( tx < 2 ) {
						sum += tiles_topography( ( span_minor & 1 ), px + tx, py + ty );
						tx += 1;
					}
					ty += 1;
				}
				tiles_set_topography( ( span_minor & 1 ) ^ 1, px, py, sum / 9 );
				px += 1;
			}
			py += 1;
		}
		span_minor += 1;
	}
}

function tile_patch( refine ) {    
	var local_tiles = 2;
	var span_minor = 0;
	var span_major = 0;
	var tile_y = 0;
	span_minor = ( 64 >> ( ( refine & 7 ) ^ 7 ) );
	span_major = ( 1 << ( ( refine & 7 ) ^ 7 ) );
	while ( tile_y < local_tiles ) {
		var tile_x = 0;
		while ( tile_x < local_tiles ) {
			var py = 0;
			while ( py < span_minor ) {
				var px = 0;
				while ( px < span_minor ) {
					var val1 = 0;
					var ty = 0;
					var tseed = 0;
					tseed = math_random();
					val1 = ( ( px << 2 ) + ( py << 10 ) );
					while ( ty < 4 ) {
						var tx = 0;
						while ( tx < 4 ) {
							var val2 = 0;
							var val3 = 0;
							var my = 0;

							val2 = ( tseed >> ( tx | ( ty << 2 ) ) );
							val3 = ( ( ( ( val2 & 1 ) << 1 ) - 1 ) * 20 );

							val2 = ( tx | ( ty << 8 ) );

							while ( my < span_major ) {
								var mx = 0;
								while ( mx < span_major ) {
									var local_map_point = 0;
									var point = 0;
									var pointx = 0;
									var pointy = 0;
									
									point = ( ( mx | ( my << 8 ) ) + ( span_major * ( val1 + val2 ) ) );
									pointx = ( point & 255 );
									pointy = ( point >> 8 );
									if (( refine & 2 ) != 0) {
										var pointx_tmp = pointx + pointy;
										pointy = pointx - pointy;
										pointx = pointx_tmp;
									}
									local_map_point = tiles_topography( 0, pointx + ( tile_x << 8 ), pointy + ( tile_y << 8 ) ) + val3;

									if ( local_map_point < 0 ) {
										local_map_point = 0;
									}
									if ( local_map_point > 255 ) {
										local_map_point = 255;
									}

									tiles_set_topography( 0, pointx + ( tile_x << 8 ), pointy + ( tile_y << 8 ), local_map_point );
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

function land_seed_genetics( r1, r2 ) {    
	genetics[0] = r1;
	genetics[1] = r2;

	genetics[0] = ( ( ( math_random() & 255 ) << 8 ) | ( math_random() & 255 ) );
	genetics[1] = ( ( ( math_random() & 255 ) << 8 ) | ( math_random() & 255 ) );

	math_random();
	math_random();
	math_random();

	genetics[0] = ( ( ( math_random() & 255 ) << 8 ) | ( math_random() & 255 ) );
	genetics[1] = ( ( ( math_random() & 255 ) << 8 ) | ( math_random() & 255 ) );
}

function land_init() {    
	var refine = 0;
	title_pack_topography();
	while ( refine < 7 ) {
		tile_patch( refine );
		tile_round();
		tiles_swap_topography();
		refine += 1;
	}
}

// Comment out if not run with other code

land_seed_genetics(5162,14583);
console.log("Land start");    
land_init();
console.log("Land end");    
console.log("Expect 159");    
console.log(topography[249614]);    
console.log("Expect 97");    
console.log(topography[242701]);    
console.log("Expect 151");    
console.log(topography[42701]);    
console.log("Expect 99");    
console.log(topography[9614]);    

