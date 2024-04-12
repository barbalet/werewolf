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

int local[2];

int math_random() {
	int tmp0 = 7656;
	int tmp1 = 37651;
	int runIt = 1;

	tmp0 = local[0];
	tmp1 = local[1];

	local[0] = tmp1;
	if (( tmp0 & 7 ) == 0) {
		local[0] = ( tmp1 ^ ( tmp0 >> 3 ) ^ 23141 );
		runIt = 0;
	}
	if (( tmp0 & 7 ) == 3) {
		local[1] = ( tmp0 ^ ( tmp1 >> 1 ) ^ 53289 );
		runIt = 0;
	}
	if (( tmp0 & 7 ) == 5) {
		local[1] = ( tmp1 ^ ( tmp0 >> 2 ) ^ 44550 );
		runIt = 0;
	}
	if (runIt == 1) {
		local[1] = ( tmp0 ^ ( tmp1 >> 1 ) );
	}
	return ( tmp1 );
}

void resolve() {
	int count[65536];
	int loop = 0;
	while(loop < 65536) {
		count[loop] = 0;
		loop += 1;
	}

	local[0] = 7656;
	local[1] = 37651;
	
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