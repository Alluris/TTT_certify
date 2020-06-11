.PHONY:all clean style cppcheck

all:
	$(MAKE) -C src $@
	$(MAKE) -C test $@

clean:
	$(MAKE) -C src $@
	$(MAKE) -C test $@

style:
	find . \( -name "*.m" -or -name "*.c" -or -name "*.cpp" -or -name "*.h" -or -name "Makefile" \) -exec sed -i 's/[[:space:]]*$$//' {} \;
	find . \( -name "*.c" -or -name "*.cpp" -or -name "*.h" \) -exec astyle --style=gnu -s2 -n {} \;

cppcheck:
	#cppcheck --language=c++ --enable=all src/*c src/*.h test/*.c
	cppcheck -q --language=c++ --enable=warning src/*c src/*.h test/*.c
	#cppcheck -q --language=c++ --enable=warning,performance,portability src/*c src/*.h test/*.c
