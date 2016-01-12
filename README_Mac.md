#Installation auf dem Mac

## Pakete nachladen

```
$ brew install gettext
$ brew install fltk
$ brew tap acornejo/quartz
```

## src/Makefile editieren

Line 6:

```
CPPFLAGS = -Wall -Wextra -ggdb `fltk-config --use-cairo --cxxflags` -D USE_X11 -D FLTK_HAVE_CAIRO
```

ändern zu

```
CPPFLAGS = -Wall -Wextra -ggdb `fltk-config --use-cairo --cxxflags` -D USE_X11 -D FLTK_HAVE_CAIRO -I/usr/local/opt/cairo-quartz/include/ -I/opt/X11/include/  -I/usr/local/Cellar/gettext/0.19.6/include/
```