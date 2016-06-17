.PHONY: all clean update

all: ttt.pot en.mo fr.mo it.mo es.mo

../ttt_gui.cpp:
	make -C ../ ttt_gui.cpp

## update sources with grep -lR gettext
ttt.pot: ../step.cpp ../ttt.cpp ../ttt_gui.f ../ttt_gui_main.cpp ../cairo_print_devices.c ../ttt_quick_check_config.f ../test_object_table.h ../test_person_table.h ../liballuris++.cpp ../ttt_param_check.f
	$(MAKE) -C ../ ttt_gui.cpp
	$(MAKE) -C ../ ttt_quick_check_config.cpp
	$(MAKE) -C ../ ttt_param_check.cpp
	xgettext -C -c --from-code=UTF-8 --package-name=TTT --msgid-bugs-address=software@alluris.de $(patsubst %.f,%.cpp,$^) -o $@

## call it pot so the translator has to rename it to .po
## this should prevent accidentally overwrites
## automatically merge in old translations

%.pot: ttt.pot
	msginit -i $< -o $@
	msgmerge $(subst .pot,.po,$@) $@ > $@_merged

%.mo: %.po
	mkdir -p $(basename $@)/LC_MESSAGES
	msgfmt -cv $< -o $(basename $@)/LC_MESSAGES/ttt.mo

update: ttt.pot
	msgmerge -U en.po ttt.pot
	msgmerge -U fr.po ttt.pot
	msgmerge -U it.po ttt.pot
	msgmerge -U es.po ttt.pot

clean:
	rm -f *.mo *po~ ttt.pot
	rm -rf en fr it es
