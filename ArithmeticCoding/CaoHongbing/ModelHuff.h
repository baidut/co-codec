#ifndef		_MODELHUFF_H_
#define		_MODELHUFF_H_

#include <iostream>
#include <fstream>
#include "Parameter.h"
using namespace std;

class HuffNode
{
public:
	friend class ModelHuff;
	HuffNode(){weight=0;lchild=rchild=parent=-1;}
	int Get_parent(){return parent;}
	int Get_lchild(){return lchild;}
	int Get_rchild(){return rchild;}
protected:
	int weight;
	int parent;
	int lchild,rchild;
};

class HuffTableNode 
{
public:
	friend class ModelHuff;
	HuffTableNode(){for(int i=0;i<N;i++) tab[i]=-1;}
	char GetBit(int n){return tab[n];}
protected:
	char tab[N];
};

class StackNode
{
public:
	StackNode(){ptr=0;flag=-1;}
	int ptr;
	char flag;
};

class ModelHuff
{
public:
	friend class HuffCodec;
	ModelHuff(){}
	ModelHuff(fstream &modelfile);
	ModelHuff(string s);						//construction whit filename	
protected:
	void SetModel(fstream *modelfile);
	void Select(int &low1,int &low2,int k);
	void SetTree();
	void SetTable();
	HuffNode HuffTree[NN];	
	HuffTableNode HuffTable[N];			//ʵ�����ֻ��n-1���룬���������ӵĻ����������һ�����ж�
	char buffer;
};

#endif