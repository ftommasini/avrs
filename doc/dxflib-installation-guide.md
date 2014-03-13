dxflib-3.3.4
------------

It is necessary 'qmake':

sudo aptitude install qt4-qmake 

Create makefile and compile:

qmake dxflib.pro
make

This creates the file 'libdxflib.a' 

To compile a dynamic version of dxflib, run 'make shared' instead of 'make'.

Now you can also copy the header files into a header directory of your choice 

sudo mkdir /usr/local/include/dxflib
sudo cp src/*.h /usr/local/include/dxflib


and copy the library files into a library directory of your choice.


sudo cp libdxflib.a /usr/local/lib/

