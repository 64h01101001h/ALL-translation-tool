/* T1.C */

/*********************************************************************************
License for KCK - Kalacakra Calendar software

Copyright (c) 2009-2011 Edward Henning

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
#include <io.h>
#include <conio.h>
#include <dos.h>
#include <ctype.h>
#include <string.h>
#include "tc.h"
#include "tc.def"

extern double mol, sol, meansol, sun_minus_mnode, moon_minus_sun;
void run_test ( void );
 
int print_on = 0;
int printinprog = 0;
char printstring[120];
FILE *repfil;

void start_print ( void );
void prin_str ( char * );
void addstrings ( char *, char * );

void cal_cyc ( void ) // MAIN ROUTINE
  {
    int more, n, newmth, i, x;
    long int juldat_m;
    double jdf_m, jdf_t, nyibarf;
    double obs_long = 83.0202222; // Observer longitude, set for Sarnath.
//  union  g_char chrt;
    int chr;

// Set language for month names:

    for ( i = 0; i < 12; ++i )
      lunmon[i] = lunmons[i];  // For Sanskrit
//    lunmon[i] = lunmont[i];  // For Tibetan

    restart: cls ();
//  Change for Win32 console app:    
//  pos (0,0);
    printf ( "%s\n", e_str[epch] );
    printf ("Enter the starting date, month and year: ");
    scanf ("%ld %ld %ld", &tt, &tm, &ty);
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
            if ( epch == 0 && zladag[0] <= 0L ) // Crude fix for Tantra epoch month.
              {
                scndzero = 1;
                zladag[0] = -1L;
              }
            adj_zla ();
            cur_mth = zladag[0];
            gza_dru (cur_mth);
            nyi_dru (cur_mth);
            rilchaf ( cur_mth ); // MOVED, from gza_dag, NEW, 11/9/94
            if ( adj_mth == -12L ) // Year will have been incremented.
              --ty; 
          }
        backtoprint:

        cls ();
        printf ( "%s\n", e_str[epch] );

        if ( printinprog )
          {
            sprintf ( printstring, "\n%s\n", e_str[epch] );
            prin_str ( printstring );
          }

        printf ("\nThe ZLA-DAG is: %ld;%ld, ", zladag[0], zladag[1]);
        printf ("Month = %ld, ", tm );
        printf ("Year = %ld\n", ty );

        if ( printinprog )
          {
            sprintf ( printstring,
              "The ZLA-DAG is: %ld;%ld, Month = %ld, Year = %ld",
               zladag[0], zladag[1], tm, ty );
            prin_str ( printstring );
          }

        printf ("DATE is: %ld / %ld / %ld,  ", tt, tm, ty);
        x = (int) adj_mth;
        if ( x < 0 ) 
          x = -x;
        printf ( "Corrected Month = %ld, %s\n", adj_mth, lunmon[x-1] );

        if ( printinprog )
          {
            sprintf ( printstring,
              "DATE is: %ld / %ld / %ld - Corrected Month = %ld\n",
               tt, tm, ty, adj_mth );
            prin_str ( printstring );
          }
        tse_dru (tt);
        nyi_lon (tt);
        add_gen (tsebar, gzadru, tsedru, 7L, gza_f );
        add_gen (nyibar, nyidru, nyilon, 27L, sun_f );

        nyibarl = exp_lst ( nyibar, sun_f ); // These are for time of day
        tsebarl = exp_lst ( tsebar, gza_f ); // and western comparisons

        nyibarf = 360.0 * (double) nyibarl /
                  (double) ( 27L * 60L * 60L * 6L * sun_f );
        tsebarl = tsebarl - tsebar[0] * 60L * 60L * 6L * gza_f;

        nyi_dag (nyibar);  // MUST BE DONE FIRST
        gza_dag (tsebar);

        do_rahu ( cur_mth, tt );  // in T2.C
        spi_zagf ();   // In T2.C - calculates juldat.
// Julian calculated is for the integer Julian day count at Noon
// GMT on weekday of "gza' dag". In juldat.

        printf ( "ril cha = %ld;%ld\n", rilcha[0], rilcha[1] );

        if ( printinprog )
          {
            sprintf ( printstring, "ril cha = %ld;%ld",
                      rilcha[0], rilcha[1] );
            prin_str ( printstring );
          }

        printf ("The gza-bar is: ");
        prn_lst ( tsebar );

        if ( printinprog )
          {
            sprintf ( printstring, "The gza-bar is: %ld;%ld,%ld,%ld,%ld",
              tsebar[0], tsebar[1], tsebar[2], tsebar[3], tsebar[4] );
            prin_str ( printstring );
          }

        printf ("The nyi-bar is: ");
        prn_lstd ( nyibar, sun_f, 1L );

        if ( printinprog )
          {
            sprintf ( printstring, "The nyi-bar is: %ld;%ld,%ld,%ld,%ld",
              nyibar[0], nyibar[1], nyibar[2], nyibar[3], nyibar[4] );
            prin_str ( printstring );
          }

        printf ("The gza-dag is: ");
        prn_lst ( gzadag );

        if ( printinprog )
          {
            sprintf ( printstring, "The gza-dag is: %ld;%ld,%ld,%ld,%ld",
              gzadag[0], gzadag[1], gzadag[2], gzadag[3], gzadag[4] );
            prin_str ( printstring );
          }

        printf ("The nyi-dag is: ");
        prn_lstd ( nyidag, sun_f, 1L );

        if ( printinprog )
          {
            sprintf ( printstring, "The nyi-dag is: %ld;%ld,%ld,%ld,%ld",
              nyidag[0], nyidag[1], nyidag[2], nyidag[3], nyidag[4] );
            prin_str ( printstring );
          }

// Now, calculate Moon at lunar day and daybreak:
        clrlst (mondag);
        clrlst (monsar);
        mondag[1] = 54L; // separation in one lunar day.
        mul_gen ( mondag, mondag, tt, 27L, sun_f );
        add_gen ( mondag, nyidag, mondag, 27L, sun_f );
        for ( i = 0; i < 6; ++i )
          monsar[i] = gzadag[i];
        monsar[0] = 0L;
        monsar[5] = ( monsar[5] * sun_f ) / gza_f;
        sub_gen ( monsar, mondag, monsar, 27L, sun_f);

        sprintf ( printstring,
                  "Moon, Lunar day: %ld;%ld,%ld,%ld,%ld,%ld - \
Daybreak: %ld;%ld,%ld,%ld,%ld,%ld.",
                mondag[0], mondag[1], mondag[2], mondag[3], mondag[4],
                mondag[5], monsar[0], monsar[1], monsar[2], monsar[3],
                monsar[4], monsar[5] );

        printf ( "%s\n", printstring );
        if ( printinprog )
          prin_str ( printstring );

        printf ("General day factors: %ld, %ld\n", spz_frac_b, spz_frac_c );

        if ( printinprog )
          {
            sprintf ( printstring, "General day factors: %ld, %ld",
                                   spz_frac_b, spz_frac_c );
            prin_str ( printstring );
          }

        do_plans ( spizag );   // In T3.C

        printf ( "Dragkang factor: %ld\n", dragkres );

        printf ( "SPYI ZHAG = %ld. ", spizag );
        printf ( "Julian day = %ld. ", juldat );
        printf ( "Western date = %s, %ld/%ld/%ld\n",
                  dayoweek[ (int) doweek ], wd, wm, wy);

        if ( printinprog )
          {
            sprintf ( printstring,
    "SPYI ZHAG = %ld. Julian day = %ld. Western date = %s, %ld/%ld/%ld",
              spizag, juldat, dayoweek[ (int) doweek ], wd, wm, wy);
            prin_str ( printstring );
          }

// To calculate exact Julian day for TRUE tithi:
// Julian day at 5.00 LMST:

        jdf_t = (double) juldat - obs_long / 360.0 - 7.0 / 24.0;

// Now add for time of true tithi:

        jdf_t = jdf_t + gzadag[1] / 60.0 + gzadag[2] / 3600.0 + gzadag[3] /
                21600.0 + gzadag[4] / ( 21600.0 * (double) gza_f );

// NOW CALCULATE FOR MEAN TITHI:

        if ( tsebar[0] == gzadag[0] )
          juldat_m = juldat;
        else if ( tsebar[0] == gzadag[0] - 1L || tsebar[0] == gzadag[0] + 6L )
          juldat_m = juldat - 1L;
        else if ( tsebar[0] == gzadag[0] + 1L || tsebar[0] == gzadag[0] - 6L )
          juldat_m = juldat + 1L;
        else
          {
            printf ( "Serious problem with GZA! - bar: %ld, dag: %ld\n",
                      tsebar[0], gzadag[0] );
            getch ();
            getch ();
          }

// Julian day at 5.00 LMST:

        jdf_m = (double) juldat_m - obs_long / 360.0 - 7.0 / 24.0;

// Now add for time of mean tithi:

        jdf_m = jdf_m + tsebar[1] / 60.0 + tsebar[2] / 3600.0 + tsebar[3] /
                21600.0 + tsebar[4] / ( 21600.0 * (double) gza_f );

        printf ( "Julian tithi time, mean: %f, true: %f\n", jdf_m, jdf_t );

        if ( printinprog )
          {
            sprintf ( printstring, "Julian tithi time, mean: %f, true: %f\n",
                                   jdf_m, jdf_t );
            prin_str ( printstring );
          }

        printinprog = 0;  // turn off printing to report.dat

// Change for Win32 console app:

        printf ( "\nNew date, Tshespa/lunar day, Month, Year, Report, Previous lunar day, Exit\n\n" );

//      chrt.cnum = getkey ();
//      chr = chrt.c[0];
        chr = getch ();

        if ( chr == 'N' || chr == 'n' )
          goto restart;
        if ( chr == 'R' || chr == 'r' )
          {
            if ( !print_on )
              {
                start_print ();
                print_on = 1;
              }
            printinprog = 1;
            goto backtoprint;
          }
        if ( chr == 'E' || chr == 'e' )
          more = 0;
        else if ( chr == 'T' || chr == 't' )
          {
            ++tt;
            newmth = 0;
            if ( tt == 31L )
              {
                if ( adj_mth == -12L ) // Year will have been decremented.
                  ++ty; 
                newmth = 1;
                tt = 1;
                if ( !zeromthfg )
                  ++tm;
                if ( tm > 12L )
                  {
                    ++ty;
                    tm = 1;
                  }
              }
          }
        else if ( chr == 'P' || chr == 'p' )  // Previous day
          {
            --tt;
            newmth = 0;
            if ( tt == -1L )
              {
                newmth = 1;
                tt = 29;
                if ( !zeromthfg )
                  --tm;
                if ( tm == 0L )
                  {
                    --ty;
                    tm = 12;
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
                if ( adj_mth == -12L ) // Year will have been incremented.
                  ++ty; 
               }
          }
        else if ( chr == 'Y' || chr == 'y' )
          {
            ++ty;
            newmth = 1;
            zeromthfg = 0;
            scndzero = 0;
          }
      }
  } // END - cal_cyc ()

void main ()
  {
    int    finish, xit;
//  union  g_char chrt;
    char   chr;
    set_epoch ();  // in T2.C
    finish = 0;
    while ( finish == 0 )
      {
        cls ();
        
// Changed for Win32 console app:        
        
        printat (2, 29, "      KALACAKRA CALENDAR\n");	
        printat (3, 27, "    traditional and modern\n");	
        printat (4, 24, "using traditional mathematics\n");	
        printat (5, 27, "    calculated for Sarnath\n\n");	
        printat (6, 32, "         Version 1.04" );
        printat (8, 18, "Use numeric keys to select from the following:\n\n");
        printat (12, 30, "    1. Set epoch.\n");
        printat (14, 30, "    2. Print calendar.\n");
        printat (16, 30, "    3. Calendar cycle.\n");
        printat (18, 30, "    9. Finish.\n\n");
        xit = 0;
// Change for Win32 console app:         
//      pos ( 25, 0 );
        do
          {
//          chrt.cnum = getkey ();
//          chr = chrt.c[1];
            chr = getch ();
//          pos ( 25, 0 );
            switch ( chr )
              {
                case '1':       /* F1  */
                  set_epoch ();  // In T2.C
                  xit = 1;
                  break;
                case '2':       /* F2  */ // Print Calendar
                  prn_cal ();
                  xit = 1;
                  break;
                case '3':       /* F3  */  
                  cal_cyc ();
                  xit = 1;
                  break;
                case '9':       /* F9 */
                /** FINISH **/
                  finish = 1;
                  xit = 1;
                  break;
                default:
                  ;
                  break;
              }
          } while ( xit == 0 );
      }
  } // END - main ()
