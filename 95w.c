#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <inttypes.h>

#ifndef TRUE
    #define TRUE    1
#endif
#ifndef FALSE
    #define FALSE   0
#endif

#ifdef ASSERT
    #undef ASSERT
#endif
#define ASSERT(x) \
    if (x) { \
        fprintf(stderr, \
            "Error: At line %d, assertation failed.\n", \
            __LINE__ \
        ); \
        exit(EXIT_FAILURE); \
    }

static long nanos(void)
{
    struct timespec ts;

    timespec_get(&ts, TIME_UTC);

    return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

/**
 * Apparently, you can use lehmer64 which uses a seed size of 128-bit
 * which however is useless in our case as we want a single number from
 * 0 to 9 and a normal xorshift with 64-bit size would be enough.
 * 
 * Lemire's blog: https://lemire.me/blog/2019/03/19/the-fastest-conventional-random-number-generator-that-can-pass-big-crush/
*/
static uint64_t xorshift64(uint64_t seed)
{
    seed ^= seed << 13;
    seed ^= seed >> 7;
    seed ^= seed << 17;

    return seed;
}

static void gen_cd_key(void)
{
    uint64_t v0, v1, v2, v3, v4, v5, v6, r;

    do {
        v0 = xorshift64((uint64_t)nanos()) % 9;
        v1 = xorshift64((uint64_t)nanos()) % 9;
        v2 = xorshift64((uint64_t)nanos()) % 9;
        v3 = xorshift64((uint64_t)nanos()) % 9;
        v4 = xorshift64((uint64_t)nanos()) % 9;
        v5 = xorshift64((uint64_t)nanos()) % 9;
        v6 = xorshift64((uint64_t)nanos()) % 9;
        r = v0 + v1 + v2 + v3 + v4 + v5 + v6;

        if (r % 7u == 0 && v3 <= 8 && v6 != 0 && v6 < 8 &&
            v0 != v1)
            break;
    } while (TRUE);

    fprintf(stdout, "CD key: %lu%lu%lu-%lu%lu%lu%lu%lu%lu%lu\n",
        v0, v1, v2, v0, v1, v2, v3, v4, v5, v6
    );
}

static void gen_nt_oem_key(int is_win95)
{
    uint64_t fsg, ysg, v0, v1, v2, v3, v4, v5, v6, r;

    fsg = (xorshift64(nanos()) % (366 - 1 + 1)) + 1;
    /* For Windows 95, the value in fact can be "02", "95", "96" and so on.
       I couldn't find any specification for this.
    */
    ysg = is_win95 ? 2 : (xorshift64(nanos()) % (95 - 3 + 1)) + 3;

    ASSERT(fsg <= 1 && fsg >= 366);

    if (!is_win95)
        ASSERT(ysg <= 3 && ysg >= 95);

    do {
        v0 = xorshift64((uint64_t)nanos()) % 9;
        v1 = xorshift64((uint64_t)nanos()) % 9;
        v2 = xorshift64((uint64_t)nanos()) % 9;
        v3 = xorshift64((uint64_t)nanos()) % 9;
        v4 = xorshift64((uint64_t)nanos()) % 9;
        v5 = xorshift64((uint64_t)nanos()) % 9;
        v6 = xorshift64((uint64_t)nanos()) % 9;
        r = v0 + v1 + v2 + v3 + v4 + v5 + v6;

        if (r % 7u == 0 && v0 == 0)
            break;
    } while (TRUE);

    fprintf(stdout, is_win95 ?
        "Windows 95 OEM key: " :
        "Windows NT/W OEM key: "
    );
    fprintf(stdout,
        "%.3lu%.2lu-OEM-%lu%lu%lu%lu%lu%lu%lu-%lu%lu%lu%lu%lu\n",
        fsg, ysg, v0, v1, v2, v3, v4, v5, v6, v0, v1, v2, v3, v4
    );
}

static void show_usage(void)
{
    fprintf(stdout,
        "95w - Generate CD/OEM keys for Windows 95 and NT\n\n"
        "Usage:\n"
        "   [-c|--cdkey]    - Generates a CD key\n"
        "   [-o|--95oem]    - Generates a Windows 95 OEM key\n"
        "   [-w|--ntwoem]   - Generates a Windows NT (Workstation) OEM key\n"
    );
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        show_usage();
        exit(EXIT_SUCCESS);
    }

    const char *opts;
    int c, fcdkey, f95oem, fntwoem, fhelp;

    fcdkey = f95oem = fntwoem = fhelp = 0;
    opts = "cow";

    struct option longopts[] = {
        { "gen", no_argument, &fcdkey, 1 },
        { "95oem", no_argument, &f95oem, 1 },
        { "ntwoem", no_argument, &fntwoem, 1 },
        { NULL, 0, NULL, 0 }
    };

    while ((c = getopt_long(argc, argv, opts, longopts, NULL)) != -1) {
        switch (c) {
        case 'c':
            fcdkey = 1;
            break;

        case 'o':
            f95oem = 1;
            break;

        case 'w':
            fntwoem = 1;
            break;

        case 'h':
            fhelp = 1;
            break;
        }
    }

    if (fcdkey)
        gen_cd_key();

    if (f95oem)
        gen_nt_oem_key(TRUE);

    if (fntwoem)
        gen_nt_oem_key(FALSE);

    if (fhelp)
        show_usage();
}
