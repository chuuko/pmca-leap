#!/bin/bash

xrdb -load .Xdefaults

LD_LIBRARY_PATH=./ python3.4 ./PMCA_pymain.py
#LD_LIBRARY_PATH=./ ./test.py

#LD_LIBRARY_PATH=./ ./PMCA PMCA_pymain.py
