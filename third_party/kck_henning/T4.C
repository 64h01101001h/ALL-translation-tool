// T4.C - all routines and data for printing a calendar to disk

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
#include <ctype.h>
#include <conio.h>
#include "tc.h"
#include "tc.ext"
#include "bcd.h"
#include "bcd.ext"

void set_lang ( void );

// Check for special days
int chk_spec ( long int m, long int t )
  {
    switch ( m )
      {
        case 12:
          if ( t == 1L )
            {
              sprintf ( outbuf, "%s",
                "From 1st to 15th, Demonstration of Miracles." );
              return (1);
            }
        break;
        case 2:
          if ( t == 15L )
            {
              sprintf ( outbuf, "%s", "Revelation of the Kalacakra Tantra." );
              return (1);
            }
        break;
        case 3:
          // 7 for Phugpa, 8 for Tsurphu
          if ( t == 8L )
            {
              sprintf ( outbuf, "%s", "Birth of the Buddha." );
              return (1);
            }
          else if ( t == 15L )
            {
              sprintf ( outbuf, "%s",
                "Enlightenment and Parinirvana of the Buddha." );
              return (1);
            }
        break;
        case 5:
          if ( t == 4L )
            {
              sprintf ( outbuf, "%s", "Turning ot the Wheel of the Dharma." );
              return (1);
            }
          else if ( t == 15L )
            {
              sprintf ( outbuf, "%s",
                        "The Buddha's entry into the womb of his mother." );
              return (1);
            }
        break;
        case 8:
          if ( t == 22L )
            {
              sprintf ( outbuf, "%s",
                "Descent of the Buddha from the realm of the gods." );
              return (1);
            }
        break;
        default:
          ;
        break;
      }
    return (0);
  } // END - chk_spec ()

// Set the language used for names of mansions, etc.
void set_lang ( void )
  {
    int i;
/**************
    for ( i = 0; i < 8; ++i )
      cycpar[i] = cycparE[i];
    for ( i = 0; i < 27; ++i )
      lunmanP[i] = lunmanS[i];
    for ( i = 0; i < 27; ++i )
      yogaP[i] = yogaS[i];
    for ( i = 0; i < 11; ++i )
      byedP[i] = byedS[i];
*******************/

/***************/
    for ( i = 0; i < 27; ++i )
      lunmanP[i] = lunmanT[i];
    for ( i = 0; i < 27; ++i )
      yogaP[i] = yogaT[i];
    for ( i = 0; i < 11; ++i )
      byedP[i] = byedT[i];
/**************/
 } // END - set_lang

