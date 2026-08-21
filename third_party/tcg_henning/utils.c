/* UTILS.C */

/*********************************************************************************
Licence for TCG - Tibetan Calendar software for "grub rtsis"

Copyright (c) 2009-2013 Edward Henning

Permission is hereby granted, free of charge, to any person  obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so, subject
to the following conditions:

The above copyright notice and this permission notice shall be included in all copies
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
************************************************************************************/

#include <conio.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <dos.h>
#include <bios.h>
#include <math.h>
#include "tc.h"
#include "tc.ext"
#include "bcd.h"
#include "bcd.ext"

extern FILE *repfil;

void prn_lst6 ( int * a1 )
   {
     printf ("%d; %d, %d, %d, %d, %d\n", a1[0], a1[1], a1[2],
                                       a1[3], a1[4], a1[5] );
   }

void prn_lst7 ( int * a1 )
   {
     printf ("%d; %d, %d, %d, %d, %d, %d\n", a1[0], a1[1], a1[2],
                                       a1[3], a1[4], a1[5], a1[6] );
   }

void prn_lstd ( int *a1, int frac1, int frac2 )
  {
   double num, den;
   double x; //, s;
   printf ("%d; %d, %d, %d, %d, %d - ", a1[0], a1[1], a1[2],
                                       a1[3], a1[4], a1[5] );

   num = ( ( ( ( (double) a1[0] * 60.0 + (double) a1[1] ) * 60.0 + (double)
         a1[2] ) * 6.0 + (double) a1[3] ) * (double) frac1 + (double) a1[4] )
         * (double) frac2 + (double) a1[5];
   den = 27.0 * 3600.0 * 6.0 * (double) frac1 * (double) frac2;
   x = 360.0 * num / den;
   printf ("%f\n", x );
  } // END - prn_lstd ()

/* Expand a list to lowest fractional part */
int exp_lst ( int l[5], int n )
  {
    int x;
    x = ( ( ( l[0] * 60 + l[1] ) * 60 + l[2] ) * 6 + l[3] ) * n + l[4];
    return ( x );
  } // END - exp_lst ()

void mul_g6 ( int a[6], int x, int frac4, int frac5 )
  {
    l2bcd ( bcdx0, a[0] );
    l2bcd ( bcdx1, a[1] );
    l2bcd ( bcdx2, a[2] );
    l2bcd ( bcdx3, a[3] );
    l2bcd ( bcdx4, a[4] );
    l2bcd ( bcdx5, a[5] );

    mulbcdl ( bcdx0, bcdx0, x );
    mulbcdl ( bcdx1, bcdx1, x );
    mulbcdl ( bcdx2, bcdx2, x );
    mulbcdl ( bcdx3, bcdx3, x );
    mulbcdl ( bcdx4, bcdx4, x );
    mulbcdl ( bcdx5, bcdx5, x );

    divbcdl ( bcary, bcdx5, frac5 );
    modbcdl ( bcdx5, bcdx5, frac5 );
    addbcd ( bcdx4, bcdx4, bcary );

    divbcdl ( bcary, bcdx4, frac4 );
    modbcdl ( bcdx4, bcdx4, frac4 );
    addbcd ( bcdx3, bcdx3, bcary );

    divbcdl ( bcary, bcdx3, 6 );
    modbcdl ( bcdx3, bcdx3, 6 );
    addbcd ( bcdx2, bcdx2, bcary );

    divbcdl ( bcary, bcdx2, 60 );
    modbcdl ( bcdx2, bcdx2, 60 );
    addbcd ( bcdx1, bcdx1, bcary );

    divbcdl ( bcary, bcdx1, 60 );
    modbcdl ( bcdx1, bcdx1, 60 );
    addbcd ( bcdx0, bcdx0, bcary );

    a[0] = bcd2l ( bcdx0 );
    a[1] = bcd2l ( bcdx1 );
    a[2] = bcd2l ( bcdx2 );
    a[3] = bcd2l ( bcdx3 );
    a[4] = bcd2l ( bcdx4 );
    a[5] = bcd2l ( bcdx5 );
  } // END - mul_g6 ()

