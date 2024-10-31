#!/bin/bash

if [ "$(uname -s)" == "Darwin" ]; then
    g++ -std=c++14 -o main main.cpp \
        -isystem /opt/homebrew/Cellar/googletest/1.15.2/include \
        -L/opt/homebrew/Cellar/googletest/1.15.2/lib \
        -lgtest -lgtest_main -pthread
else
    g++ -o main main.cpp -lgtest -lgtest_main -pthread
fi
./main