// Print calendars to disk
void prn_cal ( void )
  {
    int i, more, n, newmth, lhag, chad, nextchad, nextlhag;
    int intercal;
    int yr_gender;    // 1 = female, odd; 0 = male, even
    int mt_gender;    // 1 = female, odd; 0 = male, even
    long int mthnum; // Adjusted, without negative.
    long int cur_year, last_year;
    long int ltt, zla0, zla1;
    long int lastjd = 0L;
    long int curjd, nextjd; // Last and next JD calculations.
    int  lunmanx, sbyorba, byedpa, byedpax;
    int  prv_lunmanx, prv_sbyorba, prv_byedpa, prv_byedpax;
    long int Cmonth;
    long int sd_clunmanx;
    long int ld_parx; // sd_parx;
    long int ld_smex, sd_smex;
    long int yr_animx, mt_animx, ld_animx, sd_animx;
    long int yr_elemx, mt_elemx, /*ld_elemx,*/ sd_elemx;
    long int ZS_s, ZS_d, ZS_m;  // Zodiac signs, minutes, seconds;
    int e, g;
    int new_year;

    set_lang ();
    gza_short_flg = 0; // Flag for less accurate weekday adjustment
    cls ();
//  Changed for Win32 console app:    
//  pos (0,0);
    printf ( "%s\n", e_str[epch] );
    printf ("Enter the starting year: ");
    scanf ("%ld", &ty);
    printf ("\n");

    cur_year = ty;

    printf ("Enter the finish year: ");
    scanf ("%ld", &last_year );
    printf ("\n");

donextyear:

    new_year = 1;
    ty = cur_year;
    tm = 1L;
    zeromthfg = 0;

    sprintf ( filenam, "kc_%ld.txt", ty );

    fptgt = fopen ( filenam, "w" );
    if ( fptgt == NULL )
      {
        printf ( "Error opening target file!\n" );
        getch ();
      }

// tt not yet set
// First, check to see if we are not at adjusted 12th month:
// This gave problems with earlier versions of TC.EXE

    zla_dag (ty, tm); // But only if moving one month at a time!
    zla0 = zladag[0]; 
    zla1 = zladag[1];
    adj_zla ();

    if ( adj_mth == 12L )
      {
	if ( !zeromthfg )  
          ++tm;
      }
    else if ( adj_mth == -1L )
// This now works, and intercalary first months are detected properly
// There may be problems now, though. CHECK ALL THIS!!!
      {
        zladag[0] = zla0;
        zladag[1] = zla1;
        zeromthfg = 0;
      }
    n = 0;
    more = 1;
    newmth = 1;
    do   // START OF MONTH LOOP
      {
        if ( adj_mth == -1L )
          intercal = 1;
        else
          intercal = 0;        
        if ( newmth )
          {
            if ( !zeromthfg )   // We need to use the same data, twice.
              zla_dag (ty, tm); // But only if moving one month at a time!
            adj_zla ();
            cur_mth = zladag[0];  // Needed for general day
            gza_dru (cur_mth);
            nyi_dru (cur_mth);
            rilchaf ( cur_mth ); 

            // Do we have new year?

            if ( ( adj_mth == -1 || adj_mth == 1 ) && new_year )
              {
                if ( ty < 7L )
                  {
                    yr_animx = ( ty + 1200L - 7L ) % 12L;
                    yr_elemx = ( ( ty + 1200L ) / 2L - 3L ) % 5L;
                    yr_gender = (int) ( ty + 1200L ) % 2; // 1 = female, odd
                  }
                else
                  {
                    yr_animx = ( ty - 7L ) % 12L;
                    yr_elemx = ( ty / 2L - 3L ) % 5L;
                    yr_gender = (int) ty % 2; // 1 = female, odd; 0 = male
                  }
                sprintf ( outbuf, "\nNew Year: %ld, %s-%s-%s\n", ty,
                                  cycelem1[ (int) yr_elemx ],
                                  cycgendE[ yr_gender ],
                                  cycanim[ (int) yr_animx ] );
                fprintf ( fptgt, "%s", outbuf );
                new_year = 0;
              }
          }

        if ( adj_mth < 0L )
          intercal = 1;
        else if ( adj_mth > 0L && ( zladag[1] == 0 || zladag[1] == 1 ) )
          intercal = 0;
        if ( adj_mth < 0L )
          mthnum = -adj_mth;
        else
          mthnum = adj_mth;

        printf ("The ZLA-DAG used is: %ld;%ld\n", zladag[0], zladag[1]);

        if ( tsurlug == 1 ) // Which to use here? Currently set to tsurlug.
          {
            mt_animx = ( mthnum + 10L ) % 12L;
            Cmonth = mthnum;
          }
        else
          {
            mt_animx = mthnum % 12L;
            Cmonth = mthnum + 2L;
            if ( Cmonth > 12L )
              Cmonth -= 12L;
          }

        if ( tsurlug == 1 ) 
          {
//      yr_gender - 1 = female, odd; 0 = male, even
// ORDER IS: "Fire", "Earth", "Iron", "Water", "Wood"
//             0        1       2        3       4
            e = (int) yr_elemx; g = yr_gender;
            if ( ( e == 4 && g == 0 ) || ( e == 1 && g == 1 ) )
              mt_elemx = 0L + ( mthnum - 1L ) / 2L;
            if ( ( e == 4 && g == 1 ) || ( e == 2 && g == 0 ) )
              mt_elemx = 1L + ( mthnum - 1L ) / 2L;
            if ( ( e == 0 && g == 0 ) || ( e == 2 && g == 1 ) )
              mt_elemx = 2L + ( mthnum - 1L ) / 2L;
            if ( ( e == 1 && g == 0 ) || ( e == 3 && g == 1 ) )
              mt_elemx = 4L + ( mthnum - 1L ) / 2L;
            if ( ( e == 0 && g == 1 ) || ( e == 3 && g == 0 ) )
              mt_elemx = 3L + ( mthnum - 1L ) / 2L;
          }
        else
          mt_elemx = yr_elemx + 1L + ( mthnum + 1L ) / 2L; 

        if ( yr_gender == 0 ) // Male year, even CE number
          {
            if ( mthnum == 11L || mthnum == 12L )
              mt_elemx = yr_elemx + ( (int) mthnum + 1L ) / 2;
          }

        mt_elemx = mt_elemx % 5L;
        mt_gender = ( mthnum + 1 ) % 2; // There is no month zero

        if ( intercal )
          sprintf ( outbuf,
          "Lunar Month: %ld (Intercalary) - %s-%s-%s - %s",
          -adj_mth, cycelem1[ (int) mt_elemx ],
          cycgendE[ mt_gender ], cycanim[(int) mt_animx], lunmont[adj_mth-1] );
        else
          sprintf ( outbuf, "Lunar Month: %ld - %s-%s-%s - %s",
          adj_mth, cycelem1[ (int) mt_elemx ],
          cycgendE[ mt_gender ], cycanim[(int) mt_animx], lunmont[adj_mth-1] );

        printf ( "%s\n", outbuf );
        fprintf ( fptgt, "\n%s\n\n", outbuf );
        tt = 1L;
        ltt = 1L;
        do
// START OF DAY LOOP - START OF DAY LOOP - START OF DAY LOOP
          {   
            tt = ltt;
            nextlhag = 0;
            lhag = 0;
            chad = 0;
            nextchad = 0;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7L, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27L, sun_f );
            nyi_dag (nyibar); // Must be done first
            gza_dag (tsebar);
            spi_zagf ();
// Work out previous and next days to determine CHAD or LHAG
            curjd = juldat;
            tt = ltt - 1L;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7L, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27L, sun_f );
            nyi_dag (nyibar); // Must be done first 
            gza_dag (tsebar);
            spi_zagf ();
            lastjd = juldat;

            for ( i = 0; i < 6; ++i )
              {
                prv_nyidag[i] = nyidag[i];
                prv_gzadag[i] = gzadag[i];
              }

            tt = ltt + 1L;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7L, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27L, sun_f );
            nyi_dag (nyibar); // Must be done first
            gza_dag (tsebar);
            spi_zagf ();
            nextjd = juldat;

