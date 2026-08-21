/* T2.C */

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
#include "tc.ext"
#include "bcd.h"
#include "bcd.ext"

extern long int nyifac[5], merfac[5], venfac[5], marfac[5], jupfac[5], satfac[5];
extern long int nyifacT[5], merfacT[5], venfacT[5], marfacT[5], jupfacT[5], satfacT[5];
extern long int nyifac1[5], merfac1[5], venfac1[5], marfac1[5], jupfac1[5], satfac1[5];

extern int print_on, printinprog;
extern char printstring[120];
void start_print ( void );
void prin_str ( char * );
void addstrings ( char *, char * );

// Local to this file only:

long int sol_cor[6];

long int  spz_c, spz_b, spz_f, spz_j;

long int gzada[6];
long int nyida[6];
long int nyidm[6], gzadm[6], tsedm[6], nyilm[6];

static long int nyidmb[6] = { 2, 10, 58, 2, 10, 0 };
static long int gzadmb[6] = { 1, 31, 50, 0, 0, 0 };
static long int tsedmb[6] = { 0, 59, 3, 4, 0, 0 };
static long int nyilmb[6] = { 0, 4, 21, 5, 9, 0 };

static long int nyihaf[6] = { 13, 30, 0, 0, 0, 0 };

static long int nyibye[6] = { 4, 1, 1, 4, 6, 6};
static long int nyidom[6] = { 6, 10, 11, 10, 6, 0};

static long int gzabye[14] = { 5, 5, 4, 3, 2, 1, -1, -2, -3,
                                     -4, -5, -5, -5, 5};
static long int gzadom[14] = { 5, 10, 15, 19, 22, 24, 25, 24, 22,
                                     19, 15, 10, 5, 0};

/**** Figures for original Tantra epoch, byed rtsis (1) ****/

static long int gda1[6] = { 2, 30, 0, 0, 0, 0 };
static long int nda1[6] = { 26, 58, 0, 0, 0, 0 };

/**** Figures for Shakya Shribhadra (2) ****/

static long int gda2[6] = { 0, 15, 30, 0, 0, 0 };   // 3;34,36,5.65333 - DONE
static long int nda2[6] = { 25, 49, 1, 3, 3, 0 };   // 25;5,29,3,2 (13) - DONE

/**** Figures for modern Karana (3) ****/

static long int gda3[6] = { 3, 34, 37, 0, 0, 0 };   // 3;34,36,5.65333 - DONE
static long int nda3[6] = { 25, 5, 29, 3, 2, 0 };   // 25;5,29,3,2 (13) - DONE

/*** ROUTINE TO SET FOR BYED RTSIS ****/

void set_byed ( void )
  {
    int  i;
    for ( i = 0; i < 6; ++i )
      nyidm[i] = nyidmb[i];
    for ( i = 0; i < 6; ++i )
      gzadm[i] = gzadmb[i];
    for ( i = 0; i < 6; ++i )
      tsedm[i] = tsedmb[i];
    for ( i = 0; i < 6; ++i )
      nyilm[i] = nyilmb[i];
    sun_f = 13L;
    gza_f = 13L;   // This is correct for "byed rtsis", compatible with Sun 
		   // for weekday interpolations
  } // END - set_byed ()

/*** ROUTINE TO SET EPOCH PARAMETERS ****/

