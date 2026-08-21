// T4.C - all routines and data for printing to disk a full calendar

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
#include <string.h>
#include <conio.h>
#include "tc.h"
#include "tc.ext"
#include "bcd.h"
#include "bcd.ext"

int yan_kwong_flg = 0;
int zin_phung_flg = 0;
int klu_bzlog_flg = 0;
int klu_thebs_flg = 0;
int nyi_nag_flg = 0;
int sadag_str_len = 0;
char sadag_str[45];

void set_lang ( void );

// Routine to check for special days, festivals, anniversaries, etc.
// Many are currently commented out. Uncomment if needed.
// Month numbers would currently be wrong for Error Correction system.
int chk_spec ( int m, int t )
  {
    switch ( m )
      {
        case 1:
          if ( t == 1 )
            {
              sprintf ( outbuf, "%s",
                "From 1st to 15th, Demonstration of Miracles." );
              return (1);
            }
//        else if ( t == 14 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Milarepa." );
//            return (1);
//          }
//        else if ( t == 15 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Marpa." );
//            return (1);
//          }
        break;
//      case 2:
//        if ( t == 26 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Taranatha." );
//            return (1);
//          }
//      break;
        case 3:
//        if ( t == 3 )
//          {
//            sprintf ( outbuf, "%s", "Anniversary of Karma Pakshi." );
//            return (1);
//          }
          if ( t == 15 )
            {
              sprintf ( outbuf, "%s", "Revelation of the Kalacakra Tantra." );
              return (1);
            }
        break;
        case 4:
          // 7 for Phugpa, 8 for Tsurphu
          if ( t == 7 && tsurlug == 0 )
            {
              sprintf ( outbuf, "%s", "Birth of the Buddha." );
              return (1);
            }
          if ( t == 8 && tsurlug == 1 )
            {
              sprintf ( outbuf, "%s", "Birth of the Buddha." );
              return (1);
            }
          else if ( t == 15 )
            {
              sprintf ( outbuf, "%s",
                "Enlightenment and Parinirvana of the Buddha." );
              return (1);
            }
//       else if ( t == 29 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Mipham." );
//            return (1);
//          }
        break;
//      case 5:
//        if ( t == 10 )
//          {
//            sprintf ( outbuf, "%s", "Birth of Guru Rinpoche." );
//            return (1);
//          }
//        if ( t == 15 )
//          {
//            sprintf ( outbuf, "%s", "World Local Deities Day." );
//            return (1);
//          }
//      break;
        case 6:
          if ( t == 4 )
            {
              sprintf ( outbuf, "%s", "Turning of the Wheel of the Dharma." );
              return (1);
            }
          else if ( t == 15 )
            {
              sprintf ( outbuf, "%s",
                        "The Buddha's entry into the womb of his mother." );
              return (1);
            }
        break;
//      case 8:
//        if ( t == 19 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Dilgo Khyentse Rinpoche." );
//            return (1);
//          }
//        else if ( t == 23 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Karmapa Mikyo Dorje." );
//            return (1);
//          }
//      break;
        case 9:
//        if ( t == 16 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of 16th Karmapa." );
//            return (1);
//          }
          if ( t == 22 )
            {
              sprintf ( outbuf, "%s",
                "Descent of the Buddha from the realm of the gods." );
              return (1);
            }
        break;
//      case 10:
//        if ( t == 10 )
//          {
//            sprintf ( outbuf, "%s",
//              "White Tara day." );
//            return (1);
//          }
//        else if ( t == 25 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Tshongkhapa." );
//            return (1);
//          }
//      break;
//      case 11:
//        if ( t == 3 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of First Karmapa." );
//            return (1);
//          }
//        else if ( t == 6 )
//          {
//            sprintf ( outbuf, "%s",
//              "Nine bad omens." );
//            return (1);
//          }
//        else if ( t == 7 )
//          {
//            sprintf ( outbuf, "%s",
//              "Ten auspicious omens." );
//            return (1);
//          }
//        else if ( t == 26 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Jamgon Kongtrul." );
//            return (1);
//          }
//        break;
//      case 12:
//        if ( t == 18 )
//          {
//            sprintf ( outbuf, "%s",
//              "Anniversary of Longchen Rabjampa." );
//            return (1);
//          }
//        else if ( t == 29 )
//          {
//            sprintf ( outbuf, "%s",
//              "Mahakala Rituals." );
//            return (1);
//          }
//      break;
        default:
          ;
        break;
      }
    return (0);
  } // END - chk_spec ()

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