// Save for later, in case we have a duplicated, LHAG, day:

            for ( i = 0; i < 6; ++i )
              {
                nxt_nyidag[i] = nyidag[i];
                nxt_gzadag[i] = gzadag[i];
              }

// Recalculate for current lunar day, tithi:

            tt = ltt;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7L, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27L, sun_f );
            nyi_dag (nyibar); // Must be done first
            gza_dag (tsebar);
            spi_zagf ();
            if ( curjd == lastjd + 2L )
              lhag = 1;
            if ( nextjd == curjd + 2L )
              nextlhag = 1;
            if ( curjd == lastjd )
              chad = 1;
            if ( curjd == nextjd )
              nextchad = 1;

// Calculate lunar mansion at sunrise:
// add solar longitude to tithi * factor. Then subtract hours, etc., from gzadag

    clear_a_b ();
    for ( i = 0; i < 5; ++i )
      monlong[i] = 0L;

    monlong[1] = 54L;  // 1/30th of a revolution
    mul_gen ( monlong, monlong, tt, 27L, sun_f );
    add_gen ( monlong, monlong, nyidag, 27L, sun_f );
    listb[0] = 0L;
    for ( i = 1; i < 5; ++i )
      listb[i] = gzadag[i];
    listb[4] = ( sun_f * listb[4] ) / gza_f;
// This gives longitude at daybreak:
    sub_gen ( monlong, monlong, listb, 27L, sun_f );

    lunmanx = (int) monlong[0];

// Now calculate yoga, sbyor ba:
// This is strictly wrong, we need the sun's longitude at sunrise, but in the
// Tibetan tradition such an adjustment is not made

    add_gen ( lista, monlong, nyidag, 27L, sun_f );
    sbyorba = (int) lista[0];

    for ( i = 0; i < 6; ++i )
      sbyorlong[i] = lista[i];