void set_epoch ( void )
  {
    unsigned int chr;
//  union  g_char chrt;
    int  i;
//  Changed for Win32 console app:    
//  cls ();
//  pos ( 0, 0 );
    printf ( "\n\nSelect from the following:\n\n" );
    for ( i = 0; i < 4; ++i )
      printf ( "%s\n", e_str[i] );
    do
      chr = getch ();
    while ( chr < '1' || chr > '3' );

    if ( chr == '1' ) // Tantra, byed rtsis
      {
        set_byed ();
        for ( i = 0; i < 6; ++i )
          gzada[i] = gda1[i];
        for ( i = 0; i < 6; ++i )
          nyida[i] = nda1[i];
        epch_yr = 806L;
        epch_mth = 2L;
        ical_ind = 0L;  // Intercalation index
        ril_a = 5L;     // Anomaly
        ril_b = 112L;   // Anomaly fractional part
// These two not actually given in the tantra:
        spz_b = 32L;   // general day (64) 
        spz_c = 0L;    // general day (707)
        spz_f = 2L;    // This is subtracted to check day.
        spz_j = 2015531L; // Julian at epoch.
        rahupart = 122L; // 230 fraction for Rahu cycle
// This one calculated, not given in Tantra: 
        dragkadd = 758L; // This is added for drag po'i rkang 'dzin.
        meradd = 1674L;  
        venadd = 2163L; 
        maradd = 167L;
        jupadd = 1732L;
        satadd = 5946L;

        for ( i = 0; i < 5; ++i )
          {
            nyifac[i] = nyifacT[i];
            merfac[i] = merfacT[i];
            venfac[i] = venfacT[i];
            marfac[i] = marfacT[i];
            jupfac[i] = jupfacT[i];
            satfac[i] = satfacT[i];
          }

        zlapure = 1;
        tsurlug = 0;
      }
    if ( chr == '2' ) // Shakya Shribhadra, 1206
      {
        set_byed ();
        for ( i = 0; i < 6; ++i )
          gzada[i] = gda2[i];
        for ( i = 0; i < 6; ++i )
          nyida[i] = nda2[i];
        epch_yr = 1206L;
        epch_mth = 2L;
        ical_ind = 47L;  // Intercalation index / 47 earlier was a mistake - now 49
        ril_a = 27L;     // Anomaly
        ril_b = 19L;   // Anomaly fractional part
// These two not actually given in the tantra:
        spz_b = 47L;   // general day (64) 
        spz_c = 330L;    // general day (707)
        spz_f = 0L;    // This is subtracted to check day.
        spz_j = 2161619L; // Julian at epoch.
        rahupart = 9L; // 230 fraction for Rahu cycle
// This one calculated, not given in Tantra: 
        dragkadd = 758L; // This is added for drag po'i rkang 'dzin.
        meradd = 7454L;  
        venadd = 771L; 
        maradd = 611L;
        jupadd = 532L;
        satadd = 1310L;

        for ( i = 0; i < 5; ++i )
          {
            nyifac[i] = nyifacT[i];
            merfac[i] = merfacT[i];
            venfac[i] = venfacT[i];
            marfac[i] = marfacT[i];
            jupfac[i] = jupfacT[i];
            satfac[i] = satfacT[i];
          }
        zlapure = 1;
        tsurlug = 0;
      }      
      
    else if ( chr == '3' ) // Modern Karana, byed rtsis
      { 
        set_byed (); // More general program also handles "grub rtsis"
        for ( i = 0; i < 6; ++i )
          gzada[i] = gda3[i];
        for ( i = 0; i < 6; ++i )
          nyida[i] = nda3[i];
        epch_yr = 2009L;  
        epch_mth = 2L;
        ical_ind = 55L;  // Intercalation index
        ril_a = 4L;   // Anomaly
        ril_b = 73L;  // Anomaly fractional part 
        spz_c = 27L;  // general day fractional part, 64 radix 
        spz_b = 54L;  // general day fractional part, 707 radix
        spz_f = 3L;   // Weekday at epoch, for general day check 
        spz_j = 2454887L; 
        rahupart = 33L; // 230 fraction for Rahu cycle - DONE

// Figures to be added for planets for the nyin zhag dal ba. These are days of cycle.

        meradd = 6089L; 
        venadd = 852L;  
        maradd = 586L;  
        jupadd = 3757L; 
        satadd = 4846L; 

        for ( i = 0; i < 5; ++i )
          {
            nyifac[i] = nyifac1[i];
            merfac[i] = merfac1[i];
            venfac[i] = venfac1[i];
            marfac[i] = marfac1[i];
            jupfac[i] = jupfac1[i];
            satfac[i] = satfac1[i];
          }

        zlapure = 1;    
        tsurlug = 1;    // DONE - Which is most appropriate??
        dragkadd = 6247582L; // For "drag gsum rkang 'dzin" (6714405 radix) 
      }
  } // END - set_epoch ()

// Routine to calculate positions of Rahu. Calculates for full and new Moon
// days of month passed. Lunar day value is currently ignored

