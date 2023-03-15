import os
import subprocess

DIR = "/home/taiga/Desktop/U/mesher/"

test = subprocess.Popen(["cmake","../src/"], cwd=DIR+"build")
# output = test.communicate()[0]
# print(output)
test = subprocess.Popen(["make"], cwd=DIR+"build")
output = test.communicate()[0]
# print(output)
test = subprocess.Popen(["../build/mesher_roi"], cwd=DIR+"data")
output = test.communicate()[0]
# print(output)

test = subprocess.Popen(["../build/mesher_roi","-d","cortex.mdl","-s", "5", "-u", "cs5", "-m", "-v"], cwd=DIR+"data")
output = test.communicate()[0]
print(output)

process = subprocess.Popen(["../../view/build/viewer","-m","cs5.m3d"], cwd=DIR+"data",bufsize=0,
                           stdin=subprocess.PIPE,
                           stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE)

process.stdin.write(('%s\n' % "n").encode())

output = process.communicate()[0]
print(output)
