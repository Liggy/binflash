/**
**  cl.cpp
**  Monday, July 8, 2002
**
**  Describe a cursor stepping thru the chars of the args of the argv
**  of the main routine of a C/C++ program.
**
**  Grep views include:
**
**          grep "^[A-Za-z_]"               // subroutines
**
**  Read -xyz as -x yz, or as -x -y z, or as -x -y -z.
**
**  Read ints printed as decimal or hexadecimal.
**
**  Read // commentary.
**
**  Read the blank-delimited words of a string (for example, an
**  environment variable) as a list of args.
**
**  Allocate string space as needed from the malloc heap.  Leave the
**  work of garbage collection to the caller, or to stdlib.exit.
**
**  Wish the Posix standard and Gnu extensions of:
**  http://www.gnu.org/software/gengetopt/man_getopt.html
**  did more of this via getopt argv, getopt argc, optind, & nextchar.
**
**  See also "IEEE Std 1003.2-1992 Interpretation #150"
**  http://standards.ieee.org/reading/ieee/interp/1003-2-92_int/pasc-1003.2-150.html
**/

/**
**  Link with standard C libraries.
**/

#include <limits.h> /* INT_MAX ... */
#include <stdio.h> /* FILE ... */
#include <stdlib.h> /* calloc ... */
#include <string.h> /* memset ... */

/**
**  Link with the *.c* of ../plscsi/.
**/

#include "plscsi.h"

/**
**  Describe the main command line of a C/C++ program.
**/

struct CommandLine
    {
    char const * const *    theArgs;
    int                     theArgsLength;
    int                     theArgIndex;
    int                     theArgOffset;
    };

/**
**  List the decimal digits in order, so that the offset of a digit
**  is its value.
**/

#define DECIMAL_DIGIT_SET       "0123456789"

/**
**  List the decimal subset of the hex digits twice, so that the
**  offset of an upper or lower case hex digit, modulo x10, is its
**  value.
**/

#define HEX_DIGIT_SET           "0123456789ABCDEF0123456789abcdef"

/**
**  Construct a new CommandLine.
**/

CommandLine * newCommandLine(void)
    {
    CommandLine * cl = (CommandLine *) calloc(1, sizeof (CommandLine));
    if (cl != NULL)
        {
        cl->theArgs = NULL;
        cl->theArgsLength = 0;
        cl->theArgIndex = 0;
        cl->theArgOffset = 0;
        return cl;
        }
    return NULL;
    }

/**
**  Re/start the parse of the main args.
**/

void clSet(CommandLine * cl, char const * const * argv, int argc)
    {
    cl->theArgs = argv;
    cl->theArgsLength = argc;
    cl->theArgIndex = 0;
    cl->theArgOffset = 0;
    }

/**
**  Convert to a mutable array of mutable strings from a string.
**
**  Treat the chars of the blankSet, except not the trailing NUL,
**  as blanks that separate strings.  See strings separated by
**  blanks.  Strip leading and trailing blanks.
**
**  Also add a trailing null string past the last string.
**
**  Example usage:
**
**      char * * argv = toArgs(getenv("SOME_VAR_NAME"), " \t\n\r");
**      int argc = toArgsLength(argv);
**
**  Compare java.util.StringTokenizer
**  http://java.sun.com/j2se/1.4/docs/api/java/util/StringTokenizer.html
**
**  See toArgsLength(char const * const *).
**/

static char * * toArgs(char const * st, char const * set)
    {

    /* Fail if no chars. */

    if (st == NULL)
        {
        return NULL; /* fail */
        }

    /* Construct a large enough array. */

/*  int maxArgsLength = (((strlen(st) + 1) / 2) + 1); // perhaps */
    int maxArgsLength = strlen(st);
    char * * args = (char * *) calloc(1, maxArgsLength * sizeof (char const *));
    if (args == NULL)
        {
        return NULL; /* fail */
        }

    /* Fill the array with words. */

    int argsIndex = 0;
    for (;;)
        {

        /* Skip over white space & break past the last word. */

        st += strspn(st, set);
        if (*st == '\0')
            {
            break;
            }

        /* Measure and copy a word. */

        int argLength = strcspn(st, set);
        char * arg = (char *) calloc(1, argLength + 1); /* add 1 to add sizeof trailing Nul */
        if (arg == NULL)
            {
            return NULL; /* fail */
            }
        (void) memmove(arg, st, argLength); /* could memcpy */
        arg[argLength] = '\0'; /* calloc did this already */
        st += argLength;

        /* Remember the copied word. */

        args[argsIndex++] = arg;
        }

    /* Add a trailing null string past the last string. */

    args[argsIndex++] = NULL;
    return args;
    }

