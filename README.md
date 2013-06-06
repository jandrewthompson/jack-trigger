jack-trigger
============

A jack client app that listenes on an audio port for transients.  When a threshold is crossed, a midi event is sent on a (jack) midi output port.

This is very useful for things like triggering a kick drum in Hydrogen from an audio signal in Ardour, for instance.

To Compile:

    gcc -o trigger jacktrigger.c `pkg-config --cflags --libs jack`

To Run:

    ./trigger KickTrigger 29 30 0.5
