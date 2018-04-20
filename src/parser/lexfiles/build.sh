#!/bin/bash -
set -e
if [ -f lex.yy.c ]; then
    rm lex.yy.c
fi

if [ -f pr ]; then
    rm pr 
fi

lex dummy.lex
gcc lex.yy.c -o pr -ll
./pr sample.txt
