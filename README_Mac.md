#Installation auf dem Mac

## Pakete nachladen

$ brew install gettext confuse
$ brew tap acornejo/quartz
$ brew install cairo-quartz

## FLTK von Source kompilieren
$ wget http://fltk.org/pub/fltk/1.3.3/fltk-1.3.3-source.tar.gz
$ tar xzf fltk-1.3.3-source.tar.gz
$ cd fltk-1.3.3-source

$ export PKG_CONFIG_PATH=/usr/local/Cellar/cairo-quartz/1.10.2/lib/pkgconfig
$ export LDFLAGS=-L/usr/local/opt/cairo-quartz/lib

$ ./configure --enable-cairo
$ make
$ sudo make install

## TTT certify kompilieren
$ cd src

### 1. Makefile editieren (TTT_certify/src/Makefile)
GNU/Linux CPPFLAGS & LDFLAGS auskommentieren (Zeile 4&5)
Mac OS X CPPFLAGS & LDFLAGS einkommentieren (Zeile 8&9)

### 2. Kompilieren
$ make

## TTT GUI ausführen
$ ./ttt_gui