void do_rahu ( long int m, long int tt )
  {
    long int t, mth;

    mth = ( m + rahupart ) % 230L;
    rahufrac = mth;     // For epoch correlations
    t = ( ( m + rahupart ) % 230L ) * 30L + 15L;
    rahutsa[0] = 0L;
    rahutsa[1] = 0L;
    rahutsa[2] = 14L;
    rahutsa[3] = 0L;
    rahutsa[4] = 12L;

    mul_gen ( rahutsa, rahutsa, t, 27L, 23L );
    clear_a_b ();
    lista[0] = 27L;
    sub_gen ( rahudong, lista, rahutsa, 27L, 23L );
    listb[0] = 13L;
    listb[1] = 30L;
    add_gen ( rahujug, rahudong, listb, 27L, 23L );

    rahutsa[0] = 0L;
    rahutsa[1] = 0L;
    rahutsa[2] = 14L;
    rahutsa[3] = 0L;
    rahutsa[4] = 12L;
    mul_gen ( rahutsa, rahutsa, t + 15L, 27L, 23L ); // For new Moon.
    clear_a_b ();
    lista[0] = 27L;
    sub_gen ( rahudong30, lista, rahutsa, 27L, 23L );
    listb[0] = 13L;
    listb[1] = 30L;
    add_gen ( rahujug, rahudong, listb, 27L, 23L );

    sprintf ( printstring,
    "Rahu month: %ld, gdong, 15th: %ld;%ld,%ld,%ld,%ld - \
30th: %ld;%ld,%ld,%ld,%ld", mth, rahudong[0], rahudong[1],
rahudong[2], rahudong[3], rahudong[4], rahudong30[0], rahudong30[1],
rahudong30[2], rahudong30[3], rahudong30[4] );
    printf ( "%s\n", printstring );
    if ( printinprog )
      prin_str ( printstring );
  } // END - do_rahu ()

// Function to calculate true month, "zla ba rnam par dag pa" 
// Converts solar month count into lunar months, plus intercalation index
void zla_dag ( long int y, long int m )
  {
    long int yr, a, b; // c;
    yr = y - epch_yr;
    a = 12L * yr + m - epch_mth; // Changed from 3L, for true calendar
    if ( a >= 0L )
      {
        b = 2L * a + ical_ind;
        zladag[1] = b % 65L;
        zladag[0] = a + b / 65L;
        }
    else // We don't calculate backwards in time from epoch
      {
        zladag[1] = 0L;
        zladag[0] = 0L;
      }
  } // END - zla_dag ()

// Adjust the month, as necessary. Indicate intercalary by negative value
void adj_zla ( void )
  {
    if ( zladag[1] == 0L || zladag[1] == 1L )
      {
        if ( !scndzero ) // First of two months
          {
            zeromthfg = 1;
            scndzero = 1;
            adj_mth = (int) tm - 1;
            if ( adj_mth == 0 )
              adj_mth = 12;
            adj_mth = -adj_mth;
            zladag[0] = zladag[0] - 1L; // It has advanced 2.
          }
        else // Second
          {
            zeromthfg = 0;
            scndzero = 0;
            adj_mth = (int) tm; 
            zladag[0] = zladag[0] + 1L; // Put it back
          }
      }
    else
      {
        zeromthfg = 0;
        adj_mth = (int) tm;
        if ( adj_mth == 0 )
          adj_mth = 12;
      }
  } // - adj_zla ()

// Function to calculate anomaly, "ril cha"
void rilchaf ( long int x )
  {
    long int   a, b;
    b = x + ril_b;
    a = 2L * x + ril_a + b / 126L;
    rilcha[1] = b % 126L;
    rilcha[0] = a % 28L;
  } // END - void rilchaf ( long int x )

// Function to calculate General Day - "spyi zhag"
// Calculates Julian for the integer Julian day count at Noon UT
// on weekday of "gza' dag". Put into juldat.
void spi_zagf ( void )
  {
    long int a, b, c;
    spizag = cur_mth * 30L + tt;
    c = spizag + spz_c;
    spz_frac_c = c % 707L;  // For epoch correlations
    b = spizag + spz_b;
    b = b + c / 707L;
    spz_frac_b = b % 64L;   // For epoch correlations
    spizag = spizag - b / 64L;
    c = ( spizag + spz_f ) % 7L;
    b = gzadag[0];
    if ( c != b )
      {
        if ( c > 4L && b < 2L )
          b += 7L;
        else if ( b > 4L && c < 2L )
          c += 7L;
        spizag = spizag + b - c;
      }
    if ( b - c > 2L )
      printf ( "\nERROR IN GENERAL DAY ROUTINE: %ld\n", b - c );
    juldat = spizag + spz_j;

    jul2date ( juldat );
  } // - spi_zagf ();

