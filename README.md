# TTT_certify
Software for TTT (torque tool tester)

## Dependencies

* FLTK, cairo, sqlite3, libusb 1.0 (http://www.libusb.org/), libconfuse
* GNU Octave if you want to run the simulation tests

## Build

This project doesn't use autotools yet. Just "cd src && make".
Currently tested on GNU/Linux Debian Jessie, crosscompile using MXE
or windoze build using MSYS2 and Mingw64.

## libusb on Windows

IMPORTANT: NEC/Renesas uPD720200/uPD720200A USB 3.0 users, please upgrade your drivers to version 2.1.16.0 or later. uPD720201/uPD720202 users should use version 3.x (3.0.23 or later preferred). Older versions of the driver have a bug that prevents libusb from accessing devices. 

https://github.com/libusb/libusb/wiki/Windows#How_to_use_libusb_on_Windows
