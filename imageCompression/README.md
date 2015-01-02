
jpeg-6b & jpeg-9a 
=================

written in C, 

 * jpeg-6b : version 6b of 27-Mar-1998, from http://libjpeg.sourceforge.net/
 * jpeg-9a : version is release 9a of 19-Jan-2014, from [IJG](http://www.ijg.org/)

 > libjpeg is a widely used free library with functions for handling the image data format JPEG. It implements a JPEG codec (encoding and decoding) alongside various utilities for handling JPEG data. It is written in C and distributed as free software together with its source code under the terms of a custom permissive (BSD-like) free software license, which basically demands attribution. The original variant is maintained and published by the Independent JPEG Group (IJG). Meanwhile, there are several mostly compatible forks with additional features like for example increased coding efficiency.
	——from [wikipedia libjpeg](http://en.wikipedia.org/wiki/Libjpeg)

 > [IJG(Independent JPEG Group)](http://www.ijg.org/) is an informal group that writes and distributes a widely used free library for JPEG image compression. 

libjpeg-master
==============

written in C++

from https://github.com/thorfdbg/libjpeg

how to install
--------------

 1. install g++ & autoconf `sudo apt-get install g++` `sudo apt-get install autoconf`
 1. `./configure`
 1. `make`
 1. `make install`


libjpeg-turbo
==============

it's famous!

an ABI-/API-compatible fork of libjpeg that uses x86 SIMD instructions to achieve a substantial speedup over the reference implementation.

SIMD-accelerated libjpeg-compatible JPEG codec library

related links
 * http://libjpeg-turbo.virtualgl.org/
 * [An optimized libjpeg replacement](https://launchpad.net/libjpeg-turbo)
 * http://www.freedesktop.org/wiki/Software/libjpeg/
