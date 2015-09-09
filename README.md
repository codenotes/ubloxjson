# ubloxjson
ublox test application whos code can ultimately be used to create a proper library for json conversion and simulated GPS.

At present this exe can:

1) Convert a binary ubx file (captured directly from a ublox device or from v-center) and convert to JSON format as a file or to the screen.*
2) Convert a JSON file to a binary ublox stream and write to a COM port, such that this application can be a stand-in or simulator 
for a ublox device.  A fake GPS basically, which is its intent. 

NOTES:
1)The purpose of the UBLOX JSON  format is for consumption by other environments like MATLAB, Mathematics (what I use), or any form of application.

2) Code from the pixhawk flight system is included for reference (ubloxgps.cpp).  Note that this application only cares about the ublox
messages that are consumed by the DIY Drones flight software/pixhawk.  Other message types are not processed.  These messages are:

POSLLH
SOL
STATUS
TP
VELNED

3) This code is sloppy and rough and not factored well.  But should be functional.  Is also win32 based (use of direct serial port,
via createfile and threading, etc.) Ultimately should be modified to use boost-libraries so that it is cross platform. That is a TODO

4) Should be further modified to be a library and not an executable. 