void div_g6 ( int * a, int x, int frac4, int frac5 )
  {
    l2bcd ( bcdx0, a[0] );
    l2bcd ( bcdx1, a[1] );
    l2bcd ( bcdx2, a[2] );
    l2bcd ( bcdx3, a[3] );
    l2bcd ( bcdx4, a[4] );
    l2bcd ( bcdx5, a[5] );

    modbcdl ( bcary, bcdx0, x );
    divbcdl ( bcdx0, bcdx0, x );
    mulbcdl ( bcary, bcary, 60 );
    addbcd ( bcdx1, bcdx1, bcary );

    modbcdl ( bcary, bcdx1, x );
    divbcdl ( bcdx1, bcdx1, x );
    mulbcdl ( bcary, bcary, 60 );
    addbcd ( bcdx2, bcdx2, bcary );

    modbcdl ( bcary, bcdx2, x );
    divbcdl ( bcdx2, bcdx2, x );
    mulbcdl ( bcary, bcary, 6 );
    addbcd ( bcdx3, bcdx3, bcary );

    modbcdl ( bcary, bcdx3, x );
    divbcdl ( bcdx3, bcdx3, x );
    mulbcdl ( bcary, bcary, frac4 );
    addbcd ( bcdx4, bcdx4, bcary );

    modbcdl ( bcary, bcdx4, x );
    divbcdl ( bcdx4, bcdx4, x );
    mulbcdl ( bcary, bcary, frac5 );
    addbcd ( bcdx5, bcdx5, bcary );

    divbcdl ( bcdx5, bcdx5, x );

    a[0] = bcd2l ( bcdx0 );
    a[1] = bcd2l ( bcdx1 );
    a[2] = bcd2l ( bcdx2 );
    a[3] = bcd2l ( bcdx3 );
    a[4] = bcd2l ( bcdx4 );
    a[5] = bcd2l ( bcdx5 );
  } // END - div_g6 ()

void sub_g6 ( int *a1, int *a2, int *a3, int n1, int n5, int n6 )
  {
  int a4[6];
  int  i;
    for ( i = 0; i < 6; ++i )
      a4[i] = a2[i];

    a1[5] = a4[5] - a3[5];
    if ( a1[5] < 0 )
      {
        a1[5] = a1[5] + n6;
        a4[4] = a4[4] - 1;
      }

    a1[4] = a4[4] - a3[4];
    if ( a1[4] < 0 )
      {
        a1[4] = a1[4] + n5;
        a4[3] = a4[3] - 1;
      }

    a1[3] = a4[3] - a3[3];
    if ( a1[3] < 0 )
      {
        a1[3] = a1[3] + 6;
        a4[2] = a4[2] - 1;
      }
    a1[2] = a4[2] - a3[2];
    if ( a1[2] < 0 )
      {
        a1[2] = a1[2] + 60;
        a4[1] = a4[1] - 1;
      }
    a1[1] = a4[1] - a3[1];
    if ( a1[1] < 0 )
      {
        a1[1] = a1[1] + 60;
        a4[0] = a4[0] - 1;
      }
    a1[0] = a4[0] - a3[0];
    if ( a1[0] < 0 )
      a1[0] = a1[0] + n1;
  } // END - sub_g6 ()

// Function to add two arrays together
//           a1 = a2 + a3

void add_g6 ( int *a1, int *a2, int *a3, int n1, int n2, int n3 )
  {
    int r;
    r = a2[5] + a3[5];
    if ( r < 0 )
      {
        printf ( "\n\nERROR IN ADD_G6:\n");
        printf ( "List A2: %d;%d,%d,%d,%d,%d\n", a2[0], a2[1], a2[2], a2[3], a2[4], a2[5] );
        printf ( "List A3: %d;%d,%d,%d,%d,%d\n", a3[0], a3[1], a3[2], a3[3], a3[4], a3[5] );        
        printf ( "A2[5] = %d\n", a2[5] );
        printf ( "A3[5] = %d\n\n", a3[5] );
      }
    a1[5] = r % n3;
    r = a2[4] + a3[4] + r / n3;
    a1[4] = r % n2;
    r = a2[3] + a3[3] + r / n2;
    a1[3] = r % 6;
    r = a2[2] + a3[2] + r / 6;
    a1[2] = r % 60;
    r = a2[1] + a3[1] + r / 60;
    a1[1] = r % 60;
    r = a2[0] + a3[0] + r / 60;
    a1[0] = r % n1;
  } // END - add_g6 ()

// Function to multiply a list

