#!/bin/bash

#echo "$1" "$2" "$3"
LANG="$1"; "$2" &
#echo $LANG
pid=$!
tmp_fn=$(mktemp)
sleep 6
xwd -name "$3" -out $tmp_fn
kill $pid
convert $tmp_fn screenshots/$2_$1.png
rm $tmp_fn
