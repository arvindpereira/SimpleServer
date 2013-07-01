SimpleServer
============

A very simple TCP/IP server which is intended to be used with the Verosim simulator. This will allow us to build a ROS interface for Verosim.

VerosimMoosInterface.cpp is an example program which shows how the simple TCP_Server can be extended to 
allow fairly nice ways of talking to multiple clients.

The ultimate goal of this interface is to act as a relay between various simulators for exchange of data. All serialization is being done using NMEA-like strings. There is no real error-checking going on here and that should also be added to make this more robust.

I have also included example code for how callbacks can be setup using objects.

This project depends upon my library ArvindsTools. Please download that project first, modify the Makefile in this project
so that it points to the location of ArvindsTools (for the included headers) and finally copy (or install) the lib_arvinds_tools.so
file into the directory of this project.

The recommended way to do so is to put ArvindsTools at the same level as SimpleServer. Compile them separately and simply copy over the file lib_arvinds_tools.so into your directory. You should not have to modify ArvindsTools at all for the most part (if I designed it correctly).

The documentation can be found here: http://robotics.usc.edu/~ampereir/SimpleServer/docs/html/index.html
