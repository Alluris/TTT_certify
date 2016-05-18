.PHONY:clean

CPPFLAGS = -Wall -Wextra -ggdb -I ../src/
GCC = g++

TARGETS = test_ttt_device ttt_certify.db ttt_cli ttt_sim check_sqlite_interface check_create_cairo_report lsusb-libusb check_ttt_step check_liballuris
OBJ     = ../src/ttt_device.o ../src/ttt.o ../src/measurement_table.o ../src/step.o ../src/sqlite_interface.o ../src/cairo_drawing_functions.o ../src/cairo_print_devices.o ../src/liballuris++.o ../src/liballuris.o
LIBS    = -lsqlite3 -lcairo -lusb-1.0 -lfltk -lconfuse

all: $(TARGETS)
	$(MAKE) -C create_test_signal

%.o:%.cpp %.h
	g++ $(CPPFLAGS) -c $< -o $@

%.o:%.c %.h
	g++ $(CPPFLAGS) -c $< -o $@

ttt_certify.db:
	sqlite3 ttt_certify.db < ../src/create_database.sql
	sqlite3 ttt_certify.db < ../src/fill_database_debug.sql

test_ttt_device: test_ttt_device.cpp $(OBJ)
	$(GCC) $(CPPFLAGS) $^ -o $@ $(LIBS)

ttt_cli: ttt_cli.cpp $(OBJ)
	$(GCC) $(CPPFLAGS) $^ -o $@ $(LIBS)

ttt_sim: ttt_sim.cpp $(OBJ)
	$(GCC) $(CPPFLAGS) $^ -o $@ $(LIBS)

check_sqlite_interface: check_sqlite_interface.cpp $(OBJ)
	$(GCC) $(CPPFLAGS) $^ -o $@ $(LIBS)

check_create_cairo_report: check_create_cairo_report.cpp $(OBJ)
	$(GCC) $(CPPFLAGS) $^ -o $@ $(LIBS)

check_ttt_step: check_ttt_step.cpp $(OBJ)
	$(GCC) $(CPPFLAGS) $^ -o $@ $(LIBS)

check_liballuris: check_liballuris++.cpp $(OBJ)
	$(GCC) $(CPPFLAGS) $^ -o $@ $(LIBS)

lsusb-libusb: lsusb-libusb.c
	gcc -g -o $@ $< -lm -lusb-1.0

TEST_FILES=test_object_id6.log test_object_id7.log test_object_id8.log test_object_id9.log\
           test_object_id10.log test_object_id11.log test_object_id12.log test_object_id13.log\
           test_object_id14.log test_object_id15.log test_object_id16.log test_object_id17.log\
           test_object_id18.log test_object_id19.log test_object_id20.log test_object_id21.log\
           test_object_id22.log

## you may run "make -j20 check"
## but this can cause problems with parallel writes to database
check: $(TEST_FILES)
	egrep "Except|failed|result: [^ ]|Database" *.log

%.log: ttt_sim
	./ttt_sim $(subst .log,,$(subst test_object_id,,$@)) ./create_test_signal/$@ 2>&1 | tee $@

clean:
	rm -f $(TARGETS) *.o *.pdf *.log
	rm -f ./logfiles/*
