# WereWolf - One Language Turns into Three

The WereWolf or ``ww`` is a translator between a version of C which compiles through ``cc`` to:

*	JavaScript which compiles through ``node``, and/or,
*	Python which compiles through ``python``
*	Java which compiles through ``javac/java``

For now, these three languages will be the only ones ported. To optimize and further pattern match the ``ww`` “satire” solution to effective cross-porting.

## Command Line Syntax ##

Inside the ``satire`` directory, compile werewolf.c:

``cc werewolf.c -o ww``

Then ``ww`` can be ran as follows:

``./ww satire.c -p satire_new.py -js satire_new.js -j satire_new.java`` generates the anticipated output files

``./ww satire.c -p satire_new.py -js satire_new.js -j satire_new.java -n`` generates the anticipated output files without printing anything out the console or adding the main (which can be used for unit testing)

Alternatively also single languages can be used. For example if you only want Java output:

``./ww satire.c -j satire_new.java``

## For More Information ##

``barbalet at gmail dot com``