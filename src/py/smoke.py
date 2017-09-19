#!/usr/bin/env python
from ctypes import *
args=[]
args.append(c_char_p("abc"));
args.append(c_char_p("xyz"));

res = ""
cresp = (c_char_p * len(res))(*res)

lib = CDLL("./module_bar.so");
cargs = (c_char_p * len(args))(*args)
lib.native_bar(len(args), cargs, cresp);

print cast(cresp,POINTER(POINTER(c_char_p)))