void prn_cal ( void )
  {
    int i, more, n, newmth, lhag, chad, nextchad, nextlhag;
    int intercal, delaymth;
    int yr_gender;    // 1 = female, odd; 0 = male, even
    int mt_gender;    // 1 = female, odd; 0 = male, even
    int mthnum; // Adjusted, without negative.
    int cur_year, last_year;
    int ltt, zla0, zla1;
    int lastjd = 0;
    int curjd, nextjd; // Last and next JD calculations.
    int  lunmanx, sbyorba, byedpa, byedpax;
    int  prv_lunmanx, prv_sbyorba, prv_byedpa, prv_byedpax;
    int Cmonth;
    int sd_clunmanx;
    int ld_parx; // sd_parx;
    int ld_smex, sd_smex;
    int yr_animx, mt_animx, ld_animx, sd_animx;
    int yr_elemx, mt_elemx, /*ld_elemx,*/ sd_elemx;
    int ZS_s, ZS_d, ZS_m;  // Zodiac signs, minutes, seconds;
    int e, g;
    int new_year;

    set_lang ();
    gza_short_flg = 0; // Flag for less accurate weekday adjustment
    printf ( "\n\n%s\n", e_str[epch] );
    printf ("Enter the starting year: ");
    scanf ("%d", &ty);
    printf ("\n");

    cur_year = ty;

    printf ("Enter the finish year: ");
    scanf ("%d", &last_year );
    printf ("\n");

donextyear:

    new_year = 1;
    ty = cur_year;
    tm = 1;
    zeromthfg = 0;

    if ( tsurlug == 1 )
      sprintf ( filenam, "ts_%d.txt", ty );
    else if ( epch == 0 )
      sprintf ( filenam, "pl_%d.txt", ty );
    else if ( epch == 2 )
      sprintf ( filenam, "ec_%d.txt", ty );

    fptgt = fopen ( filenam, "w" );
    if ( fptgt == NULL )
      {
        printf ( "Error opening target file!\n" );
        getch ();
      }

// tt not yet set
// First, check to see if we are not at adjusted 12th month:

    zla_dag (ty, tm); // But only if moving one month at a time!

    zla0 = zladag[0];
    zla1 = zladag[1];

    adj_zla ();
    if ( adj_mth == 12 )
      {
        if ( !zeromthfg ) // Should always work
          ++tm;
      }
    else if ( adj_mth == -1 )
      {
        zladag[0] = zla0;
        zladag[1] = zla1;
        zeromthfg = 0;
      }
    n = 0;
    more = 1;
    newmth = 1;
    do   // START OF MONTH LOOP *******************************
      {
        intercal = 0;
        if ( adj_mth == -1 )
          intercal = 1;
        delaymth = 0;
        if ( newmth )
          {
            if ( !zeromthfg )   // We need to use the same data, twice.
              zla_dag (ty, tm); // But only if moving one month at a time!
            adj_zla ();
            cur_mth = zladag[0];  // Needed for general day
            gza_dru (cur_mth);
            nyi_dru (cur_mth);
            rilchaf ( cur_mth ); // MOVED, from gza_dag, NEW, 11/9/94

// DO WE HAVE NEW YEAR?
            if ( ( adj_mth == -1 || adj_mth == 1 ) && new_year )
              {
                if ( ty < 7 )
                  {
                    yr_animx = ( ty + 1200 - 7 ) % 12;
                    yr_elemx = ( ( ty + 1200 ) / 2 - 3 ) % 5;
                    yr_gender = ( ty + 1200 ) % 2; // 1 = female, odd; 0
                  }
                else
                  {
                    yr_animx = ( ty - 7 ) % 12;
                    yr_elemx = ( ty / 2 - 3 ) % 5;
                    yr_gender = ty % 2; // 1 = female, odd; 0 = male
                  }
                sprintf ( outbuf, "\nNew Year: %d, %s-%s-%s\n", ty,
                                  cycelem1[ yr_elemx ],
                                  cycgendE[ yr_gender ],
                                  cycanim[ yr_animx ] );
                fprintf ( fptgt, "%s", outbuf );
                new_year = 0;
              }
          }
        if ( zlapure == 0 ) // Phugpa grub rtsis
          {
            if ( adj_mth < 0 ) // Intercalary
              {
                intercal = 1;
                delaymth = 0;
              } 
            else if ( zladag[1] == zlasho1+2 || zladag[1] == zlasho2+2 ) // Changed, 1/9/2010
              {
                intercal = 0;
                delaymth = 1;
              }
          }
        else
          {
            if ( adj_mth < 0 ) // Intercalary
              {
                intercal = 1;
                delaymth = 0;
              }
            else if ( adj_mth > 0 && ( zladag[1] == 0 || zladag[1] == 1 ) )
              {
                intercal = 0;
                delaymth = 1;
              }
          }

        if ( adj_mth < 0 )
          mthnum = -adj_mth;
        else
          mthnum = adj_mth;

        if ( tsurlug == 1 )
          {
            mt_animx = ( mthnum + 10 ) % 12;
            Cmonth = mthnum;
          }
        else
          {
            mt_animx = mthnum % 12;
            Cmonth = mthnum + 2;
            if ( Cmonth > 12 )
              Cmonth -= 12;
          }

        if ( tsurlug == 1 ) 
          {
//      yr_gender - 1 = female, odd; 0 = male, even
// ORDER IS: "Fire", "Earth", "Iron", "Water", "Wood"
//             0        1       2        3       4
            e = yr_elemx; g = yr_gender;
            if ( ( e == 4 && g == 0 ) || ( e == 1 && g == 1 ) )
              mt_elemx = 0 + ( mthnum - 1 ) / 2;
            if ( ( e == 4 && g == 1 ) || ( e == 2 && g == 0 ) )
              mt_elemx = 1 + ( mthnum - 1 ) / 2;
            if ( ( e == 0 && g == 0 ) || ( e == 2 && g == 1 ) )
              mt_elemx = 2 + ( mthnum - 1 ) / 2;
            if ( ( e == 1 && g == 0 ) || ( e == 3 && g == 1 ) )
              mt_elemx = 4 + ( mthnum - 1 ) / 2;
            if ( ( e == 0 && g == 1 ) || ( e == 3 && g == 0 ) )
              mt_elemx = 3 + ( mthnum - 1 ) / 2;
          }
        else
          mt_elemx = yr_elemx + 1 + ( mthnum + 1 ) / 2;

        if ( yr_gender == 0 ) // Male year, even CE number 
          {
            if ( mthnum == 11 || mthnum == 12 )
              mt_elemx = yr_elemx + ( mthnum + 1 ) / 2;
          }

        mt_elemx = mt_elemx % 5;
        mt_gender = ( mthnum + 1 ) % 2; // There is no month zero

        if ( intercal )
          sprintf ( outbuf,
          "Tibetan Lunar Month: %d (Intercalary) - %s-%s-%s",
          -adj_mth, cycelem1[ mt_elemx ],
          cycgendE[ mt_gender ], cycanim[ mt_animx ] );

        else if ( epch != 2 && delaymth )
          sprintf ( outbuf, "Tibetan Lunar Month: %d (Delayed) - %s-%s-%s",
          adj_mth, cycelem1[ mt_elemx ],
          cycgendE[ mt_gender ], cycanim[ mt_animx ] );

        else
          sprintf ( outbuf, "Tibetan Lunar Month: %d - %s-%s-%s",
          adj_mth, cycelem1[ mt_elemx ],
          cycgendE[ mt_gender ], cycanim[ mt_animx ] );

        printf ( "%s\n", outbuf );
        fprintf ( fptgt, "\n%s\n\n", outbuf );
        tt = 1;
        ltt = 1;
// START OF DAY LOOP - START OF DAY LOOP - START OF DAY LOOP **********************
        do
          {  
            tt = ltt;
            nextlhag = 0;
            lhag = 0;
            chad = 0;
            nextchad = 0;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();

// Work out previous and next days to determine CHAD or LHAG
            curjd = juldat;
            tt = ltt - 1;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            lastjd = juldat;

            for ( i = 0; i < 6; ++i )
              {
                prv_nyidag[i] = nyidag[i];
                prv_gzadag[i] = gzadag[i];
              }

            tt = ltt + 1;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            nextjd = juldat;

            // Save for later, in case we have a LHAG day:

            for ( i = 0; i < 6; ++i )
              {
                nxt_nyidag[i] = nyidag[i];
                nxt_gzadag[i] = gzadag[i];
              }

// Recalculate for current tithi: 

            tt = ltt;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            if ( curjd == lastjd + 2 )
              lhag = 1;
            if ( nextjd == curjd + 2 )
              nextlhag = 1;
            if ( curjd == lastjd )
              chad = 1;
            if ( curjd == nextjd )
              nextchad = 1;

// Calculate lunar mansion at daybreak:
// add solar longitude to tithi * factor. Then subtract hours, etc., from gzadag

    clear_a_b ();
    for ( i = 0; i < 5; ++i )
      monlong[i] = 0;

    monlong[1] = 54;  // 1/30th of a revolution
    mul_gen ( monlong, monlong, tt, 27, sun_f );
    add_gen ( monlong, monlong, nyidag, 27, sun_f );
    listb[0] = 0;
    for ( i = 1; i < 5; ++i )
      listb[i] = gzadag[i];
    listb[4] = ( sun_f * listb[4] ) / gza_f;
// This gives longitude at daybreak:
    sub_gen ( monlong, monlong, listb, 27, sun_f );

    lunmanx = monlong[0];

// Now calculate yoga, sbyor ba:

// This is strictly wrong, we should use the Sun's longitude at daybreak, 
// but in the Tibetan tradition such an adjustment is not made

    add_gen ( lista, monlong, nyidag, 27, sun_f );
    sbyorba = lista[0];

    for ( i = 0; i < 6; ++i )
      sbyorlong[i] = lista[i];

// Now calculate karana, byed pa:

    clear_a_b ();
    sub_gen ( lista, monlong, nyidag, 27, sun_f ); // At daybreak for moon
    byedpa = lista[0] * 60 + lista[1];
    byedpa = byedpa / 27;     // OK
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

        sd_animx = ( juldat - 2 ) % 12;
        sd_elemx = ( ( juldat - 3 ) / 2 ) % 5;
        sd_clunmanx = ( juldat - 17 ) % 28;
        sd_smex = ( juldat - 2 ) % 9 + 1; 

// Lunar day data:

// If Chinese month is number 1, Trigram is Li, index = 1
        ld_parx = ( Cmonth - 1 ) * 30 + tt;
        ld_parx = ld_parx % 8;
// If Chinese month is number 1, "sme ba" is 1
        ld_smex = ( Cmonth - 1 ) * 30 + tt;
        ld_smex = ld_smex % 9;
        if ( ld_smex == 0 )
          ld_smex = 9;
// If Chinese month is number 1, Animal is Tiger, index = 11
        ld_animx = ( Cmonth - 1 ) * 30 + tt + 10;
        ld_animx = ld_animx % 12;

            if ( chad )   // This is for omitted lunar day
              {
                sprintf ( outbuf, "%d. Omitted: %s %s %d",
                          tt,
                          cycanim[ ld_animx ],
                          cycpar[ ld_parx ], ld_smex );
// Should include "gza'" for end of lunar day, plus solar longitude.
                fprintf ( fptgt, "%s\n", outbuf );
              }
            else  // If normal or duplicated:
              {
// NYIDAG is not changed after here, so we can now calculate zodiacal sign:
// Zodiac signs, degrees, minutes -  ZS_s, ZS_d, ZS_m
                ZS_m = ( ( nyidag[0] * 60 + nyidag[1] ) * 60 + nyidag[2] ) *
                         6 + nyidag[3];
                ZS_m = ( ( nyibar[0] * 60 + nyibar[1] ) * 60 + nyibar[2] ) *
                         6 + nyibar[3];
                l2bcd ( bcdx0, ZS_m );
                mulbcdl ( bcdx0, bcdx0, 21600 );
                divbcdl ( bcdx1, bcdx0, 583200 );
                ZS_m = bcd2l ( bcdx1 );
                ZS_d = ZS_m / 60;
                ZS_m = ZS_m % 60;
                ZS_s = ZS_d / 30;
                ZS_d = ZS_d % 30;
                if ( lhag )  // If this is the first of two weekdays.
                {
                // Put here any special calculations for extra day 
                // At this point, the gzadag, nyidag & monlong are correct for
                // the second of the two days with same tithi. We need for the
                // first. Previous tithi ended just before start of day we need.
                clear_a_b ();
                for ( i = 0; i < 5; ++i )
                  prv_monlong[i] = 0;
                prv_monlong[1] = 54;

                mul_gen ( prv_monlong, prv_monlong, tt, 27, sun_f );
                add_gen ( prv_monlong, prv_monlong, nyidag, 27, sun_f );
                listb[0] = 1;
                sub_gen ( prv_monlong, prv_monlong, listb, 27, sun_f );

                prv_lunmanx = prv_monlong[0];  

// Now calculate yoga, sbyor ba:

                  add_gen ( lista, prv_monlong, nyidag, 27, sun_f );
                  prv_sbyorba = lista[0];
                  for ( i = 0; i < 6; ++i )
                    prv_sbyorlong[i] = lista[i];

// Now calculate karana, sbyed pa:  

                  clear_a_b ();
                  sub_gen ( lista, prv_monlong, prv_nyidag, 27, sun_f );
                  prv_byedpa = lista[0] * 60 + lista[1];
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

                juldat = juldat - 1;
                jul2date ( juldat ); // This is normally done by spi_zagf

// Solar day data:

                sd_animx = ( juldat - 2 ) % 12;
                sd_elemx = ( ( juldat - 3 ) / 2 ) % 5;
                sd_clunmanx = ( juldat - 17 ) % 28;
                sd_smex = ( juldat - 2 ) % 9 + 1; 

                // LINE 1. - First of duplicated day
                sprintf ( outbuf, "%d: %s. %s. %s-%s; %d %s %d",
                          tt,
                          dayoweek[ doweek ],
                          lunmanP[ prv_lunmanx ],
                          fourelem[ dowkelem[ doweek ] ],
                          fourelem[ lunmanelem[ prv_lunmanx ] ],
                          wd, wmonths[ (wm-1) ], wy);
                fprintf ( fptgt, "%s\n", outbuf );

                // LINE 2. First of duplicated day
                sprintf ( outbuf, "%s, %s, %s, %s %d",
                          yogaP[ prv_sbyorba ],
                          byedP[ prv_byedpax ],
                          cycanim[ ld_animx ],
                          cycpar[ ld_parx ], ld_smex );
                fprintf ( fptgt, "\0x20\0x20%s\n", outbuf );

                // LINE 3. First of duplicated day
                if ( tsurlug == 1 )
                {
                sprintf ( outbuf,
                    "%d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d",
                       ( prv_gzadag[0] + 1 ) % 7, 60, 0,
                       prv_monlong[0], prv_monlong[1], prv_monlong[2],
                       nyidag[0], nyidag[1], nyidag[2],
                       prv_sbyorlong[0], prv_sbyorlong[1], prv_sbyorlong[2] );
                }
                else
                {
                sprintf ( outbuf,
                "%d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d",
                       ( prv_gzadag[0] + 1 ) % 7, 60, 0,
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
                              cycelem1[ sd_elemx ],
                              cycanim[ sd_animx ],
                              Clunman[ sd_clunmanx ] );
                  }
                else
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s %d" ,
                              cycelem1[ sd_elemx ],
                              cycanim[ sd_animx ],
                              Clunman[ sd_clunmanx ],
                              sd_smex );
                  }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

          // Now do second of duplicated lunar day

            juldat = juldat + 1;  // This is readjustment!
            jul2date ( juldat );

// Solar day data:   

            sd_animx = ( juldat - 2 ) % 12;
            sd_elemx = ( ( juldat - 3 ) / 2 ) % 5;
            sd_clunmanx = ( juldat - 17 ) % 28;
            sd_smex = ( juldat - 2 ) % 9 + 1;  

                // LINE 1. second of duplicated 
                sprintf ( outbuf, "%d: %s. %s. %s-%s; %d %s %d",
                          tt,
                          dayoweek[ doweek ],
                          lunmanP[ lunmanx ],
                          fourelem[ dowkelem[ doweek ] ],
                          fourelem[ lunmanelem[ lunmanx ] ],
                          wd, wmonths[ (wm-1) ], wy);
                fprintf ( fptgt, "%s\n", outbuf );

                // LINE 2.  Second of duplicated day
                sprintf ( outbuf, "%s, %s, %s, %s %d",
                          yogaP[ sbyorba ],
                          byedP[ byedpax ],
                          cycanim[ ld_animx ],
                          cycpar[ ld_parx ], ld_smex );
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

                // LINE 3. Second of duplicated
                if ( tsurlug == 1 )
                {
                sprintf ( outbuf,
                      "%d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d",
                          gzadag[0], gzadag[1], gzadag[2],
                          monlong[0], monlong[1], monlong[2],
                          nyidag[0], nyidag[1], nyidag[2],
                          sbyorlong[0], sbyorlong[1], sbyorlong[2] );
                }
                else
                {
                sprintf ( outbuf,
                "%d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d",
                          gzadag[0], gzadag[1], gzadag[2],
                          monlong[0], monlong[1], monlong[2],
                          nyidag[0], nyidag[1], nyidag[2],
                          sbyorlong[0], sbyorlong[1], sbyorlong[2],
                          ZS_s, ZS_d, ZS_m );
                }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

                // LINE 4. Second of duplicated
                if ( tsurlug == 1 )
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s" ,
                              cycelem1[ sd_elemx ],
                              cycanim[ sd_animx ],
                              Clunman[ sd_clunmanx ] );
                  }
                else
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s %d" ,
                              cycelem1[ sd_elemx ],
                              cycanim[ sd_animx ],
                              Clunman[ sd_clunmanx ],
                              sd_smex );
                  }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );
                } // END OF if ( lhag )
              else  // No lhag - normal day 
                {
                // LINE 1.
                sprintf ( outbuf, "%d: %s. %s. %s-%s; %d %s %d",
                          tt,
                          dayoweek[ doweek ],
                          lunmanP[ lunmanx ],
                          fourelem[ dowkelem[ doweek ] ],
                          fourelem[ lunmanelem[ lunmanx ] ],
                          wd, wmonths[ (wm-1L) ], wy);
                fprintf ( fptgt, "%s\n", outbuf );

                // LINE 2. NORMAL DAY 
                sprintf ( outbuf, "%s, %s, %s, %s %d",
                          yogaP[ sbyorba ],
                          byedP[ byedpax ],
                          cycanim[ ld_animx ],
                          cycpar[ ld_parx ], ld_smex );
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

                // LINE 3. NORMAL DAY 
                if ( tsurlug == 1 )
                {
                sprintf ( outbuf,
                  "%d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d",
                          gzadag[0], gzadag[1], gzadag[2],
                          monlong[0], monlong[1], monlong[2],
                          nyidag[0], nyidag[1], nyidag[2],
                          sbyorlong[0], sbyorlong[1], sbyorlong[2] );
                }
                else
                {
                sprintf ( outbuf,
                "%d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d %d;%d,%d",
                          gzadag[0], gzadag[1], gzadag[2],
                          monlong[0], monlong[1], monlong[2],
                          nyidag[0], nyidag[1], nyidag[2],
                          sbyorlong[0], sbyorlong[1], sbyorlong[2],
                          ZS_s, ZS_d, ZS_m );
                }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );

                // LINE 4. Normal day 
                if ( tsurlug == 1 )
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s" ,
                              cycelem1[ sd_elemx ],
                              cycanim[ sd_animx ],
                              Clunman[ sd_clunmanx ] );
                  }
                else
                  {
                    sprintf ( outbuf, "Solar: %s-%s. %s %d" ,
                              cycelem1[ sd_elemx ],
                              cycanim[ sd_animx ],
                              Clunman[ sd_clunmanx ],
                              sd_smex );
                  }
                fprintf ( fptgt, "\x20\x20%s\n", outbuf );
                } // End of normal day, no "lhag"

                // LINE 5, Normal day or duplicated  
                if ( tsurlug == 1 )
                  {
                  // (1) - 13 & (8) - 27 has both
                    if ( chk_sadag ( mthnum, tt ) )
                      fprintf ( fptgt, "\x20\x20\x20\x20%s\n", sadag_str );
                  }
                
                if ( !delaymth && chk_spec ( mthnum, tt ) )
                  {
                    fprintf ( fptgt, "  %s\n", outbuf );
                  }
              } // End of normal or duplicated; ie. not omitted.
            ++ltt;  
            newmth = 0;
            lastjd = juldat;
          } while ( ltt <= 30 ); // End of day loop
