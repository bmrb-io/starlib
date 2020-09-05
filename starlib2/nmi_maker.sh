#!/bin/bash 
#
#   Assumes that the CVS checkout into starlib2 dir has already occurred.
#
cd starlib2
make clean
make 
cd examples
make clean
make
