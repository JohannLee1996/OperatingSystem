#!/bin/sh

# default is a C implementation
# Comment this out if in Java
./atsim $1 $2 $3 $4

# Java implementation
# Uncomment these line.
#SIM=ATCSim
#if [ ! -e $SIM.class ]; then
#  javac $SIM.java
#fi
#java $SIM $1 $2 $3 $4

