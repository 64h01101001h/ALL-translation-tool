/* DOS conio.h shim for capturing reference outputs */
#include <stdio.h>
static int getch(void){int c=getchar(); return c<0?27:c;}
static int getche(void){return getch();}
static void clrscr(void){}
static int kbhit(void){return 1;}
