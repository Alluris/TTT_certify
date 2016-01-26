.PHONY:style

style:
	find . \( -name "*.m" -or -name "*.c" -or -name "*.cpp" -or -name "*.h" -or -name "Makefile" \) -exec sed -i 's/[[:space:]]*$$//' {} \;
	find . \( -name "*.c" -or -name "*.cpp" -or -name "*.h" \) -exec astyle --style=gnu -s2 -n {} \;
