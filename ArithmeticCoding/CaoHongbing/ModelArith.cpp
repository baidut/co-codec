#include <iostream>
#include <fstream>
#include "ModelArith.h"

ModelArith::ModelArith(fstream *modelfile)
{
	mCumCount[0]=0;
	for(int i=1;i<=N;i++)
		mCumCount[i]=1;							//at least 1 times
	while(modelfile->peek()!=EOF)
	{
		modelfile->read((char *)(&buffer),sizeof(buffer));
		mCumCount[buffer+1]+=2;					//accomulation
	}
	for(int i=1;i<=N;i++)						//count the distribution
		mCumCount[i]+=mCumCount[i-1];
	mtotal=mCumCount[N];
}

ModelArith::ModelArith(string s)
{
	fstream ModelFile;
	ModelFile.open(s,ios::in|ios::binary);
	if(!ModelFile.is_open())
	{
		cout<<"Open model file for encoder FAILURE!"<<endl;
		throw "Model file does not exist!\n";
	}
	SetModel(&ModelFile);						//construction with filename
	ModelFile.close();
}

void ModelArith::SetModel(fstream *modelfile)
{
	mCumCount[0]=0;
	for(int i=1;i<=N;i++)
		mCumCount[i]=1;
	while(modelfile->peek()!=EOF)
	{
		modelfile->read((char *)(&buffer),sizeof(buffer));
		mCumCount[buffer+1]+=2;			
	}
	for(int i=1;i<=N;i++)
		mCumCount[i]+=mCumCount[i-1];
	mtotal=mCumCount[N];	
}

unsigned int ModelArith::total()
{
	return mtotal;
}

unsigned int ModelArith::low_count(unsigned int n)
{
	return mCumCount[n];
}

unsigned int ModelArith::high_count(unsigned int n)
{
	return mCumCount[n+1];
}

unsigned int ModelArith::char2int(char s)
{
	return s;
}

char ModelArith::int2char(unsigned int k)
{
	int sLow=0,sHigh=N;				//bisearch for index of char
	int sMiddle=(sLow+sHigh)/2;
	while(sLow+1<sHigh)
	{
		if(k<mCumCount[sMiddle])
			sHigh=sMiddle;
		else if(k>mCumCount[sMiddle])
			sLow=sMiddle;
		else
			return sMiddle;
		sMiddle=(sLow+sHigh)/2;
	}
	return sLow;
} 