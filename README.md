# TTT_certify
Software for TTT (torque tool tester)

## Dependencies

* FLTK, cairo, sqlite3, libusb 1.0 (http://www.libusb.org/), libconfuse
* GNU Octave if you want to run the simulation tests

## Build

This project doesn't use autotools yet. Just "cd src && make".
Currently tested on GNU/Linux Debian Jessie, crosscompile using MXE
or windoze build using MSYS2 and Mingw64.
