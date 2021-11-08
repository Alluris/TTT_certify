#!/bin/bash

fn=../test/create_test_signal/test_object_id$1.log
echo "Running ttt_gui in simulation mode with id = $1"

if [ -e $fn ]; then
  ./ttt_gui $1 $fn
else
  echo "File $fn not found! Aborting..."
fi


