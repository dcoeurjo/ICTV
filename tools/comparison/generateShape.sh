#!/bin/zsh

# Example : ./launchMe.sh cube18.raw 18 32

LD_LIBRARY_PATH=/usr/local/lib volgen cube -len 2 -v cube4.vol
vol2raw cube4
