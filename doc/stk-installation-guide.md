STK Installation Guide
=========================
(for version 4.4.4)


by **Fabián C. Tommasini** 

*Last update: Apr 9, 2014*


Pre-requisites
--------------

It is necessary to have installed ALSA library:

	$ sudo apt-get install libasound2-dev


Steps
-----

Download STK.

Unpack the file:

	$ tar xfz stk-4.4.4.tar.gz

Configure and compiling:

	$ cd stk-4.4.4
	$ ./configure
	$ make

This creates the file `libstk.a` into `src` directory.

Now you can also copy the header files into a header directory of your choice (e.g. in `/usr/local/include/`):

	$ sudo mkdir /usr/local/include/stk
	$ sudo cp include/* /usr/local/include/stk

Then copy the library file into a library directory of your choice (e.g. in `/usr/local/lib/`).

	$ sudo cp src/libstk.a /usr/local/lib/