// Now calculate karana, byed pa:

    clear_a_b ();
    sub_gen ( lista, monlong, nyidag, 27L, sun_f ); // at sunrise for Moon
    byedpa = (int) lista[0] * 60 + (int) lista[1];
    byedpa = byedpa / 27;   
    if ( byedpa == 0 )
      byedpax = 7;
    else if ( byedpa == 57 )
      byedpax = 8;
    else if ( byedpa == 58 )
      byedpax = 9;
    else if ( byedpa == 59 )
      byedpax = 10;
    else
      byedpax = ( byedpa - 1 ) % 7;

// Solar day data:

            sd_animx = ( juldat - 2L ) % 12L;
            sd_elemx = ( ( juldat - 3L ) / 2L ) % 5L;
            sd_clunmanx = ( juldat - 17L ) % 28L;
            sd_smex = ( juldat - 2L ) % 9L + 1L; 

// Lunar day data:

// If Chinese month is number 1, Trigram is Li, index = 1
            ld_parx = ( Cmonth - 1L ) * 30L + tt;
            ld_parx = ld_parx % 8L;
// If Chinese month is number 1, "sme ba" is 1
            ld_smex = ( Cmonth - 1L ) * 30L + tt;
            ld_smex = ld_smex % 9L;
            if ( ld_smex == 0L )
              ld_smex = 9L;
// If Chinese month is number 1, Animal is Tiger, index = 11
            ld_animx = ( Cmonth - 1L ) * 30L + tt + 10L;
            ld_animx = ld_animx % 12L;

            if ( chad )   // THIS IS FOR OMITTED LUNAR DAY
                          // REPLACED OK
              {
                sprintf ( outbuf, "%ld. Omitted: %s %s %ld",
                          tt,
                          cycanim[ (int) ld_animx ],
                          cycpar[ (int) ld_parx ], ld_smex );

// Should include "gza'" for end of lunar day, plus solar longitude.

                fprintf ( fptgt, "%s\n", outbuf );
              }
            else  // IF NORMAL OR DUPLICATED:
              {
// nyidag is not changed after here, so we can calculate zodiacal sign:
// Zodiac signs, minutes, seconds -  ZS_s, ZS_d, ZS_m;

                ZS_m = ( ( nyibar[0] * 60L + nyibar[1] ) * 60L + nyibar[2] ) *
                         6L + nyibar[3];
                l2bcd ( bcdx0, ZS_m );
                mulbcdl ( bcdx0, bcdx0, 21600L );
                divbcdl ( bcdx1, bcdx0, 583200L );
                ZS_m = bcd2l ( bcdx1 );

                ZS_d = ZS_m / 60L;
                ZS_m = ZS_m % 60L;
                ZS_s = ZS_d / 30L;
                ZS_d = ZS_d % 30L;
                if ( lhag )  // IF THIS IS THE FIRST OF TWO DUPLICATES.
                {
                // Put here any special calculations for extra day
                // At this point, the gzadag, nyidag & monlong are correct for
                // the second of the two days with same tithi. We need for the
                // first
                // Previous tithi ended just before start of day we need.
                clear_a_b ();
                for ( i = 0; i < 5; ++i )
                  prv_monlong[i] = 0L;
                prv_monlong[1] = 54L;

                mul_gen ( prv_monlong, prv_monlong, tt, 27L, sun_f );
                add_gen ( prv_monlong, prv_monlong, nyidag, 27L, sun_f );
                listb[0] = 1L;
                sub_gen ( prv_monlong, prv_monlong, listb, 27L, sun_f );

                prv_lunmanx = (int) prv_monlong[0];

                 // Now calculate yoga, sbyor ba:

                  add_gen ( lista, prv_monlong, nyidag, 27L, sun_f );
                  prv_sbyorba = (int) lista[0];
                  for ( i = 0; i < 6; ++i )
                    prv_sbyorlong[i] = lista[i];

                // Now calculate karana, byed pa:

                  clear_a_b ();
                  sub_gen ( lista, prv_monlong, prv_nyidag, 27L, sun_f );
                  prv_byedpa = (int) lista[0] * 60 + (int) lista[1];
                  prv_byedpa = prv_byedpa / 27; 
                  if ( prv_byedpa == 0 )
                    prv_byedpax = 7;
                  else if ( prv_byedpa == 57 )
                    prv_byedpax = 8;
                  else if ( prv_byedpa == 58 )
                    prv_byedpax = 9;
                  else if ( prv_byedpa == 59 )
                    prv_byedpax = 10;
                  else
                    prv_byedpax = ( prv_byedpa - 1 ) % 7;

                juldat = juldat - 1L;
                jul2date ( juldat ); // This is normally done by spi_zagf

// Solar day data:

            sd_animx = ( juldat - 2L ) % 12L;
            sd_elemx = ( ( juldat - 3L ) / 2L ) % 5L;
            sd_clunmanx = ( juldat - 17L ) % 28L;
            sd_smex = ( juldat - 2L ) % 9L + 1L;  

                // LINE 1 - First of duplicated day
                sprintf ( outbuf, "%ld: %s. %s. %s-%s; %ld %s %ld",
                          tt,
                          dayoweek[ (int) doweek ],
                          lunmanP[ prv_lunmanx ],
                          fourelem[ dowkelem[ (int) doweek ] ],
                          fourelem[ lunmanelem[ prv_lunmanx ] ],
                          wd, wmonths[ (int) (wm-1L) ], wy);
                  fprintf ( fptgt, "%s\n", outbuf );

                // LINE 2.  First of duplicated day
                sprintf ( outbuf, "%s, %s, %s, %s %ld",
                          yogaP[ prv_sbyorba ],
                          byedP[ prv_byedpax ],
                          cycanim[ ld_animx ],
                          cycpar[ (int) ld_parx ], ld_smex );
                fprintf ( fptgt, "\0x20\0x20%s\n", outbuf );

                // LINE 3. First of duplicated day
                if ( tsurlug == 1 )
                {
                sprintf ( outbuf,
                    "%ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld",
                       ( prv_gzadag[0] + 1L ) % 7L, 60L, 0L,
                       prv_monlong[0], prv_monlong[1], prv_monlong[2],
                       nyidag[0], nyidag[1], nyidag[2],
                       prv_sbyorlong[0], prv_sbyorlong[1], prv_sbyorlong[2] );
                }
                else
                {
                sprintf ( outbuf,
                "%ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld",
                       ( prv_gzadag[0] + 1L ) % 7L, 60L, 0L,
                       prv_monlong[0], prv_monlong[1], prv_monlong[2],
                       nyidag[0], nyidag[1], nyidag[2],
                       prv_sbyorlong[0], prv_sbyorlong[1], prv_sbyorlong[2],
                       ZS_s, ZS_d, ZS_m );
                }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

                // LINE 4. First of duplicated day
                if ( tsurlug == 1 )
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s" ,
                              cycelem1[ (int) sd_elemx ],
                              cycanim[ (int) sd_animx ],
                              Clunman[ sd_clunmanx ] );
                  }
                else
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s %ld" ,
                              cycelem1[ (int) sd_elemx ],
                              cycanim[ (int) sd_animx ],
                              Clunman[ sd_clunmanx ],
                              sd_smex );
                  }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

          // Now do second of duplicated lunar days

                  juldat = juldat + 1L;  // This is readjustment!
                  jul2date ( juldat );

