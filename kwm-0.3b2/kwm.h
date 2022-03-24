#include "nt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xrandr.h>

extern int  dbg;

/******************** declarations *************************/

/*
 * callback routines
 */

void rt_create(), rt_destroy(), rt_map(), rt_config();
void rt_resize(), rt_unmap(), rt_mapped(), rt_configed();
void rt_press(), rt_release(), rt_move(), rt_key(XKeyEvent *ev, ClientData cd, ClientData bd);
void rt_enter() ;
void rt_raise() ;
void rt_screenresize();
void kw_key(XKeyEvent *ev, ClientData cd, ClientData bd), kw_view(XEvent *ev, ClientData cd, ClientData bd);
void ignore();



/*
 * _win holds toplevel window's state
 */

/* win->kind */

#define IsUnknown 0
#define IsNormal 1
#define IsIcon 2
#define IsPrepare 3
#define IsReserved 4
#define IsDeleted 5

typedef struct _win *win;

typedef struct _win {
  Window mw, nw;
  int sel;
  char nm;
  int kind, x, y, w, h, b, map;
  XWMHints *wmh;
  char *name;
  char * iname;
  win iwp;
} win_r;

/*
 * wp routines
 */

void get1stinfo(), get1stattr();
win lookwin(), lookwin1(), wp_lookup();
void placing();

/*
 * const for edge find routines
 */

#define E_LEFT    1
#define E_RIGHT   2
#define E_TOP     3
#define E_BOTTOM  4
#define E_MAX     5
#define E_MIN     6

/*
 * nt toolkit externals
 */

extern Display *ntdisp;
extern Window ntroot;
extern int ntroot_w, ntroot_h;

