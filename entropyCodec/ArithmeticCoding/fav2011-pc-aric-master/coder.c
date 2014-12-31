#include <stdio.h>
#include <stdlib.h>
#include <string.h>             /* for memset */
#include <stddef.h> /* for size_t */
#include <stdint.h>
#include <assert.h>
#include "coder.h"
#include "header.h"


static uint32_t high = INTERVAL_MAX;
static uint32_t low  = INTERVAL_MIN;

static uint32_t code;

static void flush_word()
{
    size_t i;
    (void) putc((int) (high >> SHIFT), out);
    for (i = 0; i < sizeof(code) - 1; i++)
        (void) putc(0, out);
    coded_bytes += sizeof(code);

    high =  INTERVAL_MAX;
    low  =  INTERVAL_MIN;
}

static void read_word()
{
	size_t i;
    code = 0;
    for (i = 0; i < sizeof(code); i++)
        code = code << 8 | getc(in);
    orig_bytes += sizeof(code);

    high = INTERVAL_MAX;
    low = INTERVAL_MIN;
}

static void copy(FILE * in, FILE * out)
{
    int c;
    while ((c = getc(in)) != EOF) {
        orig_bytes++;
        coded_bytes++;
        (void) putc(c, out);
    }
}

static void encode(int symbol)
{
    uint64_t range = high - low;

    if (range <= PRECISION_LIMIT) {
        flush_word();
        range = high - low;
    }

    high = low + (range * freq[symbol + 1] >> 16) - 1;
    low  = low + (range * freq[symbol] >> 16) + 1;

    assert(low <= high);

    // write identical leading bytes
    while ((low ^ high) <= MASK) {
        (void) putc((int) (low >> SHIFT), out);
        coded_bytes++;

        low <<= 8;
        high = high << 8 | 0xFF;
    }
}

static int decode()
{
    uint64_t range = high - low;
	int symbol_freq, symbol, c;

    if (range <= PRECISION_LIMIT) {
        read_word();
        range = high - low;
    }

    // range must be larger than 2**W to prevent rounding error
    symbol_freq = (int) ((((uint64_t) (code - low)) << W) / range);
    symbol = (int) inverse[symbol_freq];

    // END of archive reached, decoding was successful
    if (symbol == CODE_EOF) {
        orig_bytes++;
        return CODE_EOF;
    }

    (void) putc(symbol, out);
    coded_bytes++;

    high = low + (range * freq[symbol + 1] >> 16) - 1;
    low  = low + (range * freq[symbol] >> 16) + 1;

    if (low > code || code > high)
        return FAILURE;

    // write identical leading bytes
    while ((low ^ high) <= MASK) {
        orig_bytes++;
        if ((c = getc(in)) == EOF) 
            return FAILURE;
        code = code << 8 | c;

        high = high << 8 | 0xFF;
        low <<= 8;
    }
    return SUCCESS;
}


//  Try to compress file. If the coded file is larger in size than source,
//  just copy over the source file and prepend NOT_ENCODED flag.
void compress()
{
    int c;
	// Set not coded/coded flag
    (void) putc(ENCODED, out);
    coded_bytes++;

    write_header();

    // encoding loop
    while ((c = getc(in)) != EOF) {
        orig_bytes++;
        encode(c);
    }
    encode(CODE_EOF);

    // write last 4 bytes
    flush_word();

    // if encoded file larger than source, don't encode
    if (coded_bytes > orig_bytes) {
        if ((out = freopen(out_filename, "wb", out)) == NULL) {
			perror("could not open file");
			return;
		}

        rewind(in);

		// set not coded/coded flag
        (void) putc(NOT_ENCODED, out);
        orig_bytes = 0;
		// not coded/coded flag is 1 byte
        coded_bytes = 1;

		// copy over
        copy(in, out);
    }
}


// Tries to decompress archive.
int decompress()
{
    int c;
    c = getc(in);
    orig_bytes++;

    if (c == NOT_ENCODED) {
        copy(in, out);
        return SUCCESS;
    }

    inverse = (uint16_t *) malloc(sizeof(uint16_t) * 0x10000);
	if (!inverse) {
		perror ("memory error");
		return FAILURE;
	}

    read_header();
    read_word();                /* initialize buffer */

    while ((c = decode()) != CODE_EOF)
        if (c == FAILURE) {
            free(inverse);
            fprintf(stderr, CORRUPTED);
            return FAILURE;
        }

    free(inverse);
    return SUCCESS;
}
