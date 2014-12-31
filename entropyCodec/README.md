
About FiniteStateEntropy
========================

from  https://github.com/Cyan4973/FiniteStateEntropy

Copyright (C) Yann Collet 2013-2014

A new approach of entropy codec, combining speed of Huffman coding with compression rate of arithmetic coding, see also [rANS](https://github.com/rygorous/ryg_rans) 

 * compare to
	 * **[zlib](http://www.zlib.net/)** using [deflate](http://en.wikipedia.org/wiki/DEFLATE): Huffman + LZ77 algorithm [Benchmarks results of FSE vs zlib](https://github.com/Cyan4973/FiniteStateEntropy/tree/master/benchmarkResults) 
	 * arithmetic Coding - better compression rate and speed.

The license of FSE library is BSD.
 * fse.c 
 * fse.h 
 * fseU16.c 
 * fseU16.c 

 User program of the FSE library (GPL v2 License)
 * ProbaGenerator.c - Demo program creating sample file with controlled probabilies
 * Fuzzer - Automated test program for FSE
 * fullbench.c - Demo program to benchmark open-source compression algorithm
 * commandline.c - simple command line interface manager, for FSE (GPLv2 license)
	 * `fse.exe -z inputFilename -o outputFilename`



About ArithmeticCoding
======================

from  https://github.com/lzhang10/maxent


Looking for other implement of Arithmetic coding?
==================================================

nclack/arithcode https://github.com/nclack/arithcode    encode and decode string, file io not finished. 



  
 