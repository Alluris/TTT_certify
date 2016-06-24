#!/bin/bash

## Zum manuellen vergleichen
#egrep -A1 'msgid "[^a-zA-Z0-9"]' $1

egrep -A1 'msgid "[^a-zA-Z0-9"]' $1 | awk '$1~/msgid/{id=$2} $1~/msgstr/{if (substr (id, 1, 2)!=substr ($2, 1, 2)) {print id; print $2; print "---"}}'