// Solar day data:  

            sd_animx = ( juldat - 2L ) % 12L;
            sd_elemx = ( ( juldat - 3L ) / 2L ) % 5L;
            sd_clunmanx = ( juldat - 17L ) % 28L;
            sd_smex = ( juldat - 2L ) % 9L + 1L;  

                // LINE 1. Second of duplicated day
                sprintf ( outbuf, "%ld: %s. %s. %s-%s; %ld %s %ld",
                          tt,
                          dayoweek[ (int) doweek ],
                          lunmanP[ lunmanx ],
                          fourelem[ dowkelem[ (int) doweek ] ],
                          fourelem[ lunmanelem[ lunmanx ] ],
                          wd, wmonths[ (int) (wm-1L) ], wy);

                  fprintf ( fptgt, "%s\n", outbuf );

                // LINE 2.  Second of duplicated day
                sprintf ( outbuf, "%s, %s, %s, %s %ld",
                          yogaP[ sbyorba ],
                          byedP[ byedpax ],
                          cycanim[ ld_animx ],
                          cycpar[ (int) ld_parx ], ld_smex );
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

                // LINE 3.  Second of duplicated day
                if ( tsurlug == 1 )
                {
                sprintf ( outbuf,
                      "%ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld",
                          gzadag[0], gzadag[1], gzadag[2],
                          monlong[0], monlong[1], monlong[2],
                          nyidag[0], nyidag[1], nyidag[2],
                          sbyorlong[0], sbyorlong[1], sbyorlong[2] );
                }
                else
                {
                sprintf ( outbuf,
                "%ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld",
                          gzadag[0], gzadag[1], gzadag[2],
                          monlong[0], monlong[1], monlong[2],
                          nyidag[0], nyidag[1], nyidag[2],
                          sbyorlong[0], sbyorlong[1], sbyorlong[2],
                          ZS_s, ZS_d, ZS_m );
                }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

                // LINE 4. Second of duplicated day
                if ( tsurlug == 1 )
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s" ,
                              cycelem1[ (int) sd_elemx ],
                              cycanim[ (int) sd_animx ],
                              Clunman[ sd_clunmanx ] );
                  }
                else
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s %ld" ,
                              cycelem1[ (int) sd_elemx ],
                              cycanim[ (int) sd_animx ],
                              Clunman[ sd_clunmanx ],
                              sd_smex );
                  }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );
                // LINE 5.  Second if duplicated day
                sprintf ( outbuf, "Duplicated lunar day" );
                printf ( "%s\n", outbuf );
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );
                } // End of if ( lhag )
              else  // NORMAL DAY 
                {
                // LINE 1.
                sprintf ( outbuf, "%ld: %s. %s. %s-%s; %ld %s %ld",
                          tt,
                          dayoweek[ (int) doweek ],
                          lunmanP[ lunmanx ],
                          fourelem[ dowkelem[ (int) doweek ] ],
                          fourelem[ lunmanelem[ lunmanx ] ],
                          wd, wmonths[ (int) (wm-1L) ], wy);
                fprintf ( fptgt, "%s\n", outbuf );

                // LINE 2. NORMAL DAY
                sprintf ( outbuf, "%s, %s, %s, %s %ld",
                          yogaP[ sbyorba ],
                          byedP[ byedpax ],
                          cycanim[ ld_animx ],
                          cycpar[ (int) ld_parx ], ld_smex );
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );
                // LINE 3. NORMAL DAY
                if ( tsurlug == 1 )
                {
                sprintf ( outbuf,
                  "%ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld",
                          gzadag[0], gzadag[1], gzadag[2],
                          monlong[0], monlong[1], monlong[2],
                          nyidag[0], nyidag[1], nyidag[2],
                          sbyorlong[0], sbyorlong[1], sbyorlong[2] );
                }
                else
                {
                sprintf ( outbuf,
                "%ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld %ld;%ld,%ld",
                          gzadag[0], gzadag[1], gzadag[2],
                          monlong[0], monlong[1], monlong[2],
                          nyidag[0], nyidag[1], nyidag[2],
                          sbyorlong[0], sbyorlong[1], sbyorlong[2],
                          ZS_s, ZS_d, ZS_m );
                }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

                // LINE 4. NORMAL DAY
                if ( tsurlug == 1 )
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s" ,
                              cycelem1[ (int) sd_elemx ],
                              cycanim[ (int) sd_animx ],
                              Clunman[ sd_clunmanx ] );
                  }
                else
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s %ld" ,
                              cycelem1[ (int) sd_elemx ],
                              cycanim[ (int) sd_animx ],
                              Clunman[ sd_clunmanx ],
                              sd_smex );
                  }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );
                }

                if ( chk_spec ( mthnum, tt ) )
                  {
                    printf ( "  %s\n", outbuf );
                    fprintf ( fptgt, "  %s\n", outbuf );
                  }
              }

            ++ltt;
            newmth = 0;
            lastjd = juldat;
          } while ( ltt <= 30 ); 

// END OF DAY LOOP - END OF DAY LOOP - END OF DAY LOOP - END OF DAY LOOP -

        newmth = 1;
        if ( !zeromthfg )
          ++tm;
        if ( tm > 12 )
          {
            ++ty;
            tm = 1;
          }
        if ( adj_mth == 12L )
          {
	    // First check for following intercalary:
            if ( zladag[1] == 63L || zladag[1] == 64L )
              more = 1;
            else
              more = 0; // We have finished.
          }
      } while ( more );        // END OF MONTH LOOP
    fclose ( fptgt );

    if ( cur_year < last_year )
      {
        ++cur_year;
        goto donextyear;
      }

  } // END - prn_cal ()
