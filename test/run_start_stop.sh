#!/bin/bash

rm -f start_stop.exe
make start_stop 

for i in $(seq 1 100);
do
	./start_stop
	echo "run" $i "returned" $?
done