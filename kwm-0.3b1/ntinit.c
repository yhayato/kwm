#include "nt.h"

Display        *ntdisp;
unsigned long   ntblbit;
unsigned long   ntwhbit;
Window          ntroot;
int             ntroot_w, ntroot_h;
GC              ntfgc, ntggc, ntxgc, ntngc, ntngci, ntlgc;


int 
ntinit()
{
	XGCValues       mgv;
	int             x, y, b, d;
	unsigned long   m = GCFunction | GCFont | GCForeground | GCBackground;
	Window          w1;
	ntdisp = XOpenDisplay(NULL);
	ntblbit = BlackPixel(ntdisp, 0);
	ntwhbit = WhitePixel(ntdisp, 0);
	ntroot = RootWindow(ntdisp, 0);
	XGetGeometry(ntdisp, ntroot, &w1, &x, &y, &ntroot_w, &ntroot_h, &b, &d);

	ntroot_w-=2;
	ntroot_h-=2;

	ntftinit();
	/* ntbminit(); */
	mgv.function = GXcopyInverted;

	mgv.foreground = BlackPixel(ntdisp, 0) ;
	mgv.background = WhitePixel(ntdisp, 0) ;
	mgv.font = ntftget("large", NULL);
	ntlgc = XCreateGC(ntdisp, ntroot, m, &mgv);

	mgv.foreground = WhitePixel(ntdisp, 0) ; /* 1 */
	mgv.background = BlackPixel(ntdisp, 0) ; /* 0 */
	mgv.font = ntftget("icon", NULL);
	ntfgc = XCreateGC(ntdisp, ntroot, m, &mgv);
	mgv.font = ntftget("norm", NULL);
	ntngc = XCreateGC(ntdisp, ntroot, m, &mgv);
	/*
	mgv.foreground = 0;
	mgv.background = 1;
	*/
	mgv.foreground = BlackPixel(ntdisp, 0) ;
	mgv.background = WhitePixel(ntdisp, 0) ;
	ntngci = XCreateGC(ntdisp, ntroot, m, &mgv);
	mgv.function = GXcopy;
	ntggc = XCreateGC(ntdisp, ntroot, GCFunction | GCFont, &mgv);
	mgv.function = GXinvert;
	mgv.subwindow_mode = IncludeInferiors;
	mgv.graphics_exposures = False;
	ntxgc = XCreateGC(ntdisp, ntroot,
		  GCFunction | GCGraphicsExposures | GCSubwindowMode, &mgv);
}
