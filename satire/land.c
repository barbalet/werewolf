// 	=============================================================

// 	land.c

// 	=============================================================

// 	Copyright 1996-2024 Tom Barbalet. All rights reserved.

// 	Permission is hereby granted, free of charge, to any person
// 	obtaining a copy of this software and associated documentation
// 	files (the "Software"), to deal in the Software without
// 	restriction, including without limitation the rights to use,
// 	copy, modify, merge, publish, distribute, sublicense, and/or
// 	sell copies of the Software, and to permit persons to whom the
// 	Software is furnished to do so, subject to the following
// 	conditions:

// 	The above copyright notice and this permission notice shall be
// 	included in all copies or substantial portions of the Software.

// 	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// 	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// 	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// 	NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// 	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// 	WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// 	FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// 	OTHER DEALINGS IN THE SOFTWARE.

// 	This software is a continuing work of Tom Barbalet, begun on
// 	13 June 1996. No apes or cats were harmed in the writing of
// 	this software.

#include <stdio.h>

const int MAP_DIMENSION = 512;

int genetics[2];

int topography[2 * 512 * 512];
int date = 0;
int time = 0;
int tide_level = 0;

int seed[2];

int math_random() {
	int tmp0 = 7656;
	int tmp1 = 37651;
	int runIt = 1;

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

int tiles_non_planet(int lx, int ly ) {
	int converted_x = 0;
	int converted_y = 0;
	
	converted_x = ( lx + MAP_DIMENSION ) & ( MAP_DIMENSION - 1 );
	converted_y = ( ly + MAP_DIMENSION ) & ( MAP_DIMENSION - 1 );
	return converted_x | ( converted_y * MAP_DIMENSION );
}

int tiles_topography(int lx, int ly) {
	return topography[tiles_non_planet( lx, ly )];
}

void tiles_set_topography(int lx, int ly, int value ) {
	topography[tiles_non_planet( lx, ly )] = value;
}

void tile_patch(int refine) {

	const int span_minor = ( 64 >> ( ( refine & 7 ) ^ 7 ) );
	const int span_major = ( 1 << ( ( refine & 7 ) ^ 7 ) );
    
	int py = 0;
	while ( py < span_minor ) {
		int px = 0;
		while ( px < span_minor ) {
			int val1 = 0;
			int ty = 0;
			int tseed = 0;
			
			val1 = ( ( px << 2 ) + ( py << 10 ) );
			
			seed[0] = genetics[0];
			seed[1] = genetics[1];
			
			tseed = math_random() & 65535;

			while ( ty < 4 ) {
				int tx = 0;
				while ( tx < 4 ) {
					int val2 = 0;
					int val3 = 0;
					int my = 0;

					val2 = ( tseed >> ( tx | ( ty << 2 ) ) );
					val3 = ( ( ( ( val2 & 1 ) << 1 ) - 1 ) * 20 );
	
					val2 = ( tx | ( ty << 8 ) );

					while ( my < span_major ) {
						int mx = 0;
						while ( mx < span_major ) {
							int pnt = 0;
							int pntx = 0;
							int pnty = 0;
							int local_map_pnt = 0;
							
							pnt = ( ( mx | ( my << 8 ) ) + ( span_major * ( val1 + val2 ) ) );
							pntx = ( pnt & 255 );
							pnty = ( pnt >> 8 );
							
							
							if (( refine & 2 ) == 2) {
								int pntx_tmp = pntx + pnty;
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

void tile_round() {
	int local_tile_dimension = MAP_DIMENSION;
	int span_minor = 0;
	while ( span_minor < 6 ) {
		int py = 0;
		while ( py < local_tile_dimension ) {
			int px = 0;
			while ( px < local_tile_dimension ) {
				int sum = 0;
				int ty = -1;
				while ( ty < 2 ) {
					int tx = -1;
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

void tile_swap_topography() {
	int loop = 0;
	while(loop < MAP_DIMENSION) {
		topography[MAP_DIMENSION + loop] = topography[loop];
		loop += 1;
	}
}

void land_init() {
	int refine = 0;
	while ( refine < 7 ) {
		tile_patch( refine );
		tile_round();
		tile_swap_topography();
		refine += 1;
	}
}

void resolve() {
	int count[65536];
	int loop = 0;
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
		printf("%d\n", loop);
		printf("%d\n", count[loop]);
		loop += 1;
	}
}

int main (void) {
	land_init();	
}