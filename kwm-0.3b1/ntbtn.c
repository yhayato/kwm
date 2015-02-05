#include "nt.h"
#include <X11/Xutil.h>

typedef struct _ntbtn {
	Window          mw;
	int             w, h;
	Pixmap          pix;
	long            mask;
}               ntbtn_r, *ntbtn;

extern Display *ntdisp;
extern GC       ntfgc, ntggc;
extern unsigned long ntblbit, ntwhbit;

static void     redrawpix();

ntbtn 
ntb_make(w0, x, y, w, h, b)
	Window          w0;
	int             x, y, w, h, b;
{
	ntbtn           bt = (ntbtn) malloc(sizeof(ntbtn_r));
	if ( bt == NULL )
		emerg_exit() ;
	bt->mw = XCreateSimpleWindow(ntdisp, w0, x, y, w, h, b, ntblbit, ntwhbit);
	bt->w = w;
	bt->h = h;
	bt->mask = 0;
	XMapWindow(ntdisp, bt->mw);
	return bt;
}

ntbtn 
ntb_makebm(w0, x, y, b, f)
	Window          w0;
	int             x, y, b;
	char           *f;
{
	Pixmap          pix;
	ntbtn           bt = (ntbtn) malloc(sizeof(ntbtn_r));
	if 	( bt == NULL )
		emerg_exit() ;
	bt->pix = ntbmget(f, &bt->w, &bt->h);
	bt->mw = XCreateSimpleWindow(ntdisp, w0,
				   x, y, bt->w, bt->h, b, ntblbit, ntwhbit);
	bt->mask = 0;
	ntaddcb(Expose, bt->mw, redrawpix, (ClientData) bt, NULL);
	ntaddcb(NoExpose, bt->mw, NULL, NULL, NULL);
	XSelectInput(ntdisp, bt->mw, (bt->mask |= ExposureMask));
	XMapWindow(ntdisp, bt->mw);
	return bt;
}

static void 
redrawpix(ev, cd, bd)
	XEvent         *ev;
	ClientData      cd, bd;
{
	ntbtn           bt = (ntbtn) cd;
	XCopyArea(ntdisp, bt->pix, bt->mw, ntggc, 0, 0, bt->w, bt->h, 0, 0);
}

int 
ntb_setcb(bt, f, cd, bd)
	ntbtn           bt;
	void            (*f) ();
ClientData      cd, bd; {
	ntaddcb(ButtonPress, bt->mw, f, cd, bd);
	XSelectInput(ntdisp, bt->mw, (bt->mask |= ButtonPressMask));
}
