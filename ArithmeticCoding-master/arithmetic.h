#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#define HALF_CHAR_BITS          (4)
#define CHAR_BITS               (8)
#define SHORT_BITS              (CHAR_BITS << (sizeof(short)>>1))
#define INT_BITS                (CHAR_BITS << (sizeof(int)>>1))

#define HIGHEST_BIT(x)          ((x) & (1<<((sizeof(x)<<3)-1)))
#define SECOND_HIGHEST_BIT(x)   ((x) & (1<<((sizeof(x)<<3)-2)))
#define LOWEST_BIT(x)           ((x) & 0x01)

#define TAGS_TYPE_STATIC        (1<<1)
#define TAGS_TYPE_DYNAMIC       (1<<2)
#define TAGS_TYPE_CHAR          (1<<4)
#define TAGS_TYPE_SHORT         (1<<5)
#define TAGS_TYPE_INT           (1<<6)
#define TAGS_TAG                'A'

#define max(x, y)               ((x) > (y) ? (x) : (y))
#define min(x, y)               ((x) < (y) ? (x) : (y))
#define bool2int(x)             ((x) ? 1 : 0)
#define edge(x, y)              ((x)/(y) + bool2int((x)%(y)))
/*#define edge(x, y)              ((x)/(y) + (((x)%(y)) ? 1 : 0))*/

struct tags {
        unsigned char tag;              // 'A' is short for arithmetic coding
        unsigned char type;             // static coding or dynamic coding OR ?
        unsigned char lastoutbits;      // last out put *magic number bits* (1-8)
        unsigned char elements;         // number of char in the src file(0 means 256)
        unsigned int magic;             // the magic number in the final
        unsigned int filesize;          // src file size
};

#define TAGS_SIZE               sizeof(struct tags)

struct com {
        unsigned char outbits;
        unsigned int high;
        unsigned int low;
        unsigned int currsize; // file size
        unsigned int outbytes;
        unsigned int offset;
};

extern int compression(const char *outfile, char *src, unsigned int length);
extern int decompression(const char *outfile, const char *infile);

/*
                               arithmetic file arch

----------------------------------------------------------------------------------
|          tags            |              12 bytes (TAGS_SIZE)                   |
|--------------------------+------------------------------------------------------
|    elements descriptor   | static (as tags.elements say) or dynamic : 32 bytes |
|--------------------------+------------------------------------------------------
|  file body (magic body)  |      file length decrease the total bytes above     |
----------------------------------------------------------------------------------
 
*/ 

#endif // arithmetic.h
