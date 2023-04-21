#!/bin/env python

import os

try:
    os.mkdir("build")
except:
    pass
os.chdir("build")
os.system("cmake ..")
os.system("cmake --build .")