// End of day loop 
        newmth = 1;
        if ( !zeromthfg )
          ++tm;
        if ( tm > 12 )
          {
            ++ty;
            tm = 1;
          }
        if ( adj_mth == 12 )
          {
            // First check for following intercalary:
            if ( epch == 2 && ( zladag[1] == 63 || zladag[1] == 64 ) )
              more = 1;
            else
              more = 0; // We have finished.
          }
      } while ( more );        
// End of month loop
    fclose ( fptgt );
    if ( cur_year < last_year )
      {
        ++cur_year;
        goto donextyear;
      }
//  printf ( "Maximum sa_dag string length: %d\n", sadag_str_len );  
  } // END - prn_cal ()

// Check for main Earth-lords, "sa bdag"  
int chk_sadag ( int m, int t )
  {
    int i = 0;
    int n;
    
// First, "yan kwong": // Data from Kongleg.

    yan_kwong_flg = 0;

    if ( m == 1 && t == 13 ) yan_kwong_flg = 1;
    else if ( m == 2 && t == 11 ) yan_kwong_flg = 1;
    else if ( m == 3 && t == 9 ) yan_kwong_flg = 1;
    else if ( m == 4 && t == 7 ) yan_kwong_flg = 1;
    else if ( m == 5 && t == 5 ) yan_kwong_flg = 1;
    else if ( m == 6 && t == 3 ) yan_kwong_flg = 1;
    else if ( m == 7 && t == 1 ) yan_kwong_flg = 1;
    else if ( m == 7 && t == 29 ) yan_kwong_flg = 1;
    else if ( m == 8 && t == 27 ) yan_kwong_flg = 1;
    else if ( m == 9 && t == 25 ) yan_kwong_flg = 1;
    else if ( m == 10 && t == 23 ) yan_kwong_flg = 1;
    else if ( m == 11 && t == 21 ) yan_kwong_flg = 1;
    else if ( m == 12 && t == 19 ) yan_kwong_flg = 1;

// Now, "zin phung":

    zin_phung_flg = 0;

// These data are taken from Mongolian data in VKP2.DOC

    if ( m == 1 || m == 2 || m == 3 ) // dpyid gsum, etc.
      {
        if ( t == 1 || t == 7 || t == 13 || t == 19 || t == 25 )
          zin_phung_flg = 1;
      }
    else if ( m == 4 || m == 5 || m == 6 )
      {
        if ( t == 6 || t == 12 || t == 18 || t == 24 || t == 30 )
          zin_phung_flg = 1;
      }
    else if ( m == 7 || m == 8 || m == 9 )
      {
        if ( t == 3 || t == 9 || t == 15 || t == 21 || t == 27 )
          zin_phung_flg = 1;
      }
    else if ( m == 10 || m == 11 || m == 12 )
      {
        if ( t == 4 || t == 10 || t == 16 || t == 22 || t == 28 )
          zin_phung_flg = 1;
      } 

//  zin_phung_flg = 0; // Take it out for now. Need to check data.

// Now, "klu bzlog":

    klu_bzlog_flg = 0;

    if ( m == 1 )
      {
        if ( t == 5 || t == 10 || t == 15 )
          klu_bzlog_flg = 1;
      }
    else if ( m == 2 )
      {
        if ( t == 8 || t == 18 || t == 20 || t == 22 || t == 28 )
          klu_bzlog_flg = 1;
      }
    else if ( m == 4 )
      {
        if ( t == 20 || t == 25 )
          klu_bzlog_flg = 1;
      }
    else if ( m == 5 )
      {
        if ( t == 8 || t == 15 )
          klu_bzlog_flg = 1;
      } 
    else if ( m == 6 )
      {
        if ( t == 11 || t == 13 || t == 15 || t == 23 )
          klu_bzlog_flg = 1;
      } 
    else if ( m == 7 )
      {
        if ( t == 5 || t == 6 )
          klu_bzlog_flg = 1;
      }       
    else if ( m == 8 )
      {
        if ( t == 3 || t == 6 || t == 9 || t == 13 || t == 16 )
          klu_bzlog_flg = 1;
      }       
    else if ( m == 9 )
      {
        if ( t == 9 || t == 10 || t == 19 )
          klu_bzlog_flg = 1;
      }       
    else if ( m == 10 )
      {
        if ( t == 9 || t == 10 || t == 19 || t == 26 )
          klu_bzlog_flg = 1;
      }       
    else if ( m == 11 )
      {
        if ( t == 2 || t == 6 || t == 16 || t == 20 || t == 26 )
          klu_bzlog_flg = 1;
      }       

// Now, "klu thebs":

    klu_thebs_flg = 0;

    if ( m == 1 )
      {
        if ( t == 14 )
          klu_thebs_flg = 1;
      }
    else if ( m == 2 )
      {
        if ( t == 10 )
          klu_thebs_flg = 1;
      }
    else if ( m == 3 )
      {
        if ( t == 25 )
          klu_thebs_flg = 1;
      }      
    else if ( m == 4 )
      {
        if ( t == 8 || t == 15 )
          klu_thebs_flg = 1;
      }
    else if ( m == 5 )
      {
        if ( t == 20 || t == 22 )
          klu_thebs_flg = 1;
      } 
    else if ( m == 6 )
      {
        if ( t == 5 || t == 20 || t == 25 )
          klu_thebs_flg = 1;
      } 
    else if ( m == 7 )
      {
        if ( t == 9 || t == 19 )
          klu_thebs_flg = 1;
      }       
    else if ( m == 8 )
      {
        if ( t == 5 || t == 15 )
          klu_thebs_flg = 1;
      }       
    else if ( m == 9 )
      {
        if ( t == 1 || t == 11 || t == 21 || t == 22 || t == 23 )
          klu_thebs_flg = 1;
      }       
    else if ( m == 10 )
      {
        if ( t == 8 || t == 15 || t == 18 )
          klu_thebs_flg = 1;
      }       
    else if ( m == 11 )
      {
        if ( t == 7 || t == 15 || t == 21 )
          klu_thebs_flg = 1;
      }       

// Now, "nyi nag": // Data from Vaidurya dkar po.

    nyi_nag_flg = 0; 

    if ( m == 1 && t == 7 ) nyi_nag_flg = 1;
    else if ( m == 2 && t == 14 ) nyi_nag_flg = 1;
    else if ( m == 3 && t == 21 ) nyi_nag_flg = 1;
    else if ( m == 4 && t == 8 ) nyi_nag_flg = 1;
    else if ( m == 5 && t == 16 ) nyi_nag_flg = 1;
    else if ( m == 6 && t == 24 ) nyi_nag_flg = 1;
    else if ( m == 7 && t == 9 ) nyi_nag_flg = 1;
    else if ( m == 8 && t == 18 ) nyi_nag_flg = 1;
    else if ( m == 9 && t == 27 ) nyi_nag_flg = 1;
    else if ( m == 10 && t == 10 ) nyi_nag_flg = 1;
    else if ( m == 11 && t == 20 ) nyi_nag_flg = 1;
    else if ( m == 12 && t == 30 ) nyi_nag_flg = 1;

// Now, build string      

    n = yan_kwong_flg + klu_bzlog_flg + klu_thebs_flg + zin_phung_flg + nyi_nag_flg;
    if ( n == 0 )
      return (0);
    sadag_str[0] = '\0';      
    if ( yan_kwong_flg )
      {
        strcat ( sadag_str, "yan kwong" );
        i++;   
      }
    if ( klu_bzlog_flg )
      {
        if ( i )
          strcat ( sadag_str, ", " );
        strcat ( sadag_str, "klu bzlog" );
        i++;   
      }      
    if ( klu_thebs_flg )
      {
        if ( i )
          strcat ( sadag_str, ", " );
        strcat ( sadag_str, "klu thebs" );
        i++;   
      }            
    if ( zin_phung_flg )
      {
        if ( i )
          strcat ( sadag_str, ", " );
        strcat ( sadag_str, "zin phung" );
        i++;   
      }      
    if ( nyi_nag_flg )
      {
        if ( i )
          strcat ( sadag_str, ", " );
        if ( m == 1 )  
          strcat ( sadag_str, "nyi nag ngan pa dgu 'dzom" );        
        else
          strcat ( sadag_str, "nyi nag" );
        i++;   
      }            
    i = strlen ( sadag_str);
    if ( i > sadag_str_len ) // Max 35 in testing.
      sadag_str_len = i;
    return (1);      
  } // END - chk_sadag ()  
  
