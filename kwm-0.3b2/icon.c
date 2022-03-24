#include "kwm.h"

/******************** icon handlers *************************/

static void     ic_view(XExposeEvent *ev, ClientData cd, ClientData bd);
static void     ic_click(XButtonEvent *ev, ClientData cd, ClientData bd);
static void     nw_view(XExposeEvent *ev, ClientData cd, ClientData bd);

void makeicon(win , Window );

#define	ICON_W	5
#define	ICON_H	5

void
makeicon(win bwp, Window mw)
{
	static int      count = 0;
	XSetWindowAttributes atr;
	win             wp;
	int             h = ntroot_h - ICON_H;	/* H / 72 / 3 * 2 */
	int             y = ++count;
	int             x = ntroot_w - ICON_W;

	while (y > h) {
		y -= h;
		x -= ICON_W;
	}
	y = ntroot_h - ICON_H * y;
	atr.background_pixmap = ParentRelative;
	atr.background_pixel = BlackPixel(ntdisp, 0);
	atr.override_redirect = True;
	if (mw == 0) {
		mw = XCreateWindow(ntdisp, ntroot, x, y, ICON_W, ICON_H,
			     1, CopyFromParent, InputOutput, CopyFromParent,
				   CWBackPixel | CWOverrideRedirect, &atr);
		wp = lookwin1(mw);
		wp->x = x;
		wp->y = y;
		wp->w = ICON_W;
		wp->h = ICON_H;
	} else {
		Window          w1;
		int             i, b;
		wp = lookwin1(mw);
		XGetGeometry(ntdisp, wp->mw, &w1,
			     &wp->x, &wp->y, &wp->w, &wp->h, &b, &i);
	}
	wp->kind = IsIcon;
	wp->name = bwp->name;
	wp->iname = bwp->iname;
	wp->iwp = bwp;
	bwp->iwp = wp;
	if (dbg > 4)
		printf("bw=%x, iw=%x (%d,%d)\n", bwp->mw, wp->mw, wp->x, wp->y);
	XSelectInput(ntdisp, mw, ExposureMask | ButtonPressMask);
	ntaddcb(Expose, mw, ic_view, (ClientData) wp, NULL);
	ntaddcb(ButtonPress, mw, ic_click, (ClientData) wp, NULL);
	bwp->nw = XCreateWindow(ntdisp, bwp->mw, bwp->w - 12, 0, 12, 24, 1,
				CopyFromParent, InputOutput, CopyFromParent,
				CWBackPixel | CWOverrideRedirect, &atr);
	XSelectInput(ntdisp, bwp->nw, ExposureMask);
	ntaddcb(Expose, bwp->nw, nw_view, (ClientData) bwp, NULL);
}

static void
nw_view(XExposeEvent *ev, ClientData cd, ClientData bd)
{
	win             wp = (win) cd;
	extern GC       ntlgc, ntxgc;
	XClearWindow(ntdisp, wp->nw);
	if (wp->nm)
		XDrawImageString(ntdisp, wp->nw, ntlgc, 0, 22, &wp->nm, 1);
	if (wp->sel)
		XCopyArea(ntdisp, ntroot, wp->nw, ntxgc, 0, 0, 12, 24, 0, 0);
}

static void
ic_view(XExposeEvent *ev, ClientData cd, ClientData bd)
{

	win             wp = (win) cd;
	extern GC       ntfgc;

	if (dbg > 4)
		printf("icon expose, id=%x\n", wp->mw);
	if ( wp->iname != NULL ){
	  XDrawImageString(ntdisp, wp->mw, ntfgc, 0, 10,
					   wp->iname, strlen(wp->iname));
	}

}

static void
ic_click(XButtonEvent *ev, ClientData cd, ClientData bd)
{
	win             wp = (win) cd;
	XMapWindow(ntdisp, wp->iwp->mw);
	if (wp->mw != 0)
		XUnmapWindow(ntdisp, wp->mw);
}
/******************** end icon handlers *********************/
