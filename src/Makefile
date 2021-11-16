.PHONY:clean screenshots

TARGETS = ttt_gui.cpp ttt_gui.h ttt_gui ttt_certify.db ttt_quick_check_config.cpp ttt_quick_check_config.h ttt_quick_check_config ttt_param_check.cpp ttt_param_check.h ttt_param_check

## for GNU/Linux
CXXFLAGS = -Wall -Wextra -ggdb `fltk-config --use-cairo --cxxflags` -D USE_X11 -D FLTK_HAVE_CAIRO
LDFLAGS = `fltk-config --use-cairo --ldflags` -lusb-1.0 -lsqlite3 -lconfuse

## for MacOSX
#CXXFLAGS = -Wall -Wextra -ggdb `fltk-config --use-cairo --cxxflags` -I/usr/local/opt/gettext/include -I/usr/local/opt/cairo-quartz/include
#LDFLAGS = -L /usr/local/opt/gettext/lib `fltk-config --use-cairo --ldflags` -lusb-1.0 -lsqlite3 -lcairo -lconfuse -lintl

ARCH = $(shell uname -m)
# sanitize explizit nur auf x86 und damit NICHT auf ARM (z.B. RPi)
ifeq ($(ARCH),x86_64)
  CXXFLAGS += -fsanitize=address
endif

all: $(TARGETS)

ttt_gui.cpp ttt_gui.h: ttt_gui.f
	fluid -o .cpp -c $<

%.o:%.cpp %.h
	$(CXX) $(CXXFLAGS) -c $<

%.o:%.c %.h
	$(CXX) $(CXXFLAGS) -c $<

ttt_gui: ttt_gui.o ttt_gui_main.o measurement_table.o ttt.o ttt_device.o step.o lang.o sqlite_interface.o cairo_box.o cairo_device_box.o cairo_drawing_functions.o cairo_print_devices.o liballuris++.o liballuris.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

ttt_certify.db: create_database.sql fill_database_debug.sql
	sqlite3 ttt_certify.db < $(word 1,$^)
	sqlite3 ttt_certify.db < $(word 2,$^)

ttt_quick_check_config.cpp ttt_quick_check_config.h: ttt_quick_check_config.f
	fluid -o .cpp -c $<

ttt_quick_check_config: ttt_quick_check_config.o quick_check_table.o lang.o liballuris++.o liballuris.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

ttt_param_check.cpp ttt_param_check.h: ttt_param_check.f
	fluid -o .cpp -c $<

ttt_param_check: ttt_param_check.o cairo_plot.o cairo_box.o ttt_peak_detector.o ttt_device.o lang.o liballuris++.o liballuris.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

ttt_quick_check_title = 'TTT_Quick-Check V1.04.002'
ttt_param_check_title = 'TTT_Parameter-Check V1.04.002 Alluris GmbH & Co. KG, Basler Str. 65 , 79100 Freiburg, software@alluris.de'
ttt_gui_title = 'TTT_Certify V1.04.002 Alluris GmbH & Co. KG, Basler Str. 65 , 79100 Freiburg, software@alluris.de'

screenshots: $(TARGETS)
	mkdir -p screenshots
	./shot.sh de_DE.utf8 ./ttt_quick_check_config $(ttt_quick_check_title)
	./shot.sh en_US.utf8 ./ttt_quick_check_config $(ttt_quick_check_title)
	./shot.sh fr_FR.utf8 ./ttt_quick_check_config $(ttt_quick_check_title)
	./shot.sh it_IT.utf8 ./ttt_quick_check_config $(ttt_quick_check_title)
	./shot.sh es_ES.utf8 ./ttt_quick_check_config $(ttt_quick_check_title)
	./shot.sh de_DE.utf8 ./ttt_param_check $(ttt_param_check_title)
	./shot.sh en_US.utf8 ./ttt_param_check $(ttt_param_check_title)
	./shot.sh fr_FR.utf8 ./ttt_param_check $(ttt_param_check_title)
	./shot.sh it_IT.utf8 ./ttt_param_check $(ttt_param_check_title)
	./shot.sh es_ES.utf8 ./ttt_param_check $(ttt_param_check_title)
	./shot.sh de_DE.utf8 ./ttt_gui $(ttt_gui_title)
	./shot.sh en_US.utf8 ./ttt_gui $(ttt_gui_title)
	./shot.sh fr_FR.utf8 ./ttt_gui $(ttt_gui_title)
	./shot.sh it_IT.utf8 ./ttt_gui $(ttt_gui_title)
	./shot.sh es_ES.utf8 ./ttt_gui $(ttt_gui_title)

clean:
	find . -name "octave-workspace" -exec rm {} \;
	rm -f $(TARGETS)
	rm -f *.log *.o
	rm -rf screenshots

