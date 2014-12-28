#include <iostream>
#include <fstream>
#include "ModelHuff.h"

ModelHuff::ModelHuff(fstream &modelfile)
{
	SetModel(&modelfile);						//construction with filename
	SetTree();
	SetTable();
	modelfile.close();
}

ModelHuff::ModelHuff(string s)
{
	fstream ModelFile;
	ModelFile.open(s,ios::in|ios::binary);
	if(!ModelFile.is_open())
	{
		cout<<"Open model file for encoder FAILURE!"<<endl;
		throw "Model file does not exist!\n";
	}
	SetModel(&ModelFile);						//construction with filename
	SetTree();
	SetTable();
	ModelFile.close();
}

void ModelHuff::SetModel(fstream *modelfile)
{
	while(modelfile->peek()!=EOF)
	{
		modelfile->read((char *)(&buffer),sizeof(buffer));
		HuffTree[buffer].weight+=1;			
	}
}

void ModelHuff::SetTree()
{
	int low1,low2;
	for(int k=N;k<NN;k++)
	{
		Select(low1,low2,k);
		HuffTree[low1].parent=k;
		HuffTree[low2].parent=k;
		HuffTree[k].weight=HuffTree[low1].weight+HuffTree[low2].weight;
		HuffTree[k].lchild=low1;
		HuffTree[k].rchild=low2;
	}
}

void ModelHuff::Select(int &low1,int &low2,int k)
{
	int i=0;
	while(HuffTree[i].parent != -1) i++;
	low1=i++;
	while(HuffTree[i].parent != -1) i++;
	low2=i++;
	if(HuffTree[low1].weight>HuffTree[low2].weight)
	{
		int temp=low1;
		low1=low2;
		low2=temp;
	}
	for(;i<k;i++)
	{
		if(HuffTree[i].parent == -1)
		{
			if(HuffTree[i].weight<HuffTree[low1].weight)
			{
				low2=low1;
				low1=i;
			}
			else if(HuffTree[i].weight<HuffTree[low2].weight)
			{
				 low2=i;
			}
		}
	}
}

void ModelHuff::SetTable()
{
	StackNode Stack[N+1];
	int top=-1,bt=NN-1;
	do
	{
		do
		{
			++top;
			Stack[top].ptr=bt;
			Stack[top].flag=0;
			bt=HuffTree[bt].lchild;
		} while (bt!=-1);
		if(top!=-1)				// 或许可以省
		{
			for(int i=0;i<top;i++)
			{
				HuffTable[Stack[top].ptr].tab[i]=Stack[i].flag;
			}
			Stack[top].flag=1;
		}
		while(top!=-1 && Stack[top].flag==1) --top;
		if(top!=-1)
		{
			Stack[top].flag=1;
			bt=HuffTree[Stack[top].ptr].rchild;
		}
	} while (top!=-1);
}

