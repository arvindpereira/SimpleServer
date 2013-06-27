SimpleServer
============

A very simple TCP/IP server which is intended to be used with the Verosim simulator. This will allow us to build a ROS interface for Verosim.

VerosimMoosInterface.cpp is an example program which shows how the simple TCP_Server can be extended to 
allow fairly nice ways of talking to multiple clients.

The ultimate goal of this interface is to act as a relay between various simulators for exchange of data. All serialization is being done using NMEA-like strings. There is no real error-checking going on here and that should also be added to make this more robust.

I have also included example code for how callbacks can be setup using objects.
