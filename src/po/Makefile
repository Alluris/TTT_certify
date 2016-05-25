.PHONY: all
.PHONY: clean

all: ttt.pot en.mo fr.mo

../ttt_gui.cpp:
	make -C ../ ttt_gui.cpp

## update sources with grep -lR gettext
ttt.pot: ../step.cpp ../ttt.cpp ../ttt_gui.cpp ../ttt_gui_main.cpp ../cairo_print_devices.c ../ttt_quick_check_config.cpp ../test_object_table.h ../test_person_table.h ../liballuris++.cpp
	$(MAKE) -C ../ ttt_gui.cpp
	$(MAKE) -C ../ ttt_quick_check_config.cpp
	xgettext -C -c --from-code=UTF-8 --package-name=TTT --msgid-bugs-address=software@alluris.de $^ -o $@

## call it pot so the translator has to rename it to .po
## this should prevent accidentally overwrites
## automatically merge in old translations

%.pot: ttt.pot
	msginit -i $< -o $@
	msgmerge $(subst .pot,.po,$@) $@ > $@_merged

%.mo: %.po
	mkdir -p $(basename $@)/LC_MESSAGES
	msgfmt -cv $< -o $(basename $@)/LC_MESSAGES/ttt.mo

clean:
	rm -f *.mo
	rm -rf en fr

## try poedit!
