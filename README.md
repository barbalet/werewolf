# WereWolf - One Language Turns into Three

The WereWolf or ``ww`` is a translator between a version of C which compiles through ``cc`` to:

*	JavaScript which compiles through ``node``, and/or,
*	Python which compiles through ``python``
*	Java which compiles through ``javac/java``

This is currently a work in progress. More documentation coming shortly!

## Command Line Syntax ##

Inside the ``satire`` directory, compile ggcc.c:

``cc werewolf.c -o ww``

``./ww satire.c -p satire_new.py -js satire_new.js -j satire_new.java`` generates the anticipated output files

``./ww satire.c -p satire_new.py -js satire_new.js -j satire_new.java -n`` generates the anticipated output files without printing anything out the console or adding the main (which can be used for unit testing)

Alternatively also single languages can be used. For example if you only want Java output:

``./ww satire.c -j satire_new.java``

## For More Information ##

``barbalet at gmail dot com``