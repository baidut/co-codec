#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "coder.h"

#define IS_NAN(x) ((x) != (x))
#define IS_INF(x) ((x) - (x) != 0)

#define USAGE "usage: aric -c FILE COMPRESSED\n       aric -d COMPRESSED FILE"
#define MEGA 1000000


FILE *in, *out;
char *out_filename;

// keep track of written and read bytes 
int coded_bytes = 0;
int orig_bytes = 0;

static bool open_files(const char *read, const char *write)
{
    in = fopen(read, "rb");
    out = fopen(write, "wb");
    if (!in || !out) {
        perror("could not open file");
        return false;
    }
    return true;
}

static void close_files()
{
    if (in)
        (void) fclose(in);
    if (out)
        (void) fclose(out);
}

int main(int argc, char **argv)
{
    char mode_flag;
    clock_t start_time;
    double megabytes;
    double seconds;

    if (argc != 4 || strlen(argv[1]) != 2 || argv[1][0] != '-') {
        printf("%s", USAGE);
        return EXIT_FAILURE;
    }

    if (!open_files(argv[2], argv[3]))
        return EXIT_FAILURE;

    mode_flag = argv[1][1];
    start_time = clock();

    switch (mode_flag) {
    case 'c':
        out_filename = argv[3];
        compress();
        megabytes = (double) (orig_bytes / MEGA);
        break;

    case 'd':
        if (decompress() == FAILURE)
            return EXIT_FAILURE;

        megabytes = (double) (coded_bytes / MEGA);
        break;

    default:
        fprintf(stderr, "%s\n", USAGE);
        return EXIT_FAILURE;
    }

    seconds = (double) (clock() - start_time) / CLOCKS_PER_SEC;
    megabytes /= seconds;

    fprintf(stderr, "%d bytes to %d bytes", orig_bytes, coded_bytes);

    if (!IS_NAN(megabytes) && !IS_INF(megabytes) && megabytes > 0)
        fprintf(stderr, ", %6.1f MB/s", megabytes);
    fprintf(stderr, "\n");


    close_files();
    return EXIT_SUCCESS;
}
