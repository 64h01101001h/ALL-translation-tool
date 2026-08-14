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

// T1.C:

void cal_cyc ( void );
void new_yer ( void );

// T2.C:

void set_grub ( void );
void set_epoch ( void );
void do_rahu ( int, int );
void zla_dag ( int, int );
void adj_zla ( void );
void rilchaf ( int );
void spi_zagf ( void );
int gregd2jul ( int, int, int ); // MOVE ??
void jul2date ( int ); // MOVE ??
void nyi_dru ( int );
void gza_dru ( int );
void nyi_lon ( int );
void tse_dru ( int );
void nyi_dag ( int * );
void gza_dag ( int * );

// T3.C - Planet routines.

void calc_dalbar ( int *, int, int, int );
void do_daldag ( int *, int *, int *, int *,
                 int *, int );
void do_plans ( int );
void marsdag ( void );
void jupdag ( void );
void satdag ( void );
void merdag ( void ); // Mercury myur dag
void vendag ( void ); // Venus myur dag

// T4.C: Routines for printing calendars:

void prn_cal ( void );
int chk_spec ( int, int );
int chk_sadag ( int, int );
void s_cyc ( void );


// UTILS.C:

void prn_lst6 ( int * );
void prn_lst7 ( int * );
void prn_lstd ( int *, int, int );
void mul_g6 ( int *, int, int, int );
void mul_g7 ( int *, int, int, int, int );
void div_g6 ( int *, int, int, int );
void sub_g6 ( int *, int *, int *, int, int, int
                       );
void add_g6 ( int *, int *, int *, int, int, int
                       );
void add_g7 ( int *, int *, int *, int, int, int,
              int );
void mul_gen ( int *, int *, int, int, int );
int exp_lst ( int *, int ); 
double conv2degs ( int *, int, int );
double solar_long ( int, double );
double conv2degs1 ( int *, int, int );
void add_gen ( int *, int *, int *, int, int );
void sub_gen ( int *, int *, int *, int, int );
void prn_lst ( int * );
void clrlst ( int * );
void clrlst6 ( int * );
void clear_a_b ( void );
void clear_a_b6 ( void );
void testbcd ( void );

void UTET ( int, int, int, int, int, int, double *, double * );
double AINT ( double );
double get_deltaT ( double );
double modfeh ( double, double );

int prfldat ( int, int, int, unsigned char * );
void start_print ( void );
void prin_str ( char * );
void addstrings ( char *, char * );

// TESTS.C:

void set_errfl ( void );
void wrt_errfl ( void );
void clos_errfl ( void );
void srch_cyc ( void );

