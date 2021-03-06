/*
 * File: ifact.c
 * -------------
 * This program includes the Fact function and a test
 * program that prints the factorials of the numbers between
 * the limits LowerLimit and UpperLimit, inclusive.
 */

#include <stdio.h>
#include "genlib.h"

/*
 * Constants
 * ---------
 * LowerLimit -- Starting value for factorial table
 * UpperLimit -- Final value for factorial table
 */

#define LowerLimit 0
#define UpperLimit 7

/* Function prototypes */

static int Fact(int n);

/* Main program */

main()
{
    int i;

    for (i = LowerLimit; i <= UpperLimit; i++) {
        printf("%d! = %5d\n", i, Fact(i));
    }
}


/*
 * Function: Fact
 * Usage: f = Fact(n);
 * -------------------
 * This function returns the factorial of n, which is defined
 * as the product of all integers from 1 up to n.
 */

static int Fact(int n)
{
    int product, i;

    product = 1;
    for (i = 1; i <= n; i++) {
        product *= i;
    }
    return (product);
}
