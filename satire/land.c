// 	random.c

// 	=============================================================

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
// 	included in all copies or substantial portions of the Software.

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
			int tseed;
			
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
							int point = 0;
							int pointx = 0;
							int pointy = 0;
							int local_map_point;
							
							point = ( ( mx | ( my << 8 ) ) + ( span_major * ( val1 + val2 ) ) );
							pointx = ( point & 255 );
							pointy = ( point >> 8 );
							
							
							if (( refine & 2 ) == 2) {
								int pointx_tmp = pointx + pointy;
								pointy = pointx - pointy;
								pointx = pointx_tmp;
							}
							
							local_map_point = tiles_topography(pointx, pointy) + val3;

							if ( local_map_point < 0 ) {
								local_map_point = 0;
							}
							if ( local_map_point > 255 ) {
								local_map_point = 255;
							}
							tiles_set_topography(pointx, pointy, local_map_point);
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

// void tile_round() {
// 
// }
// 
// void tile_swap_topography() {
// 
// }

void land_init() {
	int refine = 0;
	while ( refine < 7 ) {
		tile_patch( refine );
// 		tile_round();
// 		tile_swap_topography();
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
	resolve();	
}