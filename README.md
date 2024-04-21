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

## Origin of the Satire ##

The idea of the satire is central to WereWolf. It offers a subset of C, Python, JavaScript and Java that describes the majority of the functionality. It is a "satire" of the language.

It also provides a skeleton for language translation between the four languages versus the current crop of slightly substandard generative AI solutions. Thus it is also a satire of the current solution space.

There are a few other applicable uses for the term satire in creating WereWolf. For now, these should be enough to start some discussion.

## For More Information ##

``barbalet at gmail dot com``