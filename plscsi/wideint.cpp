/**
**  wideint.cpp
**  Monday, July 8, 2002
**
**  Extend the C library to cope more often with wide int's.
**/

/**
**  Link with standard C libraries.
**/

#include <limits.h> /* INT_MAX ... */
#include <stdio.h> /* sprintf ... */
#include <stdlib.h> /* calloc ... */
#include <string.h> /* memset ... */

/**
**  Link with the *.c* of ../plscsi/.
**/

#include "plscsi.h"

#ifdef __MSDOS__

/**
**  Divide a wide length into the next of a series of thin lengths.
**/

static int TO_THIN_INT(UINT length)
    {
    int thinLength = (int) length;
    if (thinLength != length)
        {
        if (length < 0)
            {
            return INT_MIN;
            }
        return INT_MAX;
        }
    return thinLength;
    }

/**
**  Repeat fread for a wide length.
**/

UINT FREAD(void * to, UINT width, UINT length, FILE * fi)
    {
    UINT result = 0;

    int thinWidth = TO_THIN_INT(width);
    if (thinWidth != width)
        {
        return -1;
        }
    if (!(length <= (INT_MOST_POS / thinWidth)))
        {
        return -1;
        }

    while (0 < length)
        {
        int thinLength = TO_THIN_INT(length);
        int ioLength = fread(to, thinWidth, thinLength, fi);
        if (0 <= ioLength) result += ioLength;
        if (ioLength != thinLength) return result;
        to = TO_VOID_P(TO_INT(to) + ((INT) thinWidth * (INT) thinLength));
        length -= thinLength;
        }
    return result;
    }

/**
**  Repeat fwrite for a wide length.
**/

UINT FWRITE(void const * from, UINT width, UINT length, FILE * fi)
    {
    UINT result = 0;

    int thinWidth = TO_THIN_INT(width);
    if (thinWidth != width)
        {
        return -1;
        }
    if (!(length <= (INT_MOST_POS / thinWidth)))
        {
        return -1;
        }

    while (0 < length)
        {
        int thinLength = TO_THIN_INT(length);
        int ioLength = fwrite(from, thinWidth, thinLength, fi);
        if (0 <= ioLength) result += ioLength;
        if (ioLength != thinLength) return result;
        from = TO_VOID_P(TO_INT(from) + ((INT) thinWidth * (INT) thinLength));
        length -= thinLength;
        }
    return result;
    }

/**
**  Repeat memcmp for a wide length.
**/

int MEMCMP(void * to, void * from, UINT length)
    {
    int result = 0;
    while (0 < length)
        {
        int thinLength = TO_THIN_INT(length);
        result = memcmp(to, from, thinLength);
        if (result != 0)
            {
            return result;
            }
        to = TO_VOID_P(TO_INT(to) + thinLength);
        from = TO_VOID_P(TO_INT(from) + thinLength);
        length -= thinLength;
        }
    return result;
    }

/**
**  Repeat memmove for a wide length.
**/

void * MEMMOVE(void * to, void * from, UINT length)
    {
    void * result = to;
    while (0 < length)
        {
        int thinLength = TO_THIN_INT(length);
        memmove(to, from, thinLength);
        to = TO_VOID_P(TO_INT(to) + thinLength);
        from = TO_VOID_P(TO_INT(from) + thinLength);
        length -= thinLength;
        }
    return result;
    }

/**
**  Repeat memset for a wide length.
**/

void * MEMSET(void * to, char cc, UINT length)
    {
    void * result = to;
    while (0 < length)
        {
        int thinLength = TO_THIN_INT(length);
        memset(to, cc, thinLength);
        to = TO_VOID_P(TO_INT(to) + thinLength);
        length -= thinLength;
        }
    return result;
    }

/**
**  Convert to a wide byte offset from an address.
**/

INT TO_INT(const void * vo)
    {
    INT ii = ((INT) vo);
    #ifdef __MSDOS__
    ii = ((((ii >> 0x10) & 0xFFFF) << 4) + (ii & 0xFFFF));
    #endif
    return ii;
    }

/**
**  Convert from a wide byte offset to an address.
**/

void * TO_VOID_P(INT ii)
    {
    #ifdef __MSDOS__
    ii = (((ii >> 4) << 0x10) | (ii & 0xF));
    #endif
    void * vo = ((void *) ii);
    return vo;
    }

#endif /*  __MSDOS__ */

/* end of file */
