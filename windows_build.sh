#!/bin/sh

# build TCHunt in MingW on Windows Vista or Windows XP or Windows 7

echo 'Enter binary name: '
read binary

windres gui.rc -o gui.o

g++ -static -Os gui.o `fltk-config --cxxflags` TCHunt.cpp `fltk-config --ldstaticflags` -o $binary \
-Ic://Boost/include/ \
c://Boost/lib/libboost_filesystem-s.a \
c://Boost/lib/libboost_system-s.a \
c://Boost/lib/libboost_date_time-s.a \
c://Boost/lib/libboost_thread-mt-s.a

# Note: May need to copy /mingw/lib/libpthread.dll.a to /mingw/lib/libpthread.a... figure it out.

# Striping and packing are optional (makes the binary smaller)
strip -s $binary
upx --best $binary

echo 'built' $binary
