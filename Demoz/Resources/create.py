import os
import sys

name = sys.argv[1]
os.mkdir(name)
os.chdir(name)
os.mkdir("Output")
os.mkdir("Shaders")
os.mkdir("Resource")