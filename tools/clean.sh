#! /bin/bash -
curr_path=$(cd `dirname $(which $0)`; pwd)
cd $curr_path/..

#remove the tail spaces of code
if [ -d src ]; then
    find src -type f | xargs -I{} sed -i 's/[ ]\{1,\}$//g' {}
fi