/**
**  Count the not-null strings of an array of strings.
**/

static int toArgsLength(char const * const * args)
    {
    int argsLength = 0;
    if (args != NULL)
        {
        while (*args++ != NULL)
            {
            ++argsLength;
            }
        }
    return argsLength;
    }

/**
**  Re/start the parse of args from a string.
**/

void clSetFromString(CommandLine * cl, char const * st)
    {
    char * * argv = toArgs(st, " \t\n\r"); /* leave out "\a\b\f" */
    cl->theArgs = (char const * const *) argv; /* tcpp101 requires this cast */
    cl->theArgsLength = toArgsLength(cl->theArgs);
    cl->theArgIndex = 0;
    cl->theArgOffset = 0;
    }

/**
**  Return the next arg that will be read, else null.
**/

char const * clPeek(CommandLine * cl)
    {

    /* Fetch the CommandLine. */

    char const * const * args = cl->theArgs;
    int argsIndex = cl->theArgIndex;
    int argsLength = cl->theArgsLength;
    int argOffset = cl->theArgOffset;

    /* Find the arg. */

    if (args != NULL)
    if (argsIndex < argsLength)
        {
        char const * arg = args[argsIndex];

        /* Split the arg. */

        if (arg != NULL)
            {
            if (0 <= argOffset)
                {
                arg = &arg[argOffset];
                }
            }
        return arg;
        }
    return NULL;
    }

/**
**  Consume the selected arg and return it, else return null.
**/

char const * clRead(CommandLine * cl)
    {
    char const * arg = clPeek(cl);
    if (arg != NULL)
        {
        ++cl->theArgIndex;
        cl->theArgOffset = 0;
        }
    return arg;
    }

/**
**  Consume a comment and return zero, else return negative.
**
**  Read an arg of -- as an isolated comment.
**
**  Read all the args following the arg // as part of one comment.
**/

int clReadComment(CommandLine * cl)
    {
    char const * arg = clPeek(cl);
    if (arg != NULL)
        {
        if ((arg[0] == '-') && (arg[1] == '-') && (arg[2] == '\0'))
            {
            (void) clRead(cl);
            return 0; /* succeed */
            }
        if ((arg[0] == '/') && (arg[1] == '/') && (arg[2] == '\0'))
            {
            cl->theArgIndex = cl->theArgsLength;
            cl->theArgOffset = 0;
            return 0; /* succeed */
            }
        }
    return -1; /* fail */
    }

/**
**  Consume another concise -xyz switch and return it, else
**  return -1.
**/

int clReadSwitch(CommandLine * cl)
    {

    /* Find the arg. */

    char const * arg = clPeek(cl);
    if (arg != NULL)
        {

        /* Read "-" from "-xyz", but leave "-" or "--" alone. */

        int argOffset = cl->theArgOffset;
        if (argOffset == 0)
            {
            char ch0 = arg[0];
            if (ch0 == '-')
                {
                char ch1 = arg[1];
                if ((ch1 != '\0') && (ch1 != '-'))
                    {
                    cl->theArgOffset = argOffset = 1;
                    ++arg; /* in effect, again: arg = peekArg(cl); */
                    }
                }
            }

        /* Read the first else the next of the switches of a list. */

        if (0 < argOffset)
            {
            char ch0 = arg[0]; /* theArgs[theArgIndex][theArgOffset] */
            int switchInt = (ch0 & 0xFF);
            char ch1 = arg[1];

            /* Read the switch, don't just peek at it. */

            cl->theArgOffset = (argOffset + 1);

            /* After consuming the last switch, consume the arg. */

            if (ch1 == '\0')
                {
                (void) clRead(cl);
                }

            /* Return the switch read. */

            return switchInt;
            }

        /* Else say no switch read. */

        }
    return -1;
    }

/**
**  Return the leftmost offset into a string at which a char is
**  found, else return negative.  Differ from c.lang.string.strchr
**  by never finding '\0' in a string.
**/

static int indexOf(char const * st, char ch)
    {
    int index = -1;
    if (st != NULL)
        {
        char const * which = strchr(st, ch);
        if ((which != NULL) && (*which != '\0'))
            {
            index = ((int) (which - st));
            }
        }
    return index;
    }

