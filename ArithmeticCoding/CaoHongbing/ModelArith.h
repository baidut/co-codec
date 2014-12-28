#ifndef		_MODELARITH_H_
#define		_MODELARITH_H_

#include <iostream>
#include <fstream>
#include "Parameter.h"
using namespace std;

class ModelArith
{
public:
	ModelArith(){}
	ModelArith(fstream *modelfile);
	ModelArith(string s);						//construction whit filename
	void SetModel(fstream *modelfile);
	unsigned int total();
	unsigned int low_count(unsigned int n);
	unsigned int high_count(unsigned int n);
	unsigned int char2int(char s);
	char int2char(unsigned int n);
protected:
	unsigned int mCumCount[N+1];	
	unsigned int mtotal;
	char buffer;
};

#endif