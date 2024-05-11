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


#include <stdio.h>

const int MAP_DIMENSION = 512;

int genetics[2];

int topography[2 * 512 * 512];

int math_random() {
	int tmp0 = 0;
	int tmp1 = 0;
	int runIt = 1;

	tmp0 = genetics[0];
	tmp1 = genetics[1];

	genetics[0] = tmp1;
	int tempAnd7 = tmp0 & 7;

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

int tiles_non_planet( int lx, int ly ) {
	int converted_x = 0;
	int converted_y = 0;
	converted_x = ( lx + MAP_DIMENSION ) & ( MAP_DIMENSION - 1 );
	converted_y = ( ly + MAP_DIMENSION ) & ( MAP_DIMENSION - 1 );
	return ( converted_x | ( converted_y * MAP_DIMENSION ) );
}

int tiles_topography( int buffer, int lx, int ly ) {
	return topography[(buffer * 512 * 512) + tiles_non_planet( lx, ly )];
}

void tiles_set_topography( int buffer, int lx, int ly, int value ) {
	topography[(buffer * 512 * 512) + tiles_non_planet( lx, ly )] = value;
}

void tiles_swap_topography() {
	int loop = 0;
	while (loop < (512 * 512)) {
		topography[(512 * 512) + loop] = topography[loop];
		loop += 1;
	}
}

void title_pack_topography() {
	int loop = 0;
	while (loop < (512 * 512)) {
		topography[loop] = 128;
		loop += 1;
	}
}

void tile_round() {
	int local_tile_dimension = 1 << 9;
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

void tile_patch( int refine ) {
	int local_tiles = 2;
	int span_minor = 0;
	int span_major = 0;
	int tile_y = 0;
	span_minor = ( 64 >> ( ( refine & 7 ) ^ 7 ) );
	span_major = ( 1 << ( ( refine & 7 ) ^ 7 ) );
	while ( tile_y < local_tiles ) {
		int tile_x = 0;
		while ( tile_x < local_tiles ) {
			int py = 0;
			while ( py < span_minor ) {
				int px = 0;
				while ( px < span_minor ) {
					int val1 = 0;
					int ty = 0;
					int tseed = 0;
					tseed = math_random();
					val1 = ( ( px << 2 ) + ( py << 10 ) );
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
									int local_map_point = 0;
									int point = 0;
									int pointx = 0;
									int pointy = 0;
									
									point = ( ( mx | ( my << 8 ) ) + ( span_major * ( val1 + val2 ) ) );
									pointx = ( point & 255 );
									pointy = ( point >> 8 );
									if (( refine & 2 ) != 0) {
										int pointx_tmp = pointx + pointy;
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

void land_seed_genetics( int r1, int r2 ) {
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

void land_init() {
	int refine = 0;
	title_pack_topography();
	while ( refine < 7 ) {
		tile_patch( refine );
		tile_round();
		tiles_swap_topography();
		refine += 1;
	}
}

// Comment out if not run with other code

int main (void) {
	land_seed_genetics(5162,14583);
	printf("Land start\n");
	land_init();
	printf("Land end\n");
	printf("Expect 159\n");
	printf("%d\n", topography[249614]);
	printf("Expect 97\n");
	printf("%d\n", topography[242701]);
	printf("Expect 151\n");
	printf("%d\n", topography[42701]);
	printf("Expect 99\n");
	printf("%d\n", topography[9614]);
}
