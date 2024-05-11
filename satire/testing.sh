#!/bin/bash
# 	testing.sh

# 	=============================================================

#   Copyright 1996-2024 Tom Barbalet. All rights reserved.

#   Permission is hereby granted, free of charge, to any person
#   obtaining a copy of this software and associated documentation
#   files (the "Software"), to deal in the Software without
#   restriction, including without limitation the rights to use,
#   copy, modify, merge, publish, distribute, sublicense, and/or
#   sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following
#   conditions:

#   The above copyright notice and this permission notice shall be
# 	included in all copies or substantial portions of the Software.

#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#   OTHER DEALINGS IN THE SOFTWARE.

#   This software is a continuing work of Tom Barbalet, begun on
#   13 June 1996. No apes or cats were harmed in the writing of
#   this software.

echo "--- check code ---"

cc werewolf.c -o ww

./ww land.c -p land.py -j land.java -js land.js -r land.rb
./ww satireReal.c -js satireReal.js -p satireReal.py -j satireReal.java -r satireReal.rb

cc land.c -o land
cc satireReal.c -o satireReal

./land
./satireReal

rm land
rm satireReal

echo "--- copy code ---"

cp land.js testing/land.js
cp land.py testing/land.py
cp land.java testing/land.java
cp land.rb testing/land.rb

cp satireReal.js testing/satireReal.js
cp satireReal.py testing/satireReal.py
cp satireReal.java testing/satireReal.java
cp satireReal.rb testing/satireReal.rb

rm ww

rm land.js
rm land.py
rm land.java
rm land.rb

rm satireReal.js
rm satireReal.py
rm satireReal.java
rm satireReal.rb

cd testing

diff satireReal.js satireReal_compare.js
diff satireReal.py satireReal_compare.py
diff satireReal.java satireReal_compare.java
diff satireReal.rb satireReal_compare.rb

echo "--- land.java ---"
diff land.java land_compare.java
echo "--- land.py ---"
diff land.py land_compare.py
echo "--- land.js ---"
diff land.js land_compare.js
echo "--- land.rb ---"
diff land.rb land_compare.rb

rm land.js
rm land.py
rm land.java
rm land.rb

rm satireReal.js
rm satireReal.py
rm satireReal.java
rm satireReal.rb

