/* tests.c */

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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <conio.h>
#include "tc.h"
#include "tc.ext"

// Local to this file only:

int start_year, end_year;

void srch_cyc ( void )  // Main search and analysis routine. Reuses existing code.
  {
    int more, n, newmth;
    char chr;
    restart: 
    printf ( "\n\n%s\n", e_str[epch] );
    printf ("Enter the starting lunar date, month and year: ");
    scanf ("%d %d %d", &tt, &tm, &ty);

    start_year = ty;
    end_year = 1000;

    printf ("\n");
    n = 0;
    more = 1;
    newmth = 1;
    while (more)
      {
        if ( newmth )
          {
            if ( !zeromthfg )   // We need to use the same data, twice.
              zla_dag (ty, tm);
            adj_zla ();
            cur_mth = zladag[0];
            gza_dru (cur_mth);
            nyi_dru (cur_mth);
            rilchaf ( cur_mth ); 
          }

        printf ("\nThe ZLA-DAG is: %d;%d, ", zladag[0], zladag[1]);
        printf ("Month = %d, ", tm );
        printf ("Year = %d\n", ty );

        printf ("DATE is: %d / %d / %d,  ", tt, tm, ty);
        printf ( "Corrected Month = %d\n", adj_mth );
        tse_dru (tt);
        nyi_lon (tt);

        printf ("The gza-dru is: ");
        prn_lst ( gzadru );
        printf ("The nyi-dru is: ");
        prn_lst ( nyidru );

        add_gen (tsebar, gzadru, tsedru, 7, 707);
        add_gen (nyibar, nyidru, nyilon, 27, sun_f );
        nyibarl = exp_lst ( nyibar, sun_f ); // These are for time of day
        tsebarl = exp_lst ( tsebar, 707 ); // and western comparisons
        printf ( "ril cha = %d, %d\n", rilcha[0], rilcha[1] );
        printf ("The gza-bar is: ");
        prn_lst ( tsebar );
        printf ("The nyi-bar is: ");
        prn_lst ( nyibar );
        nyi_dag (nyibar);  // Must be done before gza_dag
        gza_dag (tsebar);
        do_rahu ( cur_mth, tt );
        printf ("The gza-dag is: ");
        prn_lst ( gzadag );
        printf ("The nyi-dag is: ");
        prn_lstd ( nyidag, sun_f, 1 );

        spi_zagf ();
        do_plans ( spizag );

        printf ( "SPYI ZHAG = %d. ", spizag );
        printf ( "Julian day = %d. ", juldat );
        printf ( "Western date = %s, %d/%d/%d\n",
                  dayoweek[ doweek ], wd, wm, wy);

// Write the required test criteria here:

// As an example, Phugpa details for the enlightenment:
//  gza': 1;38
//  Moon: 16;0 (tshes khyud)
//  Sun: 2;30
//  Rhu: 16;29

        if (  gzadag[0] == 1 &&
              gzadag[1] == 38 &&
              nyidag[0] == 2 &&
              nyidag[1] == 30 &&
              rahudong[0] == 16 )  
        chr = getch ();
        if ( chr == 'E' || chr == 'e' )
          more = 0;
        chr = 'm'; // Repeat every month 
//      chr = 'y'; // Repeat every year
//      chr = 't'; // Repeat every lunar day

        if ( wy == end_year )
          more = 0;

        if ( chr == 'N' || chr == 'n' )
          goto restart;
 
        else if ( chr == 'T' || chr == 't' )
          {
            ++tt;
            newmth = 0;
            if ( tt == 31 )
              {
                newmth = 1;
                tt = 1;
                if ( !zeromthfg )
                  ++tm;
                if ( tm > 12 )
                  {
                    ++ty;
                    tm = 1;
                  }
              }
          }
        else if ( chr == 'M' || chr == 'm' )
          {
            newmth = 1;
            if ( ! zeromthfg )
              ++tm;
            if ( tm > 12 )
              {
                ++ty;
                tm = 1;
               }
          }
        else if ( chr == 'Y' || chr == 'y' )
          {
            ++ty; // Watch this!
//          ty = ty + 5;
            newmth = 1;
            zeromthfg = 0;
          }
      }
    fclose ( fptgt );
  } // END - srch_cyc ()

