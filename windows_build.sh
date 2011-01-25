#!/bin/sh

# build TCHunt in MingW on Vista or XP

echo 'Enter binary name: '
read binary

windres gui.rc -o gui.o

g++ -static -Os gui.o `fltk-config --cxxflags` gTCHunt.cpp `fltk-config --ldstaticflags` -o $binary \
-Ic://Boost/include/ \
c://Boost/lib/libboost_filesystem-s.a \
c://Boost/lib/libboost_system-s.a \
c://Boost/lib/libboost_date_time-s.a \
c://Boost/lib/libboost_thread-mt-s.a

## May need to copy /mingw/lib/libpthread.dll.a to /mingw/lib/libpthread.a... figure it out.

strip -s $binary

upx --best $binary

echo 'built' $binary
