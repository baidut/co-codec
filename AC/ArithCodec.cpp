#include <iostream>
#include <fstream>
#include "ArithCodec.h"

ArithCodec::ArithCodec(fstream *file)
{
	mFile=file;
	mBitCount=0;
	mBitBuffer=0;
	mLow=0;
	mHigh=g_Whole-1;						//prevent overflow at the first operation
	mStep=0;
	mScale=0;
}

int ArithCodec::Encode(ArithCodec &EnOutput,ModelArith &Mod)
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

void ArithCodec::Decode(ArithCodec &DeOutput,ModelArith &Mod,unsigned int n)
{
	Decode_Init();
	for(unsigned int i=0;i<n;i++)
		Decode_Char(DeOutput,Mod);
}

void ArithCodec::SetFile(fstream *file)
{
	mFile=file;
}

char ArithCodec::GetChar()
{
	mFile->read((char *)(&mBitBuffer),sizeof(mBitBuffer));
	return mBitBuffer;
}

void ArithCodec::SetBit(const char bit)
{
	mBitBuffer=(mBitBuffer<<1)|bit;			//add bit to the buffer
	if(++mBitCount==8)						//buffer full and write to the file
	{
		mBitCount=0;
		mFile->write((char *)(&mBitBuffer),sizeof(mBitBuffer));
	}
}

void ArithCodec::SetBitFlush()
{
	while(mBitCount!=0)						//write the last few code to the file
		SetBit(0);
}

char ArithCodec::GetBit()
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

void ArithCodec::Encode_Init()
{
	mLow=0;								
	mHigh=g_Whole-1;
	mStep=0;
	mScale=0;
	mBuffer=0;
	mBitBuffer=0;
	mBitCount=0;
}

void ArithCodec::Encode_Char(ArithCodec &EnOutput,ModelArith &Mod)
{
	char enc=GetChar();						//get symbol from the file
	int m=Mod.char2int(enc);
	mStep=(mHigh-mLow+1)/Mod.total();
	mHigh=mLow+mStep*Mod.high_count(m)-1;
	mLow=mLow+mStep*Mod.low_count(m);
	while( mHigh<g_Half || mLow>=g_Half)	//E1 and E2 scaling
	{
		if(mHigh<g_Half)
		{
			EnOutput.SetBit(0);
			mLow=mLow*2;
			mHigh=mHigh*2+1;
			for(;mScale>0;mScale--)
				EnOutput.SetBit(1);
		}
		else
		{
			EnOutput.SetBit(1);
			mLow=(mLow-g_Half)*2;
			mHigh=(mHigh-g_Half)*2+1;
			for(;mScale>0;mScale--)
				EnOutput.SetBit(0);
		}
	}
	while( mLow>=g_FirstQuarter && mHigh<g_ThirdQuarter)	//E3 scaling
	{
		++mScale;
		mLow=(mLow-g_FirstQuarter)*2;
		mHigh=(mHigh-g_FirstQuarter)*2+1;
	}
}

void ArithCodec::Encode_End(ArithCodec &EnOutput)
{
	if(mLow<=g_FirstQuarter)				//add the end bit
	{
		EnOutput.SetBit(0);
		EnOutput.SetBit(1);
		for(;mScale>0;mScale--)				
			EnOutput.SetBit(1);
	}
	else
	{
		EnOutput.SetBit(1);
	}
	EnOutput.SetBitFlush();
}

void ArithCodec::Decode_Init()
{
	mLow=0;
	mHigh=g_Whole-1;
	mStep=0;
	mScale=0;
	mBuffer=0;
	mBitBuffer=0;
	mBitCount=0;
	for(int i=0;i<mBuffer_bits-1;i++)
	{
		mBuffer= (mBuffer<<1) | GetBit();
	}
}

void ArithCodec::Decode_Char(ArithCodec &DeOutput,ModelArith &Mod)
{
	mStep=(mHigh-mLow+1)/Mod.total();
	char dec=Mod.int2char((mBuffer-mLow)/mStep);
	//cout<<dec;
	int m=Mod.char2int(dec);
	DeOutput.mFile->write(&dec,1);
	mHigh=mLow+Mod.high_count(m)*mStep-1;
	mLow=mLow+Mod.low_count(m)*mStep;
	while( mHigh<g_Half || mLow>=g_Half )	//E1 and E2 scaling
	{
		if(mHigh<g_Half)
		{
			mLow=mLow*2;
			mHigh=mHigh*2+1;
			mBuffer=(mBuffer<<1) | GetBit();
		}
		else
		{
			mLow=(mLow-g_Half)*2;
			mHigh=(mHigh-g_Half)*2+1;
			mBuffer=(mBuffer-g_Half)<<1 | GetBit();
		}
	}
	while(mLow>=g_FirstQuarter && mHigh<=g_ThirdQuarter)	//E3 scaling
	{
		mLow=(mLow-g_FirstQuarter)*2;
		mHigh=(mHigh-g_FirstQuarter)*2+1;
		mBuffer=(mBuffer-g_FirstQuarter)<<1 | GetBit();
	}
}

