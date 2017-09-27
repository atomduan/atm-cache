#!/usr/bin/env python

import sys
from ctypes import *

def native_call(lpath, arglist):
    args=[]
    for arg in arglist:
        args.append(c_char_p(arg));
    lib = CDLL(lpath);
    cargs = (c_char_p * len(args))(*args)
    buf = lib.native_bar(len(args), cargs);
    return cast(buf,c_char_p).value

if __name__ == '__main__':
    args = ['abc', 'xyz']
    res = native_call("./module_bar.so",args);
    print "python get res is : " + res + ", " + str(len(res))
