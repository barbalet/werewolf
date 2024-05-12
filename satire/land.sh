#!/bin/bash
# 	land.sh

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

echo "--- werewolf generated code ---"

cc werewolf.c -o ww

./ww land.c -js land.js -p land.py -j land.java -r land.rb

cc land.c -o land

./land

rm land

echo "--- werewolf compile javasscipt code ---"

node land.js

echo "--- werewolf compile python code ---"

python land.py

echo "--- werewolf compile java code ---"

javac land.java
java land

rm land.class

echo "--- werewolf compile ruby code ---"

ruby land.rb
