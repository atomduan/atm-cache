#!/bin/bash -
project_path=$(cd `dirname $(which $0)`; pwd)
cd $project_path
if [ -d "./smoke" ]; then
    rm -r ./smoke
fi
make clean