void s_cyc ( void )
  {
    int i, more, n, newmth, lhag, chad, nextchad, nextlhag;
    int intercal, delaymth;
    int lhag_chad[6], lhag_chad_cnt;
    int calc_mth_adj, wwd, wwm, wwy, dow;
    int scndzero = 0;
    int rab_byung, rab_num;

    int yr_gender;    // 1 = female, odd; 0 = male, even
    int mthnum; // Adjusted, without negative.
    int cur_year, last_year;
    int ltt, zla0, zla1;
    int lastjd = 0;
    int curjd, nextjd; // Last and next JD calculations.
    int yr_animx, yr_elemx;
    int new_year;

    set_lang ();
    gza_short_flg = 0; // Flag for less accurate weekday adjustment
    printf ( "\n\n\n%s\n", e_str[epch] );
    printf ("Enter the starting year: ");
    scanf ("%d", &ty);
    printf ("\n");

    cur_year = ty;

    printf ("Enter the finish year: ");
    scanf ("%d", &last_year );
    printf ("\n");

    sprintf ( filenam, "yr_tab.txt" );

    fptgt = fopen ( filenam, "w" );
    if ( fptgt == NULL )
      {
        printf ( "Error opening target file!\n" );
        getch ();
      }

donextyear:

    new_year = 1;
    ty = cur_year;
    tm = 1;
    zeromthfg = 0;

// tt not yet set
// First, check to see if we are not at adjusted 12th month:

    zla_dag (ty, tm); // But only if moving one month at a time!

    zla0 = zladag[0];
    zla1 = zladag[1];

    adj_zla ();
    if ( adj_mth == 12 )
      {
        if ( !zeromthfg ) 
          ++tm;
      }
    else if ( adj_mth == -1 )
      {
        zladag[0] = zla0;
        zladag[1] = zla1;
        zeromthfg = 0;
      }
    n = 0;
    more = 1;
    newmth = 1;
    do   // START OF MONTH LOOP *******************************
      {
        for ( i = 0; i < 6; ++i )
          lhag_chad[i] = 0; 
        lhag_chad_cnt = 0;
        intercal = 0;
        if ( adj_mth == -1 )
          intercal = 1;
      
        delaymth = 0;
        if ( newmth )
          {
            if ( !zeromthfg )   // We need to use the same data, twice.
              zla_dag (ty, tm); // But only if moving one month at a time!
            adj_zla ();
            cur_mth = zladag[0];  // Needed for general day
            gza_dru (cur_mth);
            nyi_dru (cur_mth);
            rilchaf ( cur_mth ); // MOVED, from gza_dag, NEW, 11/9/94
            if ( adj_mth < 0 )
              {
                ++adj_mth;
                if ( adj_mth == 0 )
                  adj_mth = -12;
              }

// DO WE HAVE NEW YEAR?
            if ( ( adj_mth == -1 || adj_mth == 1 ) && new_year )
              {
                if ( ty < 7 )
                  {
                    yr_animx = ( ty + 1200 - 7 ) % 12;
                    yr_elemx = ( ( ty + 1200 ) / 2 - 3 ) % 5;
                    yr_gender = ( ty + 1200 ) % 2; // 1 = female, odd; 0
                  }
                else
                  {
                    yr_animx = ( ty - 7 ) % 12;
                    yr_elemx = ( ty / 2 - 3 ) % 5;
                    yr_gender = ty % 2; // 1 = female, odd; 0 = male
                  }

                rab_num = cur_year - 1027;
                rab_byung = 1 + rab_num/60;
                rab_num = 1 + rab_num % 60;

                sprintf ( outbuf, "\n%d.RAB-BYUNG/ %d.YEAR %s-%s-%s\n", 
                                  rab_byung, rab_num, 
                                  cycelem1[ yr_elemx ],
                                  cycgendE[ yr_gender ],
                                  cycanim[ yr_animx ] );
                fprintf ( fptgt, "%s\n", outbuf );
                new_year = 0;
              }
          }
        if ( zlapure == 0 ) // Phugpa grub rtsis
          {
            if ( adj_mth < 0 ) // Intercalary
              {
                intercal = 1;
                delaymth = 0;
              } 
            else if ( zladag[1] == zlasho1+2 || zladag[1] == zlasho1+2 )
              {
                intercal = 0;
                delaymth = 1;
              }
          }
        else
          {
            if ( adj_mth < 0 ) // Intercalary
              {
                intercal = 1;
                delaymth = 0;
              }
            else if ( adj_mth > 0 && ( zladag[1] == 0 || zladag[1] == 1 ) )
              {
                intercal = 0;
                delaymth = 1;
              }
          }

        if ( adj_mth < 0 )
          mthnum = -adj_mth;
        else
          mthnum = adj_mth;

        printf ("The ZLA-DAG used is: %d;%d\n", zladag[0], zladag[1]);

// Put calc_mth here: 

        if ( zladag[1] == 0 || zladag[1] == 1 )
          {
            if ( !scndzero ) // First of two months
              {
                scndzero = 1;
                calc_mth_adj = tm;
              }
            else // Second
              {
                scndzero = 0;
                calc_mth_adj = -tm; 
              }
          }
        else
          {
            calc_mth_adj = tm;
            if ( calc_mth_adj == 0 )
              calc_mth_adj = 12;
          }

        tt = 1;
        ltt = 1;
// START OF DAY LOOP - START OF DAY LOOP - START OF DAY LOOP **********************
        do
          {  
            tt = ltt;
            nextlhag = 0;
            lhag = 0;
            chad = 0;
            nextchad = 0;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();

// Work out previous and next days to determine CHAD or LHAG
            curjd = juldat;
            tt = ltt - 1;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            lastjd = juldat;

            tt = ltt + 1;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            nextjd = juldat;

// Recalculate for current tithi: 

            tt = ltt;
            tse_dru (tt);
            nyi_lon (tt);
            add_gen (tsebar, gzadru, tsedru, 7, gza_f);
            add_gen (nyibar, nyidru, nyilon, 27, sun_f );
            nyi_dag (nyibar); // Must be done before gza_dag
            gza_dag (tsebar);
            spi_zagf ();
            if ( curjd == lastjd + 2 )
              lhag = 1;
            if ( nextjd == curjd + 2 )
              nextlhag = 1;
            if ( curjd == lastjd )
              chad = 1;
            if ( curjd == nextjd )
              nextchad = 1;

            if ( tt == 1 ) // What about omitted 1 ?
              {
                wwd = wd; // These are longs
                wwm = wm;
                wwy = wy;
                dow = gzadag[0] + 3;
                if ( dow > 6 )
                  dow = dow - 7;
              }
           
               if ( chad )   // This is for omitted lunar day
              {
                lhag_chad[lhag_chad_cnt] = -tt;
                ++lhag_chad_cnt;
              }
            else  // If normal or duplicated:
              {
                if ( lhag )  // If this is the first of two weekdays.
                  {
                    lhag_chad[lhag_chad_cnt] = tt;
                    ++lhag_chad_cnt;
                  } 
              }
            ++ltt;  
            newmth = 0;
            lastjd = juldat;
          } while ( ltt <= 30 ); // End of day loop
        newmth = 1;
        if ( !zeromthfg )
          ++tm;
        if ( tm > 12 )
          {
            ++ty;
            tm = 1;
          }
        if ( adj_mth == 12 )
          more = 0; // We have finished.
        
        sprintf ( outbuf, "%3ld %3d/%2d.%2d.%d %3d %3d %3d %3d  %d/\n", adj_mth, calc_mth_adj, 
                  wwd, wwm, wwy,
                  lhag_chad[0], lhag_chad[1], lhag_chad[2], lhag_chad[3], dow );
        fprintf ( fptgt, "%s", outbuf );

      } while ( more );        
// End of month loop
    if ( cur_year < last_year )
      {
        ++cur_year;
        goto donextyear;
      }
    fclose ( fptgt );
  } // END - s_cyc ()
  