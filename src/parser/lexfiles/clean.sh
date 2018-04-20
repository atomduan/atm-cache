#!/bin/bash -
set -e
if [ -f lex.yy.c ]; then
    rm lex.yy.c
fi

if [ -f pr ]; then
    rm pr 
fi
