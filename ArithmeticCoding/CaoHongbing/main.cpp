#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "HuffCodec.h"
#include "ArithCodec.h"
#include "ModelHuff.h"
#include "ModelArith.h"
#include <stdlib.h>
using namespace std;


int Encode(string enc_in,string enc_out,ModelHuff &Mod)
{
	fstream EnInFile,EnOutFile;

	EnInFile.open(enc_in,ios::in | ios::binary);
	if(!EnInFile.is_open())
	{
		throw "Open input file for encoder FAILURE!\n";
	}

	EnOutFile.open(enc_out,ios::out | ios::binary);

	HuffCodec EncodeIn(&EnInFile),EncodeOut(&EnOutFile);

	int num=EncodeIn.Encode(EncodeOut,Mod);			//count the number of symbol

	EnInFile.close();
	EnOutFile.close();

	return num;
}

int Encode(string enc_in,string enc_out,ModelArith &Mod)
{
	fstream EnInFile,EnOutFile;

	EnInFile.open(enc_in,ios::in | ios::binary);
	if(!EnInFile.is_open())
	{
		throw "Open input file for encoder FAILURE!\n";
	}

	EnOutFile.open(enc_out,ios::out | ios::binary);

	ArithCodec EncodeIn(&EnInFile),EncodeOut(&EnOutFile);

	int num=EncodeIn.Encode(EncodeOut,Mod);			//count the number of symbol

	EnInFile.close();
	EnOutFile.close();

	return num;
}

void Decode(string dec_in,string  dec_out,ModelHuff &Mod,unsigned int num)
{
	fstream DeInFile,DeOutFile;

	DeInFile.open(dec_in,ios::in | ios::binary);
	if(!DeInFile.is_open())
	{
		throw "Open input file FAILURE!";
	}

	DeOutFile.open(dec_out,ios::out | ios::binary);

	HuffCodec DecodeIn(&DeInFile),DecodeOut(&DeOutFile);


	DecodeIn.Decode(DecodeOut,Mod,num);

	DeInFile.close();
	DeOutFile.close();
}

void Decode(string dec_in,string  dec_out,ModelArith &Mod,unsigned int num)
{
	fstream DeInFile,DeOutFile;

	DeInFile.open(dec_in,ios::in | ios::binary);
	if(!DeInFile.is_open())
	{
		throw "Open input file FAILURE!";
	}

	DeOutFile.open(dec_out,ios::out | ios::binary);

	ArithCodec DecodeIn(&DeInFile),DecodeOut(&DeOutFile);


	DecodeIn.Decode(DecodeOut,Mod,num);

	DeInFile.close();
	DeOutFile.close();
}

double Compare(string enc_in,string enc_out,string dec_out)
{
	int flag=0;
	fstream EnInFile,EnOutFile,DeOutFile;

	EnInFile.open(enc_in,ios::in | ios::binary);
	if(!EnInFile.is_open())
	{
		throw "Open encode input file FAILURE!";
	}

	EnOutFile.open(enc_out,ios::in | ios::binary);
	if(!EnInFile.is_open())
	{
		throw "Open encode output file FAILURE!";
	}

	DeOutFile.open(dec_out,ios::in | ios::binary);
	if(!DeOutFile.is_open())
	{
		throw "Open decode output file FAILURE!";
	}


	EnInFile.seekg(0,ios::end);
	EnOutFile.seekg(0,ios::end);
	DeOutFile.seekg(0,ios::end);

	int length_EncIn=EnInFile.tellg();
	int	length_EncOut=EnOutFile.tellg();
	int length_Dec_Out=DeOutFile.tellg();

	if(length_EncIn!=length_Dec_Out)
	{
		EnInFile.close();
		DeOutFile.close();
		throw "Compress Failure\n";
	}

	while(EnInFile.peek()!=EOF)
	{
		char temp1,temp2;
		EnInFile.read((char *)(&temp1),sizeof(temp1));
		DeOutFile.read((char *)(&temp2),sizeof(temp2));
		if(temp1!=temp2)
		{
			EnInFile.close();
			DeOutFile.close();
			throw "Compress Failure\n";
		}
	}

	EnInFile.close();
	DeOutFile.close();

	double compress_rate=(double)length_EncOut/(double)length_EncIn;
	return compress_rate;
}

double Code_Model(string enc_in,ModelArith &Mod)
{
	string enc_out="_EnOut_Arith_"+enc_in;
	string dec_out="_DeOut_Arith_"+enc_in;
	int num=Encode(enc_in,enc_out,Mod);
	Decode(enc_out,dec_out,Mod,num);
	double compress_rate=Compare(enc_in,enc_out,dec_out);
	return compress_rate;
}

double Code_Model(string enc_in,ModelHuff &Mod)
{
	string enc_out="_EnOut_Huffm_"+enc_in;
	string dec_out="_DeOut_Huffm"+enc_in;
	int num=Encode(enc_in,enc_out,Mod);
	Decode(enc_out,dec_out,Mod,num);
	double compress_rate=Compare(enc_in,enc_out,dec_out);
	return compress_rate;
}


string file[5][5]={
					{"Life1.txt","Life2.txt","Life3.txt","Life4.txt","Life5.txt"},
					{"Sports1.txt","Sports2.txt","Sports3.txt","Sports4.txt","Sports5.txt"},
					{"Market1.txt","Market2.txt","Market3.txt","Market4.txt","Market5.txt"},
					{"Culture1.txt","Culture2.txt","Culture3.txt","Culture4.txt","Culture5.txt"},
					{"Medecine1.txt","Medecine2.txt","Medecine3.txt","Medecine4.txt","Medecine5.txt"},
				  };