// Convert Gregorian date (and Julian) to Julian day
long int gregd2jul ( long int D, long int M, long int Y )
{
  long int JD;
  int calndar;  // Gregorian = 1.

// In change from Julian to Gregorian calendars, in 1582, Oct 4th was followed
// by Oct 15th

  if ( Y > 1582L )
    calndar = 1;
  else if ( Y < 1582L )
    calndar = 0;
  else
    {
      if ( M > 10L )
        calndar = 1;
      else if ( M < 10L )
        calndar = 0;
      else
        {
          if ( D >= 15L )
            calndar = 1;
          else
            calndar = 0;
        }
    }
  if ( calndar )  // Gregorian:
    {
// CHANGED - the following is from ESAA, 1992, p. 604. 
// Should check its range of validity???

  JD = D + ( 1461 * ( Y + 4800L + ( M - 14L ) / 12L ) ) / 4L
         + ( 367L * ( M - 2L - 12L * ( ( M - 14L ) / 12L ) ) ) / 12L
         - ( 3L * ( ( Y + 4900L + ( M - 14 ) / 12L ) / 100L ) ) / 4L - 32075L;
    }
  else  // Julian:
    {
      JD = 367L * Y - ( 7L * ( Y + 5001L + ( M - 9L ) / 7L )) / 4L
           + ( 275L * M ) / 9L + D + 1729777L;
    }
  return ( JD );
} // END - gregd2jul ()

// Convert Julian day to Gregorian or Julian date
void jul2date ( long int jd )
  {
    long int l, n, j, k, i;

// This algorithm is from photcopied notes, from James Neely.
// Also, checked with ESAA, 1992, p. 604
// Calculates date, at noon on which the Julian date starts.
// Julian 0 starts Greenwich mean noon on 1st Jan 4713 BC, Julian proleptic
// calendar.
// In change from Julian to Gregorian calendars, in 1582, Oct 4th was followed
// by Oct 15th

// First, get day of week:

  doweek = jd - 7L * (( jd + 1 ) / 7L ) + 2L;
  if ( doweek == 7L )
    doweek = 0L;
  if ( doweek > 7L )
    {
      printf ( "ERROR IN DAY OF WEEK ROUTINE:\n" );
      getch ();
    }

  if ( jd >= 2299161L )  // Gregorian calendar:
    { // This has been tested between March 1, 1600 and Jan 31, 2100

    l = jd + 68569L;
    n = ( 4L * l ) / 146097L;
    l = l - ( 146097L * n + 3L ) / 4L;

    l2bcd ( bcda, 4000L );

    mulbcdl ( bcda, bcda, l + 1L );
    divbcdl ( bcda, bcda, 1461001L );

    wy = bcd2l ( bcda );

    l = l - ( 1461L * wy ) / 4L + 31L;
    wm = ( 80L * l ) / 2447L;
    wd = l - ( 2447L * wm ) / 80L;
    l = wm / 11L;
    wm = wm + 2L - 12L * l;
    wy = 100L * ( n - 49L ) + wy + l;
    }
  else // Julian calendar
    {
      j = jd + 1402L;
      k = ( j - 1L ) / 1461L;
      l = j - 1461L * k;
      n = ( l - 1L ) / 365L - l / 1461L;
      i = l - 365L * n + 30L;
      j = ( 80L * i ) / 2447L;
      wd = i - ( 2447L * j ) / 80L;
      i = j / 11L;
      wm = j + 2L - 12L * i;
      wy = 4L * k + n + i - 4716L;
    }
  } // END - jul2date ()

// Function to calculate monthly mean Sun, "nyi ma'i dhru ba".
void nyi_dru ( long int x )
  {
    mul_gen ( nyidru, nyidm, x, 27L, sun_f );
    add_gen ( nyidru, nyidru, nyida, 27L, sun_f );
  } // END - nyi_dru ()

// Function to calculate monthly mean weekday, "gza'i dhru ba". 
void gza_dru ( long int x )
  {
    mul_gen ( gzadru, gzadm, x, 7L, gza_f );
    add_gen ( gzadru, gzadru, gzada, 7L, gza_f );
  } // END - gza_dru ()

