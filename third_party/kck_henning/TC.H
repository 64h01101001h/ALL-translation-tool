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

// T1.C:

void cal_cyc ( void );
void year_cyc ( void );
void new_yer ( void );

// T2.C:

void set_byed ( void );
void set_epoch ( void );
void do_rahu ( long int, long int );
void zla_dag ( long int, long int );
void adj_zla ( void );
void rilchaf ( long int );
void spi_zagf ( void );
void jul_chk ( void ); // MOVE - NOT NEEDED HERE
long int gregd2jul ( long int, long int, long int ); // MOVE ??
void jul2date ( long int ); // MOVE ??
void nyi_dru ( long int );
void gza_dru ( long int );
void nyi_lon ( long int );
void tse_dru ( long int );
void nyi_dag ( long int * );
void gza_dag ( long int * );
void do_lopho ( long int ); // UNFINISHED

// T3.C - Planet routines.

void calc_dalbar ( long int *, long int, long int, long int );
void do_daldag ( long int *, long int *, long int *, long int *,
                 long int *, long int );
void do_plans ( long int );
void marsdag ( void );
void jupdag ( void );
void satdag ( void );
void merdag ( void ); // Mercury myur dag
void vendag ( void ); // Venus myur dag
void plan_cyc ( void );

// T4.C: Routines for printing actual calendars:

void prn_cal ( void );
void prn_bir_cal ( void );
void prn_bir2_cal (void);
void prn_mtsk_cal (void);
void prn_cal2 ( void );
int chk_spec ( long int, long int );

// UTILS.C:

void prn_lst6 ( long int * );
void prn_lst7 ( long int * );
void prn_lstd ( long int *, long int, long int );
long int exp_lst ( long int *, long int ); 
void mul_g6 ( long int *, long int, long int, long int );
void mul_g7 ( long int *, long int, long int, long int, long int );
void div_g6 ( long int *, long int, long int, long int );
void sub_g6 ( long int *, long int *, long int *, long int, long int, long int
                       );
void add_g6 ( long int *, long int *, long int *, long int, long int, long int
                       );
void add_g7 ( long int *, long int *, long int *, long int, long int, long int,
              long int );
void mul_gen ( long int *, long int *, long int, long int, long int );
double conv2degs ( long int *, long int, long int );
double conv2degs1 ( long int *, long int, long int );
void add_gen ( long int *, long int *, long int *, long int, long int );
void sub_gen ( long int *, long int *, long int *, long int, long int );
void prn_lst ( long int * );
void clrlst ( long int * );
void clrlst6 ( long int * );
void clear_a_b ( void );

// Originally in EHROUT.C/.H:

#define ESCAPE	-2
#define FUN3	-5
#define SHFTAB	-3
#define NORTAB	-4
#define RETURN	 5

void cls ( void );
void pos ( int, int );
int prfldat ( int, int, int, unsigned char * );
void printat ( int, int, char * );
void conv2rc ( int *, int *, int );
unsigned getkey ( void );
void testnewkey ( void );

// OLD:

// void chk_mth ( long );
// void chk_gang ( long );

union g_char
  {
    char      c[2];
    int       cnum;
  };
