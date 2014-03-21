dxflib Installation Guide
=========================
(for version 3.3.4)


by **Fabián C. Tommasini** 

*Last update: Mar 13, 2014*


Pre-requisites
--------------

It is necessary to have installed [`qmake`](http://qt-project.org/doc/qt-4.8/qmake-manual.html):

	$ sudo apt-get install qt4-qmake 


Steps
-----

Download dxflib 3.3.4 from [here](http://www.qcad.org/en/dxflib-downloads).

Unpack the file:

	$ tar xfz dxflib-3.3.4-src.tar.gz

Create makefile and compiling:

	$ cd dxflib-3.3.4-src
	$ qmake dxflib.pro
	$ make

This creates the file `libdxflib.a`.

> *Note*: To compile a dynamic version of dxflib, run `make shared` instead of `make`.

Now you can also copy the header files into a header directory of your choice (e.g. in `/usr/local/include/`):

	$ sudo mkdir /usr/local/include/dxflib
	$ sudo cp src/*.h /usr/local/include/dxflib

Then copy the library file into a library directory of your choice (e.g. in `/usr/local/lib/`).

	$ sudo cp libdxflib.a /usr/local/lib/