#if 1//1
#define DISPLAY(...)         fprintf(stderr, __VA_ARGS__)
#define DISPLAYLEVEL(l, ...) if (displayLevel>=l) { DISPLAY(__VA_ARGS__); }
static char* programName;
static int   displayLevel = 2;
// 0 : no display  // 1: errors  // 2 : + result + interaction + warnings ;  // 3 : + progression;  // 4 : + information
static int   fse_pause = 0;

static int usage(void)
{
    DISPLAY("Usage :\n");
    DISPLAY("%s [arg] inputFilename [-o [outputFilename]]\n", programName);
    DISPLAY("Arguments :\n");
    DISPLAY(" -1 : using arithmetic\n");
    DISPLAY(" -2 : using huffman\n");
    DISPLAY(" -d : decompression\n");
    DISPLAY(" -h/-H : display help/long help and exit\n");
    return 0;
}
static int badusage(void)
{
    DISPLAYLEVEL(1, "Incorrect parameters\n");
    if (displayLevel >= 1) usage();
    exit(1);
}

int main(){

     int   i,
          forceCompress=0, decode=0, bench=3, benchLZ4e=0; // default action if no argument
    int   algoNb = -1;
    int   indexFileNames=0;
    char* input_filename=0;
    char* output_filename=0;
    int   nextNameIsOutput = 0;

    // Welcome message
    programName = argv[0];
    DISPLAY(WELCOME_MESSAGE);

    if (argc<1) badusage();

    for(i = 1; i <= argc; i++)
    {
        char* argument = argv[i];
        nextNameIsOutput --;

        if(!argument) continue;   // Protection if argument empty

        // Decode command (note : aggregated commands are allowed)
        if (argument[0]=='-')
        {
            // '-' means stdin/stdout
            if (argument[1]==0)
            {
                if (!input_filename) input_filename=stdinmark;
                else output_filename=stdoutmark;
            }

            while (argument[1]!=0)
            {
                argument ++;

                switch(argument[0])
                {
                    // Display help
                case 'V': DISPLAY(WELCOME_MESSAGE); return 0;   // Version
                case 'h':
                case 'H': usage(); return 0;

                    // compression
                case 'o': bench=0; nextNameIsOutput=2; break;

                    // Decoding
                case 'd': decode=1; bench=0; break;

                    // Benchmark full mode
                case 'b': bench=1; break;
                    // zlib Benchmark mode
                case 'z':
                    bench=1;
                    BMK_SetByteCompressor(3);
                    break;

                    // Verbose mode
                case 'v': displayLevel=4; break;

                    // Quiet mode
                case 'q': displayLevel--; break;

                    // Pause at the end (hidden option)
                case 'p': fse_pause=1; break;

                    // Unrecognised command
                default : badusage();
                }
            }
            continue;
        }

        // following -o argument
        if (nextNameIsOutput == 1) { output_filename=argument; continue; }

        // first provided filename is input
        if (!input_filename) { input_filename=argument; indexFileNames=i; continue; }
    }

    // End of command line reading
    DISPLAYLEVEL(3, WELCOME_MESSAGE);

    // No input filename ==> use stdin
    if(!input_filename) badusage();

    // Check if benchmark is selected
    if (bench==1) { ModelArith Mod(input_filename);}
    if (bench==3) { ModelHuff Mod(input_filename);}

    if (decode) {
            num = sscanf(input_filename,"[%d]")
            Decode(input_filename,output_filename,Mod,num);
    }
    else Encode(input_filename,output_filename,Mod);


    num

    							//create model for Encoding
    Huffm_Rate[i][j]=Code_Model(file[i][j],Mod);
        string enc_out="_EnOut_Arith_"+enc_in;
        string dec_out="_DeOut_Arith_"+enc_in;
        int num=Encode(enc_in,enc_out,Mod);




_end:
    if (fse_pause) waitEnter();
    return 0;
}
#else

int main()
{
	double Arith_Rate[5][5],Huffm_Rate[5][5];

	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			ModelArith Mod(file[i][j]);							//create model for Encoding
			Arith_Rate[i][j]=Code_Model(file[i][j],Mod);
		}
	}

	for(int i=0;i<5;i++)
	{
		for(int j=0;j<5;j++)
		{
			ModelHuff Mod(file[i][j]);							//create model for Encoding
			Huffm_Rate[i][j]=Code_Model(file[i][j],Mod);
		}
	}

	cout<<"Compression Ratio of Arithmetic Coding and Huffman Coding:"<<endl<<endl;
	double average=0;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			double diff=Huffm_Rate[i][j]-Arith_Rate[i][j];
			average += diff;
			cout<<"File: "<<setiosflags(ios::left)<<setfill(' ')<<setw(15)<<"\""+file[i][j]+"\"";
			cout<<"    Huffm_Rate= "<<setiosflags(ios::fixed)<<setprecision(1)<<Huffm_Rate[i][j]*100<<"%";
			cout<<"    Arith_Rate= "<<setiosflags(ios::fixed)<<setprecision(1)<<Arith_Rate[i][j]*100<<"%";
			cout<<"    D="<<setiosflags(ios::right)<<setfill(' ')<<setw(4)<<setiosflags(ios::fixed)<<setprecision(1)<<diff*100<<"%"<<endl;
		}
	}
	average/=25;
	cout<<endl<<"The average advantage of Arithmetic Coding to Huffman Coding = ";
	cout<<setiosflags(ios::fixed)<<setprecision(1)<<average*100<<"%."<<endl<<endl;
	system("pause");
}
#endif

