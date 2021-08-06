#!/bin/sh
echo "Start, please Wait"
g++ $1 -o $2 `pkg-config --cflags --libs opencv`
echo "Finish, run C++ Project"
./$2
