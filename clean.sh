#!/bin/bash -
project_path=$(cd `dirname $(which $0)`; pwd)
cd $project_path
if [ -d "./smoke" ]; then
    rm -r ./smoke
fi

if [ -d "./build" ] || [ -f "./Makefile" ]; then
    make clean
fi

if [ -f "./tags" ]; then
    rm ./tags
fi