void mul_gen ( int *res, int *lst, int x, int n1, int n2 )
  {
    if ( x >= 0 )
      {
        l2bcd ( bcdx0, lst[0] );
        l2bcd ( bcdx1, lst[1] );
        l2bcd ( bcdx2, lst[2] );
        l2bcd ( bcdx3, lst[3] );
        l2bcd ( bcdx4, lst[4] );

        mulbcdl ( bcdx0, bcdx0, x );
        mulbcdl ( bcdx1, bcdx1, x );
        mulbcdl ( bcdx2, bcdx2, x );
        mulbcdl ( bcdx3, bcdx3, x );
        mulbcdl ( bcdx4, bcdx4, x );

        divbcdl ( bcary, bcdx4, n2 );
        modbcdl ( bcdx4, bcdx4, n2 );
        addbcd ( bcdx3, bcdx3, bcary );

        divbcdl ( bcary, bcdx3, 6 );
        modbcdl ( bcdx3, bcdx3, 6 );
        addbcd ( bcdx2, bcdx2, bcary );

        divbcdl ( bcary, bcdx2, 60 );
        modbcdl ( bcdx2, bcdx2, 60 );
        addbcd ( bcdx1, bcdx1, bcary );

        divbcdl ( bcary, bcdx1, 60 );
        modbcdl ( bcdx1, bcdx1, 60 );
        addbcd ( bcdx0, bcdx0, bcary );

        modbcdl ( bcdx0, bcdx0, n1 );

        res[0] = bcd2l ( bcdx0 );
        res[1] = bcd2l ( bcdx1 );
        res[2] = bcd2l ( bcdx2 );
        res[3] = bcd2l ( bcdx3 );
        res[4] = bcd2l ( bcdx4 );
      }
    else
      {
        x = -x;
        mul_gen ( res, lst, x, n1, n2 );
        clrlst (zerlst);
        sub_gen ( res, zerlst, res, n1, n2);
      }
  } // END - mul_gen ()

double conv2degs ( int * list, int fact4, int fact5 )
  {
    double x, f;
    if ( fact5 != 1 )
      x = ( ( ( ( (double) list[0] * 60.0 + (double) list[1] ) * 60.0 +
          (double) list[2] ) * 6.0 + (double) list[3] ) * (double) fact4 +
          (double) list[4] ) * (double) fact5 + (double) list[5];
    else
      x = ( ( ( (double) list[0] * 60.0 + (double) list[1] ) * 60.0 +
          (double) list[2] ) * 6.0 + (double) list[3] ) * (double) fact4 +
          (double) list[4];
    if ( fact5 != 1 )
      f = 583200.0 * (double) fact4 * (double) fact5;
    else
      f = 583200.0 * (double) fact4;
    return ( x * 360.0 / f );
  } // END - conv2degs ()

double conv2degs1 ( int * l, int frac5, int frac6 )
  {
    int n, c;
    n = ( ( ( ( l[0] * 60 + l[1] ) * 60 + l[2] ) * 6 + l[3] ) * frac5 +
          l[4] ) * frac6 + l[5];
    c = 27 * 60 * 60 * 6 * frac5 * frac6;

    return ( (double) n / (double) c * 360.0 );
  } // END - conv2degs1 ()

/* Function to add two arrays together */
/*           a1 = a2 + a3              */

void add_gen ( int *a1, int *a2, int *a3, int n1, int n2)
  {
    int r;
    r = a2[4] + a3[4];
    if ( r < 0 )
      {
        printf ( "\n\nERROR IN ADD_GEN:\n");
        printf ( "A2[4] = %d\n", a2[4] );
        printf ( "A3[4] = %d\n\n", a3[4] );
      }
    a1[4] = r % n2;
    r = a2[3] + a3[3] + r / n2;
    a1[3] = r % 6;
    r = a2[2] + a3[2] + r / 6;
    a1[2] = r % 60;
    r = a2[1] + a3[1] + r / 60;
    a1[1] = r % 60;
    r = a2[0] + a3[0] + r / 60;
    a1[0] = r % n1;
  } // END - add_gen ()

// Function to subtract two arrays
//           a1 = a2 - a3

void sub_gen ( int *a1, int *a2, int *a3, int n1, int n2 )
  {
  int a4[5];
  int  i;
    for ( i = 0; i < 5; ++i )
      a4[i] = a2[i];
    a1[4] = a4[4] - a3[4];
    if ( a1[4] < 0 )
      {
        a1[4] = a1[4] + n2;
        a4[3] = a4[3] - 1;
      }
    a1[3] = a4[3] - a3[3];
    if ( a1[3] < 0 )
      {
        a1[3] = a1[3] + 6;
        a4[2] = a4[2] - 1;
      }
    a1[2] = a4[2] - a3[2];
    if ( a1[2] < 0 )
      {
        a1[2] = a1[2] + 60;
        a4[1] = a4[1] - 1;
      }
    a1[1] = a4[1] - a3[1];
    if ( a1[1] < 0 )
      {
        a1[1] = a1[1] + 60;
        a4[0] = a4[0] - 1;
      }
    a1[0] = a4[0] - a3[0];
    if ( a1[0] < 0 )
      a1[0] = a1[0] + n1;
  } // END - sub_gen ()