// Calculate solar longitude from beginning of month 
void nyi_lon ( long int x )
  {
    mul_gen ( nyilon, nyilm, x, 27L, sun_f );
  } // END - nyi_lon ()

// Calculate weekday change from beginning of month, "tshes kyi dhru ba"
void tse_dru ( long int x )
  {
    mul_gen ( tsedru, tsedm, x, 7L, gza_f );
  } // END - tse_dru ()

// Calculate true solar longitude, "nyi dag". 
void nyi_dag ( long int *a1 )
  {
    long int test, tquo, trem;

    clrlst (zerlst);
    sub_gen ( nyiwor, a1, nyifac, 27L, sun_f );
    test = 60L * nyiwor[0] + nyiwor[1];
    if ( test < 810L )
      nyidor = 0;
    else
      {
        nyidor = 1;
        sub_gen ( nyiwor, nyiwor, nyihaf, 27L, sun_f);
        test = 60L * nyiwor[0] + nyiwor[1];
      }
    trem = test % 135; // Interpolation variable
    tquo = test / 135; // Index for table
    if ( tquo == 0 )
      tquo = 6;

    clear_a_b ();

    lista[2] = ( trem * 60L + nyiwor[2] ) * nyibye[(int)tquo - 1];
    lista[3] = nyiwor[3] * nyibye[(int)tquo - 1];
    lista[4] = nyiwor[4] * nyibye[(int)tquo - 1];

    div_g6 ( lista, 135L, sun_f, 1L );

    clrlst (zerlst);
    add_gen ( lista, zerlst, lista, 27L, sun_f );

    listb[1] = nyidom[(int)tquo - 1];
    if ( tquo == 3L || tquo == 4L || tquo == 5L ) // Then, subtract:
      sub_gen ( sol_cor, listb, lista, 27L, sun_f );
    else
      add_gen ( sol_cor, listb, lista, 27L, sun_f );

    if ( nyidor == 0 )
      sub_gen ( nyidag, a1, sol_cor, 27L, sun_f );
    else
      add_gen ( nyidag, a1, sol_cor, 27L, sun_f );
  } // END - nyi_dag ()

// Function to calculate true weekday, "gza' dag"

void gza_dag ( long int *a1 )
  {
    long int tot, trem, chasha, rilpo, coeff;
    long int gzawor[5];

    clrlst (list1);
    clrlst (list2);
    clrlst (zerlst);
    rilpo = rilcha[0] + tt; // Index into table
    trem = rilpo % 14;  // Interpolation variable
    if ( trem == 0 )
      trem = 14;
    list2[1] = gzadom[(int)trem - 1];
    gza_short_flg = 0;
    if ( gza_short_flg ) // Flag for less accurate weekday adjustment	
			 // This is more compatible with the Tantra method		 
      {			 // See also in TC.DEF and TC4.C
        chasha = rilcha[1] * gzabye[(int)trem - 1] * 360L * gza_f; 
       // Maximum value = 125 * 5 * 254520 = 159075000 - OK
        tot = chasha / 126L;
      }
    else // This is more normal in Tibetan systems,
      {  // but not described in the original Tantra
        l2bcd ( bcda, ( 30L * rilcha[1] + tt ) * gzabye[(int)trem - 1] );
        mulbcdl ( bcda, bcda, 360L * gza_f );
        divbcdl ( bcda, bcda, 3780L );
        tot = bcd2l (bcda);
      }
    if ( tot < 0 )
      {
        list1[4] = -tot;
        add_gen ( list1, list1, zerlst, 7L, gza_f );
        sub_gen ( list3, list2, list1, 7L, gza_f );
      }
    else
      {
        list1[4] = tot;
        add_gen ( list1, list1, zerlst, 7L, gza_f );
        add_gen ( list3, list2, list1, 7L, gza_f );
      }

    if (( rilpo / 14 ) % 2 == 0 )
      add_gen ( gzawor, a1, list3, 7L, gza_f);
    else
      sub_gen ( gzawor, a1, list3, 7L, gza_f);

    if ( nyidor == 0 ) // Solar correction now is applied:
      sub_gen ( gzadag, gzawor, sol_cor, 7L, sun_f );
    else
      add_gen ( gzadag, gzawor, sol_cor, 7L, sun_f );
  } // END - gza_dag ()
