#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>


#define TRUE 1
#define FALSE 0
// #define NULL 0

typedef int *ClientData;
typedef int *Opaque;
typedef int bool;

/* ntfont.c */
void ntftinit();
Font ntftget();

/* ntbitmap.c */
void ntbminit();
Pixmap ntbmget();