// Function to print a list on the screen

void prn_lst ( int *a1 )
   {
     printf ("%d; %d, %d, %d, %d\n", a1[0], a1[1], a1[2], a1[3], a1[4]);
   }

void clrlst ( int *l )
  {
    int n;
    for ( n = 0; n < 5; ++n )
      l[n] = 0;
  }

void clrlst6 ( int *l )
  {
    int n;
    for ( n = 0; n < 6; ++n )
      l[n] = 0;
  }

void clear_a_b ( void )
  {
    clrlst ( lista );
    clrlst ( listb );
  }

void clear_a_b6 ( void )
  {
    clrlst6 ( lista );
    clrlst6 ( listb );
  }

// Routines for printing to report file.

void start_print ( void )
  {
    repfil = fopen ( "report.dat", "w" );
  }

void prin_str ( char *s )
  {
    fprintf ( repfil, "%s\n", s );
  }

// Function to add one string to end of another.
// S1 = S1 + S2

void addstrings (str1, str2)
  char  str1[], str2[];
  {
    int i, j, k;
    i = strlen ( str1 );
    j = strlen ( str2 );
    for ( k = 0; k <= j; ++k )
      str1[i + k] = str2[k];
  }

// This routine not currently used:

void mul_g7 ( int a[6], int x, int frac4, int frac5, int frac6 )
  {
    l2bcd ( bcdx0, a[0] );
    l2bcd ( bcdx1, a[1] );
    l2bcd ( bcdx2, a[2] );
    l2bcd ( bcdx3, a[3] );
    l2bcd ( bcdx4, a[4] );
    l2bcd ( bcdx5, a[5] );
    l2bcd ( bcdx6, a[6] );

    mulbcdl ( bcdx0, bcdx0, x );
    mulbcdl ( bcdx1, bcdx1, x );
    mulbcdl ( bcdx2, bcdx2, x );
    mulbcdl ( bcdx3, bcdx3, x );
    mulbcdl ( bcdx4, bcdx4, x );
    mulbcdl ( bcdx5, bcdx5, x );
    mulbcdl ( bcdx6, bcdx6, x );

    divbcdl ( bcary, bcdx6, frac6 );
    modbcdl ( bcdx6, bcdx6, frac6 );
    addbcd ( bcdx5, bcdx5, bcary );

    divbcdl ( bcary, bcdx5, frac5 );
    modbcdl ( bcdx5, bcdx5, frac5 );
    addbcd ( bcdx4, bcdx4, bcary );

    divbcdl ( bcary, bcdx4, frac4 );
    modbcdl ( bcdx4, bcdx4, frac4 );
    addbcd ( bcdx3, bcdx3, bcary );

    divbcdl ( bcary, bcdx3, 6 );
    modbcdl ( bcdx3, bcdx3, 6 );
    addbcd ( bcdx2, bcdx2, bcary );

    divbcdl ( bcary, bcdx2, 60 );
    modbcdl ( bcdx2, bcdx2, 60 );
    addbcd ( bcdx1, bcdx1, bcary );

    divbcdl ( bcary, bcdx1, 60 );
    modbcdl ( bcdx1, bcdx1, 60 );
    addbcd ( bcdx0, bcdx0, bcary );

    a[0] = bcd2l ( bcdx0 );
    a[1] = bcd2l ( bcdx1 );
    a[2] = bcd2l ( bcdx2 );
    a[3] = bcd2l ( bcdx3 );
    a[4] = bcd2l ( bcdx4 );
    a[5] = bcd2l ( bcdx5 );
    a[6] = bcd2l ( bcdx6 );
  } // END - mul_g7 ()

// This routine not currently used:

void add_g7 ( int *a1, int *a2, int *a3, int n1, int n2, int n3, int n4 )
  {
    int r;
    r = a2[6] + a3[6];
    if ( r < 0 )
      {
        printf ( "\n\nERROR IN ADD_G7:\n");
        printf ( "A2[5] = %d\n", a2[6] );
        printf ( "A3[5] = %d\n\n", a3[6] );
      }

    a1[6] = r % n4;
    r = a2[5] + a3[5] + r / n4;
    a1[5] = r % n3;
    r = a2[4] + a3[4] + r / n3;
    a1[4] = r % n2;
    r = a2[3] + a3[3] + r / n2;
    a1[3] = r % 6;
    r = a2[2] + a3[2] + r / 6;
    a1[2] = r % 60;
    r = a2[1] + a3[1] + r / 60;
    a1[1] = r % 60;
    r = a2[0] + a3[0] + r / 60;
    a1[0] = r % n1;
  } // END - add_g7 ()
