/* T1.C */

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
#include <ctype.h>
#include <conio.h>
#include "tc.h"
#include "tc.def"

FILE *repfil;

void chk_bcd ( void );

void cal_cyc ( void ) // Main routine
  {
    int more, n, newmth, i, x;
    int juldat_m;
    double jdf_m, jdf_t, nyibarf;
    double obs_long = 91.0; // Observer longitude, set for Lhasa.
    char chr;
    restart: 
    printf ( "\n\n%s\n", e_str[epch] );
    printf ("Enter the starting date, month and year: ");
    scanf ("%d %d %d", &tt, &tm, &ty);
    printf ("\n");
    n = 0;
    more = 1;
    newmth = 1;
    while (more)
      {
        if ( newmth )
          {
            if ( !zeromthfg )   // We need to use the same data, twice.
              zla_dag (ty, tm);  // KTC 16
            adj_zla ();
            cur_mth = zladag[0];
            gza_dru (cur_mth);  // KTC 18
            nyi_dru (cur_mth);  // KTC 22
            rilchaf ( cur_mth ); // KTC 21
            if ( epch == 2 && adj_mth == -12 ) // Year will have been incremented.
              --ty;
          }

backtoprint:
        printf ( "\n\n%s\n", e_str[epch] );

        if ( printinprog )
          {
            sprintf ( printstring, "\n%s\n", e_str[epch] );
            prin_str ( printstring );
          }

        printf ("\nTrue month: %d;%d, ", zladag[0], zladag[1]);
        printf ("Month = %d, ", tm );
        printf ("Year = %d\n", ty );

        if ( printinprog )
          {
            sprintf ( printstring,
              "True month: %d;%d, Month = %d, Year = %d",
               zladag[0], zladag[1], tm, ty );
            prin_str ( printstring );
          }

        x = adj_mth;
        if ( x < 0 )
          x = -x;
        if ( epch == 2 ) // Only if "'khrul sel" 
          x = x - 1;
        else
          x = x - 2;
        if ( x < 0 )
          x = x + 12;

        printf ( "Target date: %d / %d / %d - Corrected Month = %d, %s\n",
               tt, tm, ty, adj_mth, lunmont[x] );

        if ( printinprog )
          {
            sprintf ( printstring,
              "Target date: %d / %d / %d - Corrected Month = %d, %s\n",
               tt, tm, ty, adj_mth, lunmont[x] );
            prin_str ( printstring );
          }

        tse_dru (tt);
        nyi_lon (tt);
        add_gen (tsebar, gzadru, tsedru, 7, gza_f ); // KTC 23
        add_gen (nyibar, nyidru, nyilon, 27, sun_f ); // KTC 23
        nyibarl = exp_lst ( nyibar, sun_f ); // These are for time of day
        tsebarl = exp_lst ( tsebar, gza_f ); // and western comparisons
        nyibarf = 360.0 * (double) nyibarl /
                  (double) ( 27 * 60 * 60 * 6 * sun_f );
        tsebarl = tsebarl - tsebar[0] * 60 * 60 * 6 * gza_f;
        nyi_dag (nyibar);  // Must be done before gza_dag - KTC 31
        gza_dag (tsebar);  // KTC 24
        do_rahu ( cur_mth, tt );  // in T2.C - KTC 96
        spi_zagf ();   // In T2.C - calculates juldat. KTC 46

// Julian calculated is for the integer Julian day count at noon
// GMT on weekday of "gza' dag". In juldat.

        printf ( "ril cha = %d;%d\n", rilcha[0], rilcha[1] );

        if ( printinprog )
          {
            sprintf ( printstring, "ril cha = %d;%d",
                      rilcha[0], rilcha[1] );
            prin_str ( printstring );
          }

        printf ("The gza-bar is: ");
        prn_lst ( tsebar );

        if ( printinprog )
          {
            sprintf ( printstring, "The gza-bar is: %d;%d,%d,%d,%d",
              tsebar[0], tsebar[1], tsebar[2], tsebar[3], tsebar[4] );
            prin_str ( printstring );
          }

        printf ("The nyi-bar is: ");
        prn_lstd ( nyibar, sun_f, 1 );

        if ( printinprog )
          {
            sprintf ( printstring, "The nyi-bar is: %d;%d,%d,%d,%d",
              nyibar[0], nyibar[1], nyibar[2], nyibar[3], nyibar[4] );
            prin_str ( printstring );
          }

        printf ("The gza-dag is: ");
        prn_lst ( gzadag );

        if ( printinprog )
          {
            sprintf ( printstring, "The gza-dag is: %d;%d,%d,%d,%d",
              gzadag[0], gzadag[1], gzadag[2], gzadag[3], gzadag[4] );
            prin_str ( printstring );
          }

        printf ("The nyi-dag is: ");
        prn_lstd ( nyidag, sun_f, 1 );

        if ( printinprog )
          {
            sprintf ( printstring, "The nyi-dag is: %d;%d,%d,%d,%d",
              nyidag[0], nyidag[1], nyidag[2], nyidag[3], nyidag[4] );
            prin_str ( printstring );
          }

// Now, calculate Moon at lunar day and daybreak:
        clrlst (mondag);
        clrlst (monsar);
        mondag[1] = 54; // separation in one lunar day.
        mul_gen ( mondag, mondag, tt, 27, sun_f );
        add_gen ( mondag, nyidag, mondag, 27, sun_f ); // KTC 42
        for ( i = 0; i < 6; ++i )
          monsar[i] = gzadag[i];
        monsar[0] = 0;
        monsar[5] = ( monsar[5] * sun_f ) / gza_f;
        sub_gen ( monsar, mondag, monsar, 27, sun_f); // KTC 43

        sprintf ( printstring,
                  "Moon, lunar day: %d;%d,%d,%d,%d,%d - \
Daybreak: %d;%d,%d,%d,%d,%d.",
                mondag[0], mondag[1], mondag[2], mondag[3], mondag[4],
                mondag[5], monsar[0], monsar[1], monsar[2], monsar[3],
                monsar[4], monsar[5] );

        printf ( "%s\n", printstring );
        if ( printinprog )
          prin_str ( printstring );

        printf ("General day factors: %d, %d\n", spz_frac_b, spz_frac_c );

        if ( printinprog )
          {
            sprintf ( printstring, "General day factors: %d, %d",
                                   spz_frac_b, spz_frac_c );
            prin_str ( printstring );
          }

        do_plans ( spizag );   // In T3.C

        printf ( "SPYI ZHAG = %d. ", spizag );
        printf ( "Julian day = %d. ", juldat );
        printf ( "Western date = %s, %d/%d/%d\n",
                  dayoweek[ doweek ], wd, wm, wy);

        if ( printinprog )
          {
            sprintf ( printstring,
    "General day = %d. Julian day = %d. Western date = %s, %d/%d/%d",
              spizag, juldat, dayoweek[ doweek ], wd, wm, wy);
            prin_str ( printstring );
          }

// To calculate exact Julian day for TRUE lunar day (tithi):
// Julian day at 5.00 LMST:

        jdf_t = (double) juldat - obs_long / 360.0 - 7.0 / 24.0;

// Now add time of true tithi:

        jdf_t = jdf_t + gzadag[1] / 60.0 + gzadag[2] / 3600.0 + gzadag[3] /
                21600.0 + gzadag[4] / ( 21600.0 * (double) gza_f );

// NOW CALCULATE FOR MEAN TITHI:

        if ( tsebar[0] == gzadag[0] )
          juldat_m = juldat;
        else if ( tsebar[0] == gzadag[0] - 1 || tsebar[0] == gzadag[0] + 6 )
          juldat_m = juldat - 1;
        else if ( tsebar[0] == gzadag[0] + 1 || tsebar[0] == gzadag[0] - 6 )
          juldat_m = juldat + 1;
        else
          {
            printf ( "Serious problem with GZA! - bar: %d, dag: %d\n",
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
       
        printf ( "\nNew date, Tshespa/lunar day, Month, Year, Report, Previous lunar day, Exit\n\n" );

        chr = getch ();
        printf ("\n");

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
            if ( tt == 31 )
              {
                if ( epch == 2 && adj_mth == -12 ) // Year will have been decremented.
                  ++ty;
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
        else if ( chr == 'P' || chr == 'p' )  // Previous day
          {
            --tt;
            newmth = 0;
            if ( tt == -1 )
              {
                newmth = 1;
                tt = 29;
                if ( !zeromthfg )
                  --tm;
                if ( tm == 0 )
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
                if ( epch == 2 && adj_mth == -12 ) // Year will have been incremented.
                  ++ty;
               }
          }
        else if ( chr == 'Y' || chr == 'y' )
          {
            ++ty;
            newmth = 1;
            zeromthfg = 0;
          }
      }
  } // END - cal_cyc ()

int main (void)
  {
    int    finish, xit;
    char   chr;
    set_epoch ();  // T2.C
    finish = 0;

    while ( finish == 0 )
      {
        printf ( "\n\n   TIBETAN CALENDAR SOFTWARE\n\n");
        printf ( "    for Tibetan \"grub rtsis\"\n");        
        printf ( "    \"siddhanta calculations\"\n");                
        printf ( "          Version 1.06\n\n");
        printf ( "Use numeric keys to select from the following:\n\n");
        printf ( "   1. Set epoch.\n");
        printf ( "   2. Print calendar.\n");
        printf ( "   3. Calendar cycle.\n");
        printf ( "   4. Run search test.\n");
        printf ( "   5. Calculate New Years.\n");
        printf ( "   6. Calculate yearly tables.\n");        
        printf ( "   9. Finish.\n");
        xit = 0;

        do
          {
            chr = getch ();
            switch ( chr )
              {
                case '1':       
                  set_epoch ();
                  xit = 1;
                  break;
                case '2':       // Print Calendar
                  prn_cal ();
                  xit = 1;
                  break;
                case '3':       
                  cal_cyc ();  // Main calendar cycle
                  xit = 1;
                  break;
               case '4':       
                  srch_cyc ();  // This runs the main test and search routine
                  xit = 1;
                  break;
                case '5':      
                  new_yer ();  // Calculates new years
                  xit = 1;
                  break;
                case '6':      
                  s_cyc ();  // Run table routine
                  xit = 1;
                  break;                  
                case '9':      // FINISH
                  finish = 1;
                  xit = 1;
                  break;
                default:
                  ;
                  break;
              }
          } while ( xit == 0 );
      }
    chk_bcd ();
  } // END - main ()

void new_yer ( void ) // Routine to calculate successive new year days:
  {                   // Reuses existing code. No contrived subtlety.
    int more, n, newmth, lhag, chad;
    int lastjd = 0;
    int curjd; // Previous JD calculations.
    int cycanimx, cycelemx; 
    int lasty;

    fptgt = fopen ( "years.dat", "w" );
    if ( fptgt == NULL )
      {
        printf ( "Error opening target file!\n" );
        getch ();
      }
    
    printf ("\nEnter the starting year: ");
    scanf ("%d", &ty);
    printf ("\n");
    printf ("Enter the finish year: ");
    scanf ("%d", &lasty );
    printf ("\n");
    tm = 1;

// tt not yet set
    lhag = 0; // Before version 1.06 these two were not set, causing
    chad = 0; // the first year sometimes to be derived wrongly.
    n = 0;
    more = 1;
    newmth = 1;
    do
      {
        if ( newmth )
          {
            if ( !zeromthfg )   // We need to use the same data, twice.
              zla_dag (ty, tm); // But only if moving one month at a time!
            adj_zla ();
            cur_mth = zladag[0];
            gza_dru (cur_mth);
            nyi_dru (cur_mth);
            rilchaf ( cur_mth ); // MOVED, from gza_dag, NEW, 11/9/94
          }

        printf ( "Month = %d, Year = %d\n", tm, ty );

        if ( adj_mth != 1 && adj_mth != -1 ) // Also for 1st intercalary,
                                             // such as Phugpa, 1935, 2000 CE???
          goto newmonth;

        tt = 1;
        do
          {   // START OF DAY LOOP
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            curjd = juldat;
            tse_dru (tt-1);
            nyi_lon (tt-1);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            lastjd = juldat;

// Recalculate for current tithi:
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            if ( curjd == lastjd + 2 )
              lhag = 1;
            if ( curjd == lastjd )
              chad = 1;
            if ( tt == 1 && chad ) // Take next date
              {
                // Rough fix for Phugpa 1977 - 1st lunar date omitted -> 19 Feb is correct day
                juldat = juldat + 1;
                jul2date ( juldat ); // This is normally done by spi_zagf
              }

            cycanimx = ( ty - 7 ) % 12;
            cycelemx = ( ty - 5 ) % 10;

            sprintf ( outbuf, "%s-%s Year: %s, %d %s %d.",
                cycelem[cycelemx], cycanim[cycanimx], dayoweek[doweek],
                wd, wmonths[ (wm-1) ], wy );
            if ( adj_mth == -1 ) // Bug fix, for 1st month intercalaries
              tm = 12;
            fprintf ( fptgt, "%s\n", outbuf );
            ++tt;
            newmth = 0;
            lastjd = juldat;
            lhag = 0;
            chad = 0;
          } while ( tt < 2 );    // END OF DAY OOP
        newmonth:
        newmth = 1;
        if ( !zeromthfg )
          ++tm;
        if ( tm > 12 )
          {
            ++ty;
            tm = 1;
          }
      } while ( ty < lasty );  // END OF MONTH LOOP
    fclose ( fptgt );
  } // END - new_yer ()
