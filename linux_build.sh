#!/bin/sh

echo 'Enter binary name: '
read binary

# build TCHunt on Ubuntu Linux or Centos

g++ -Os `fltk-config --cxxflags` gTCHunt.cpp `fltk-config --ldstaticflags` -o $binary \
-I/usr/local/include/boost/ \
/usr/local/lib/libboost_filesystem-s.a \
/usr/local/lib/libboost_system-s.a \
/usr/local/lib/libboost_date_time-s.a \
/usr/local/lib/libboost_thread-mt-s.a

strip -s $binary

upx --best $binary

echo 'built' $binary
