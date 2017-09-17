#!/usr/bin/env python
from ctypes import *
lib = CDLL("./module_bar.so");
lib.native_bar();
