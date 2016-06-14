.PHONY:clean

TARGETS = ttt_gui.cpp ttt_gui.h ttt_gui ttt_certify.db ttt_quick_check_config.cpp ttt_quick_check_config.h ttt_quick_check_config

## for GNU/Linux
CPPFLAGS = -Wall -Wextra -ggdb `fltk-config --use-cairo --cxxflags` -D USE_X11 -D FLTK_HAVE_CAIRO
LDFLAGS = `fltk-config --use-cairo --ldflags` -lusb-1.0 -lsqlite3 -lcairo -lconfuse

## for MacOSX
#CPPFLAGS = -Wall -Wextra -ggdb `fltk-config --use-cairo --cxxflags` -I/usr/local/opt/gettext/include -I/usr/local/opt/cairo-quartz/include
#LDFLAGS = -L /usr/local/opt/gettext/lib `fltk-config --use-cairo --ldflags` -lusb-1.0 -lsqlite3 -lcairo -lconfuse -lintl

all: $(TARGETS)

ttt_gui.cpp ttt_gui.h: ttt_gui.f
	fluid -o .cpp -c $<

%.o:%.cpp %.h
	g++ $(CPPFLAGS) -c $<

%.o:%.c %.h
	g++ $(CPPFLAGS) -c $<

ttt_gui: ttt_gui.o ttt_gui_main.o measurement_table.o ttt.o ttt_device.o step.o sqlite_interface.o cairo_box.o cairo_device_box.o cairo_drawing_functions.o cairo_print_devices.o liballuris++.o liballuris.o
	g++ $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

ttt_certify.db: create_database.sql fill_database.sql
	sqlite3 ttt_certify.db < $(word 1,$^)
	sqlite3 ttt_certify.db < $(word 2,$^)

ttt_quick_check_config.cpp ttt_quick_check_config.h: ttt_quick_check_config.f
	fluid -o .cpp -c $<

ttt_quick_check_config: ttt_quick_check_config.o quick_check_table.o liballuris++.o liballuris.o
	g++ $(CPPFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	find . -name "octave-workspace" -exec rm {} \;
	rm -f $(TARGETS)
	rm -f *.log *.o

