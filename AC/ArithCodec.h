#ifndef _ARITHCODEC_H_
#define _ARITHCODEC_H_

#include <iostream>
#include <fstream>
#include "Parameter.h"
#include "ModelArith.h"
using namespace std;

const unsigned int mBuffer_bits=sizeof(int)*8;
const unsigned int g_Whole=0x80000000;
const unsigned int g_Half=g_Whole/2;
const unsigned int g_FirstQuarter=g_Half/2;
const unsigned int g_ThirdQuarter=g_Half+g_ThirdQuarter;

class ArithCodec
{
public:
	ArithCodec(fstream *file=NULL/*nullptr*/);
	int Encode(ArithCodec &EnOutput,ModelArith &Mod);
	void Decode(ArithCodec &DeOutput,ModelArith &Mod,unsigned int n);
	void SetFile(fstream *file);
	char GetChar();
protected:
	//bit/byte operation
	void SetBit(const char bit);
	void SetBitFlush();
	char GetBit();

	//encode and decode
	void Encode_Init();
	void Encode_Char(ArithCodec &EnOutput,ModelArith &Mod);
	void Encode_End(ArithCodec &EnOutFile);
	void Decode_Init();
	void Decode_Char(ArithCodec &DeOutput,ModelArith &Mod);
	//parameter
	fstream *mFile;
	unsigned int mLow;
	unsigned int mHigh;
	unsigned int mStep;
	unsigned int mScale;
	unsigned int mBuffer;
	unsigned char mBitBuffer;
	unsigned char mBitCount;
};

#endif
