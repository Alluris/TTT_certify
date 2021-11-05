# TTT_certify
Small GUI for PC communication with Alluris TTT (torque tool tester) devices. Includes user- and device administration as well as calibration in ISO-6789 norm.

## Binaries

[Installer for 32/64-bit MS-Windows](https://alluris.de/de/produkte/software/TTT-300)

## Dependencies

* FLTK, cairo, sqlite3, libusb 1.0 (http://www.libusb.org/), libconfuse
* GNU Octave if you want to run the simulation tests

## GNU/Linux

## Debian GNU/Linux bullseye
```
sudo apt install libfltk1.3-dev libfltk-cairo1.3
```

### optional: Build FLTK from source

build FLTK from source if your distro doesn't ship FLTK with support for cairo.

```
./configure --enable-cairo --enable-shared
```

fltk1.3.3 has a bug: http://www.fltk.org/str.php?L3156
You might try the snapshot where it's fixed if you ran into this problem or patch it yourself.

### TTT_certify

This project doesn't use autotools yet. Just "cd src && make".
Currently tested on GNU/Linux Debian Jessie.

### USB access permissions

If you get the error "access denied" you may create a udev rule to set permissions.
In the example below anyone in the group "plugdev" can access the device:

```
$ cat /etc/udev/rules.d/90-alluris-usb.rules
ACTION=="add", ATTRS{idVendor}=="04d8", ATTRS{idProduct}=="f25e", MODE="0660", OWNER="root", GROUP="plugdev"
```

## Windows 7 and newer

### Caveat

IMPORTANT: NEC/Renesas uPD720200/uPD720200A USB 3.0 users, please upgrade your drivers to version 2.1.16.0 or later. uPD720201/uPD720202 users should use version 3.x (3.0.23 or later preferred). Older versions of the driver have a bug that prevents libusb from accessing devices. 

See https://github.com/libusb/libusb/wiki/Windows#How_to_use_libusb_on_Windows

### Build using MSYS2 (recommendation)
Download and install MSYS2 from https://msys2.github.io/ (tested with https://github.com/msys2/msys2-installer/releases/download/2021-07-25/msys2-x86_64-20210725.exe
)

In a mingw32 shell (`C:\msys64\mingw32.exe`):

```
pacman -Su
pacman -S mingw32/mingw-w64-i686-cairo mingw32/mingw-w64-i686-gcc git tar make man pkg-config mingw32/mingw-w64-i686-libusb mingw32/mingw-w64-i686-sqlite3 mingw32/mingw-w64-i686-confuse
```

### FLTK
#### Download
```
wget https://www.fltk.org/pub/fltk/1.3.7/fltk-1.3.7-source.tar.gz
```
#### Verify tarball
```
md5sum.exe fltk-1.3.7-source.tar.gz
8483e58b2312a348c0f66101eead1409  fltk-1.3.7-source.tar.gz
```
#### Build
```
tar xzf fltk-1.3.7-source.tar.gz
cd fltk-1.3.7/
./configure --enable-cairo
make -j6
make install
```

### TTT_certify
```
git clone https://github.com/Alluris/TTT_certify.git
cd TTT_certify/src
make -f Makefile.msys2
```

## Mac OSX 10.9 and newer

### Install dependencies using homebrew

```
brew install gettext confuse
brew tap acornejo/quartz
brew install cairo-quartz
```

### FLTK

```
wget http://fltk.org/pub/fltk/1.3.7/fltk-1.3.7-source.tar.gz
tar xzf fltk-1.3.7-source.tar.gz
cd fltk-1.3.7-source

export PKG_CONFIG_PATH=/usr/local/Cellar/cairo-quartz/1.10.2/lib/pkgconfig
export LDFLAGS=-L/usr/local/opt/cairo-quartz/lib

./configure --enable-cairo
make
sudo make install
```

### Build TTT certify

** edit CPPFLAGS and LDFLAGS in TTT_certify/src/Makefile **

See instructions in Makefile

```
cd src
make
```
