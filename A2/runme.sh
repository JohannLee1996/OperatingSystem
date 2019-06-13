#!/bin/sh

# default is a C implementation
# Comment this out if in Java
./atsim $1 $2 $3 $4

# Java implementation
# Uncomment these line.
#XSIM=XSim
#if [ ! -e $XSIM.class ]; then
#  javac $XSIM.java
#fi
#java $XSIM $1 $2 $3 $4

