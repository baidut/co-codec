#include <iostream>
#include <fstream>
#include "HuffCodec.h"


HuffCodec::HuffCodec(fstream *file)
{
	mFile=file;
	mBitCount=0;
	mBitBuffer=0;
}

int HuffCodec::Encode(HuffCodec &EnOutput,ModelHuff &Mod)
{
	int num=0;
	Encode_Init();
	while(mFile->peek()!=EOF)
	{
		num++;								//count the number of symbols
		Encode_Char(EnOutput,Mod);			//encode with model Mod
	}
	Encode_End(EnOutput);
	return num;
}

void HuffCodec::Decode(HuffCodec &DeOutput,ModelHuff &Mod,unsigned int n)
{
	Decode_Init();
	for(unsigned int i=0;i<n;i++)
		Decode_Char(DeOutput,Mod);
}

void HuffCodec::SetFile(fstream *file)
{
	mFile=file;
}

char HuffCodec::GetChar()
{
	mFile->read((char *)(&mBitBuffer),sizeof(mBitBuffer));
	return mBitBuffer;
}

void HuffCodec::SetBit(char bit)
{
	mBitBuffer=(mBitBuffer<<1)|bit;			//add bit to the buffer
	if(++mBitCount==8)						//buffer full and write to the file
	{
		mBitCount=0;
		mFile->write((char *)(&mBitBuffer),sizeof(mBitBuffer));
	}
}

void HuffCodec::SetBitFlush()
{
	while(mBitCount!=0)						//write the last few code to the file
		SetBit(0);
}

char HuffCodec::GetBit()
{
	if(mBitCount==0)						//buffer empty and read the file
	{
		if(mFile->peek()!=EOF)
			mFile->read((char *)(&mBitBuffer),sizeof(mBitBuffer));
		else
			mBitBuffer=0;					//append zeros
		mBitCount=8;
	}
	char mbit=(mBitBuffer & mBitBufferMSB)>>7;
	mBitBuffer<<=1;							//shift the bit out
	--mBitCount;
	return mbit;
}

void HuffCodec::Encode_Init()
{
	mBitBuffer=0;
	mBitCount=0;
}

void HuffCodec::Encode_Char(HuffCodec &EnOutput,ModelHuff &Mod)
{
	char enc=GetChar();						//get symbol from the file
	int i=0;
	char flag;
	while ( (flag=Mod.HuffTable[enc].GetBit(i))!=-1 )
	{
		EnOutput.SetBit(flag);
		i++;
	}
}

void HuffCodec::Encode_End(HuffCodec &EnOutput)
{
	EnOutput.SetBitFlush();
}

void HuffCodec::Decode_Init()
{
	mBitBuffer=0;
	mBitCount=0;
}

void HuffCodec::Decode_Char(HuffCodec &DeOutput,ModelHuff &Mod)
{
	int bt=NN-1;
	while(Mod.HuffTree[bt].Get_lchild() != -1 )
	{
		if(GetBit())
			bt=Mod.HuffTree[bt].Get_rchild();
		else
			bt=Mod.HuffTree[bt].Get_lchild();
	}
	char dec=char(bt);
	DeOutput.mFile->write(&dec,1);
}