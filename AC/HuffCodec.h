#ifndef _HUFFCODEC_H_
#define _HUFFCODEC_H_

#include <iostream>
#include <fstream>
#include "Parameter.h"
#include "ModelHuff.h"
using namespace std;

class HuffCodec
{
public:
	HuffCodec(fstream *file=nullptr);
	int Encode(HuffCodec &EnOutput,ModelHuff &Mod);
	void Decode(HuffCodec &DeOutput,ModelHuff &Mod,unsigned int n);
	void SetFile(fstream *file);
	char GetChar();
protected:
	//bit/byte operation 
	void SetBit(char bit);
	void SetBitFlush();
	char GetBit();	
	
	//encode and decode
	void Encode_Init();
	void HuffCodec::Encode_Char(HuffCodec &EnOutput,ModelHuff &Mod);
	void Encode_End(HuffCodec &EnOutFile);
	void Decode_Init();
	void HuffCodec::Decode_Char(HuffCodec &DeOutput,ModelHuff &Mod);
	//parameter
	fstream *mFile;
	unsigned char mBitBuffer;
	unsigned char mBitCount;
};

#endif