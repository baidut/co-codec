JPEG2000 Standard implementation
================================

Jasper
------

Jasper IS NOT JasperReports！

Stripped Jasper from https://github.com/vmarkovtsev/jasper ,for original Jasper, go to https://github.com/smanders/jasper

[中文介绍-Jasper-JPEG2000的实现(1)](http://blog.csdn.net/windcsn/article/details/556834) 此博客还有更多相关的资料

OpenJPEG
--------

> OpenJPEG is an open-source library to encode and decode JPEG 2000 images. As of version 2.1 released in April 2014, it is officially conformant with the JPEG 2000 Part-1 standard. -- from [wikipedia/OpenJPEG](http://en.wikipedia.org/wiki/OpenJPEG)

[OpenJPEG homepage](http://www.openjpeg.org/)

TortoiseSVN：一个非svn文件夹下右键svn->checkout, 输入网址http://openjpeg.googlecode.com/svn/trunk/ openjpeg-read-only 下载不下来的话可以考虑下载gihtub上的镜像文件https://github.com/OpenJPEG/openjpeg

JPEG Standard implementation
============================

libjpeg is a popular library supporting the JPEG image compression.

Important notice: you can now choose between two alternative implementations:
 * libjpeg is the standard, plain library
 * libjpeg-turbo is a new library, that fully takes profit from the most recent Intel/AMD CPUs
If you are planning to deploy your software on such platforms, then using libjpeg-turbo can ensure a 200% performance boost (and even more than this).
I strongly recommend using libjpeg-turbo: both libraries share the same identical API/ABI (they are absolutely inter-changeable), but libjpeg-turbo runs in an impressively faster mode.

jpeg-6b & jpeg-9a 
-----------------

written in C, author:Tom Lane, Guido Vollbeding, Philip Gladstone,
Bill Allombert, Jim Boucher, Lee Crocker, Bob Friesenhahn, Ben Jackson,
Julian Minguillon, Luis Ortiz, George Phillips, Davide Rossi, Ge' Weijers,
and other members of the Independent JPEG Group

 * jpeg-6b : version 6b of 27-Mar-1998, from http://libjpeg.sourceforge.net/
 * jpeg-9a : version is release 9a of 19-Jan-2014, from [IJG](http://www.ijg.org/)

 > libjpeg is a widely used free library with functions for handling the image data format JPEG. It implements a JPEG codec (encoding and decoding) alongside various utilities for handling JPEG data. It is written in C and distributed as free software together with its source code under the terms of a custom permissive (BSD-like) free software license, which basically demands attribution. The original variant is maintained and published by the Independent JPEG Group (IJG). Meanwhile, there are several mostly compatible forks with additional features like for example increased coding efficiency.
	——from [wikipedia libjpeg](http://en.wikipedia.org/wiki/Libjpeg)

 > [IJG(Independent JPEG Group)](http://www.ijg.org/) is an informal group that writes and distributes a widely used free library for JPEG image compression. 

 related links
 * http://www.freedesktop.org/wiki/Software/libjpeg/
 * http://www.infai.org/jpeg/
 

libjpeg-master
--------------

written in C++, author:[Thomas Richter](https://www.researchgate.net/researcher/70636586_Thomas_Richter),from https://github.com/thorfdbg/libjpeg

> [jpeg-ls](http://www.libjpeg-turbo.org/About/SmartScale-Lossless)
For comparison, Thomas Richter's JPEG-LS implementation, hereafter referred to as "jpeg-ls", was used to compress the source images into the JPEG-LS format. 

how to install
 1. install g++ & autoconf `sudo apt-get install g++` `sudo apt-get install autoconf`
 1. `./configure`
 1. `make`
 1. `make install`


libjpeg-turbo
--------------

it's famous!

an ABI-/API-compatible fork of libjpeg that uses x86 SIMD instructions to achieve a substantial speedup over the reference implementation.

SIMD-accelerated libjpeg-compatible JPEG codec library

related links
 * http://libjpeg-turbo.virtualgl.org/
 * [An optimized libjpeg replacement](https://launchpad.net/libjpeg-turbo)
 * http://www.freedesktop.org/wiki/Software/libjpeg/
 * [download libjpeg-turbo](http://sourceforge.net/projects/libjpeg-turbo/) 


OTHERS
======

[matlab implementation](http://www.mathworks.com/matlabcentral/fileexchange/10476-jpeg-codec)