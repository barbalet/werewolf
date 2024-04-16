	// =============================================================

	// random.c

	// =============================================================

//   Copyright 1996-2024 Tom Barbalet. All rights reserved.

//   Permission is hereby granted, free of charge, to any person
//   obtaining a copy of this software and associated documentation
//   files (the "Software"), to deal in the Software without
//   restriction, including without limitation the rights to use,
//   copy, modify, merge, publish, distribute, sublicense, and/or
//   sell copies of the Software, and to permit persons to whom the
//   Software is furnished to do so, subject to the following
//   conditions:

//   The above copyright notice and this permission notice shall be
	// included in all copies or substantial portions of the Software.

//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//   OTHER DEALINGS IN THE SOFTWARE.

//   This software is a continuing work of Tom Barbalet, begun on
//   13 June 1996. No apes or cats were harmed in the writing of
//   this software.


const MAP_DIMENSION = 512;  

var genetics = [2];

var topography = [2 * 512 * 512];
var date = 0;
var time = 0;
var tide_level = 0;

var seed = [2];

function math_random() {  
	var tmp0 = 7656;
	var tmp1 = 37651;
	var runIt = 1;

	tmp0 = seed[0];
	tmp1 = seed[1];

	seed[0] = tmp1;
	if (( tmp0 & 7 ) == 0) {
		seed[0] = ( tmp1 ^ ( tmp0 >> 3 ) ^ 23141 );
		runIt = 0;
	}
	if (( tmp0 & 7 ) == 3) {
		seed[1] = ( tmp0 ^ ( tmp1 >> 1 ) ^ 53289 );
		runIt = 0;
	}
	if (( tmp0 & 7 ) == 5) {
		seed[1] = ( tmp1 ^ ( tmp0 >> 2 ) ^ 44550 );
		runIt = 0;
	}
	if (runIt == 1) {
		seed[1] = ( tmp0 ^ ( tmp1 >> 1 ) );
	}
	return ( tmp1 );
}

function tiles_non_planet(lx, ly ) {  
	var converted_x = 0;
	var converted_y = 0;
	
	converted_x = ( lx + MAP_DIMENSION ) & ( MAP_DIMENSION - 1 );
	converted_y = ( ly + MAP_DIMENSION ) & ( MAP_DIMENSION - 1 );
	return converted_x | ( converted_y * MAP_DIMENSION );
}

function tiles_topography(lx, ly) {  
	return topography[tiles_non_planet( lx, ly )];
}

function tiles_set_topography(lx, ly, value ) {   
	topography[tiles_non_planet( lx, ly )] = value;
}

function tile_patch(refine) {   

	const span_minor = ( 64 >> ( ( refine & 7 ) ^ 7 ) );  
	const span_major = ( 1 << ( ( refine & 7 ) ^ 7 ) );  
    
	var py = 0;
	while ( py < span_minor ) {
		var px = 0;
		while ( px < span_minor ) {
			var val1 = 0;
			var ty = 0;
			var tseed = 0;
			
			val1 = ( ( px << 2 ) + ( py << 10 ) );
			
			seed[0] = genetics[0];
			seed[1] = genetics[1];
			
			tseed = math_random() & 65535;

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
							var pnt = 0;
							var pntx = 0;
							var pnty = 0;
							var local_map_pnt = 0;
							
							pnt = ( ( mx | ( my << 8 ) ) + ( span_major * ( val1 + val2 ) ) );
							pntx = ( pnt & 255 );
							pnty = ( pnt >> 8 );
							
							
							if (( refine & 2 ) == 2) {
								var pntx_tmp = pntx + pnty;
								pnty = pntx - pnty;
								pntx = pntx_tmp;
							}
							
							local_map_pnt = tiles_topography(pntx, pnty) + val3;

							if ( local_map_pnt < 0 ) {
								local_map_pnt = 0;
							}
							if ( local_map_pnt > 255 ) {
								local_map_pnt = 255;
							}
							tiles_set_topography(pntx, pnty, local_map_pnt);
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

function tile_round() {   
	var local_tile_dimension = MAP_DIMENSION;
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
						sum += tiles_topography(px + tx, py + ty );
						tx += 1;
					}
					ty += 1;
				}
				tiles_set_topography(px, py, ( sum / 9 ) );
				px += 1;
			}
			py += 1;
		}
		span_minor += 1;
	}
}

function tile_swap_topography() {   
	var loop = 0;
	while(loop < MAP_DIMENSION) {
		topography[MAP_DIMENSION + loop] = topography[loop];
		loop += 1;
	}
}

function land_init() {   
	var refine = 0;
	while ( refine < 7 ) {
		tile_patch( refine );
		tile_round();
		tile_swap_topography();
		refine += 1;
	}
}

function resolve() {   
	var count = [65536];
	var loop = 0;
	while(loop < 65536) {
		count[loop] = 0;
		loop += 1;
	}

	seed[0] = 7656;
	seed[1] = 37651;
	
	loop = 0;
	while(loop < 655360) {
		count[math_random() & 65535] += 1;
		loop += 1;
	}
	
	loop = 0;
	while(loop < 65536) {
		console.log(loop);    
		console.log(count[loop]);    
		loop += 1;
	}
}

land_init();
