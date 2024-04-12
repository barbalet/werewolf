#!/bin/bash
#    regression.sh
#
#	=============================================================
#
#   Copyright 1996-2024 Tom Barbalet. All rights reserved.
#
#   Permission is hereby granted, free of charge, to any person
#   obtaining a copy of this software and associated documentation
#   files (the "Software"), to deal in the Software without
#   restriction, including without limitation the rights to use,
#   copy, modify, merge, publish, distribute, sublicense, and/or
#   sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following
#   conditions:
#
#   The above copyright notice and this permission notice shall be
#	included in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#   OTHER DEALINGS IN THE SOFTWARE.
#
#   This software is a continuing work of Tom Barbalet, begun on
#   13 June 1996. No apes or cats were harmed in the writing of
#   this software.

echo "--- ggcc generated code ---"

cc werewolf.c -o ww

./ww satire.c -js satire_new.js -p satire_new.py -j satire_new.java
./ww satireArray.c -js satireArray_new.js -p satireArray_new.py -j satireArray_new.java
./ww satireReal.c -js satireReal_new.js -p satireReal_new.py -j satireReal_new.java

cc satire.c -o satire
cc satireArray.c -o satireArray
cc satireReal.c -o satireReal

./satire
./satireArray
./satireReal

rm satire
rm satireArray
rm satireReal

echo "--- ggcc compile javascipt code ---"

node satire_new.js
node satireArray_new.js
node satireReal_new.js

rm satire_new.js
rm satireArray_new.js
rm satireReal_new.js

echo "--- ggcc compile python code ---"

python satire_new.py
python satireArray_new.py
python satireReal_new.py

rm satire_new.py
rm satireArray_new.py
rm satireReal_new.py

echo "--- ggcc compile java code ---"

javac satire_new.java
java satire_new

rm satire_new.class

javac satireArray_new.java
java satireArray_new

rm satireArray_new.class

javac satireReal_new.java
java satireReal_new

rm satireReal_new.class

rm satire_new.java
rm satireArray_new.java
rm satireReal_new.java