/**
**  Convert to int from nothing but digits.  Return an equal
**  nonnegative int value, else return negative.
**
**  Skip over leading zeroes without changing the radix or digitSet.
**
**  Reject anything not in the digitSet, even a leading plus or
**  minus sign.
**
**  Differ from c.lang.stdio.sscanf by ensuring c.lang.stdio.printf
**  is equal.  For example, in places where INT_MAX is x7FFF, do
**  not accept x12345 as meaning x2345.
**
**  Differ from c.lang.stdio.sscanf %d by ensuring that all of the
**  string was read.  Note c.lang.stdio.sscanf %d%c should return 1
**  rather than 2 if indeed all of a string was read.
**/

static INT fromIntDigits(char const * st, char const * set, int radix)
    {

    /* Constrain args. */

    if ((st == NULL) || (set == NULL) && (radix < 2))
        {
        return -1; /* fail */
        }

    /* Sum the digits. */

    INT sum = 0;
    while (*st != '\0')
        {
        char ch = *st++;
        int delta = indexOf(set, ch);
        if (delta < 0)
            {
            return -1; /* fail */
            }
        int digit = (delta % radix);
        INT maxInt = INT_MOST_POS;
        if (((maxInt - digit) / radix) < sum)
            {
            return -1; /* fail */
            }
        sum *= radix; sum += digit;
        }
    return sum; /* succeed */
    }

/**
**  Consume an int and return an equal nonnegative int value, else
**  return negative.
**
**  Differ from c.lang.stdio.sscanf by ensuring c.lang.stdio.printf
**  is equal.  For example, in places where INT_MAX is x7FFF, do
**  not accept x12345 as meaning x2345.
**
**  Differ from C by accepting any of the prefixes "0x x X" to mean
**  hexadecimal, not just the prefix "0x".
**
**  Differ from c.lang.stdio.sscanf %d by refusing to let a decimal
**  int other than "0" to begin with zeroes.
**
**  Differ from C by accepting only decimal or hexadecimal ints,
**  not also octal ints composed of decimal digits that happen to
**  begin with the zero.
**/

INT clReadInt(CommandLine * cl)
    {
    INT sum = -1;

    /* Parse the arg. */

    char const * arg = clPeek(cl);
    if (arg != NULL)
        {

        /* Choose radix. */

        int radix = 10;
        char const * set = DECIMAL_DIGIT_SET;
        if ((arg[0] == '0') && (arg[1] != 'x') && (arg[1] != '\0'))
            {
            return -1; /* reject octal */
            }
        if ((arg[0] == '0') && (arg[1] == 'x'))
            {
            ++arg;
            }
        if ((arg[0] == 'x') || (arg[0] == 'X'))
            {
            ++arg;
            radix = 0x10;
            set = HEX_DIGIT_SET;
            }

        /* Accept decimal. */

        sum = fromIntDigits(arg, set, radix);

        /* Consume only an acceptable arg. */

        if (0 <= sum)
            {
            (void) clRead(cl);
            }
        }

    /* Proceed. */

    return sum;
    }

/**
**  Consume a hex byte and return an equal nonnegative int value,
**  else return negative.
**
**  As a hex byte, accept:
**
**          ( *|:)?[0-9A-Fa-f][0-9A-Fa-f]?
**
**  That is, one or two hexadecimal digits in upper or lower or mixed
**  case, preceded by run of blanks or by a single colon.
**/

int clReadXX(CommandLine * cl)
    {

    /* Read an arg. */

    char const * arg = clPeek(cl);
    if (arg == NULL)
        {
        return -1;
        }

    /* Disregard one colon or a number of blanks (or neither). */

    int index = 0;
    if (arg[index] == ':')
        {
        ++index;
        }
    else
        {
        while (arg[index] == ' ')
            {
            ++index;
            }
        }

    /* Accept one or two hexadecimal digits. */

    int digit = indexOf(HEX_DIGIT_SET, arg[index]);
    if (0 <= digit)
        {
        ++index;
        int sum = (digit & 0xF);
        digit = indexOf(HEX_DIGIT_SET, arg[index]);
        if (0 <= digit)
            {
            ++index;
            sum <<= 4; sum += (digit & 0xF);
            }

        /* Consume everything accepted. */

        cl->theArgOffset += index;
        if (arg[index] == '\0')
            {
            (void) clRead(cl);
            }

        /* Succeed. */

        return sum;
        }

    /* Otherwise fail. */

    return -1; /* fail */
    }

/* end of file */
