#include "nt.h"

typedef struct ntlwin {
	Window          mw;
	unsigned long   mask;
	int             w, h, d;
	char          **lns;
	int            *lnl;
	int             len, lim;
}               ntlwin_r, *ntlwin;

extern Display *ntdisp;
extern GC       ntfgc;
extern unsigned long ntblbit, ntwhbit;

static void     redraw();

ntlwin 
ntl_make(w0, x, y, w, h, b)
	Window          w0;
	int             x, y, w, h, b;
{
	ntlwin          lw = (ntlwin) malloc(sizeof(ntlwin_r));
	if ( lw == NULL )
		emerg_exit() ;
	lw->mw = XCreateSimpleWindow(ntdisp, w0, x, y, w, h, b, ntblbit, ntwhbit);
	lw->w = w;
	lw->h = h;
	lw->d = 20;
	lw->mask = 0;
	if ( NULL == ( lw->lns = (char **) malloc(sizeof(char *) * 100) ) )
		emerg_exit() ;
	if ( NULL == ( lw->lnl = (int *) malloc(sizeof(int) * 100) ) )
		emerg_exit() ;
	lw->lim = 100;
	lw->len = 0;
	ntaddcb(Expose, lw->mw, redraw, (ClientData) lw, NULL);
	ntaddcb(NoExpose, lw->mw, NULL, NULL, NULL);
	XSelectInput(ntdisp, lw->mw, (lw->mask |= ExposureMask));
	XMapWindow(ntdisp, lw->mw);
	return lw;
}

static void 
redraw(ev, cd, bd)
	XEvent         *ev;
	ClientData      cd, bd;
{
	ntlwin          lw = (ntlwin) cd;
	register int    i = 0;
	register int    y = lw->d;
	while (y < lw->h) {
		XDrawString(ntdisp, lw->mw, ntfgc, 0, y, lw->lns[i], lw->lnl[i]);
		printf("[%d] %s:%d, %d,%d\n", i, lw->lns[i], lw->lnl[i], 0, y);
		++i;
		y += lw->d;
	}
	printf("redraw.\n");
}

void 
ntl_trim(lw)
	ntlwin          lw;
{
	lw->len = 0;
}

void 
ntl_addh(lw, s, l)
	ntlwin          lw;
	char           *s;
	int             l;
{
	register int    i = lw->len;
	if (lw->len >= lw->lim)
		return;
	printf("addh [%d] %s, %d\n", i, s, l);
	lw->lns[i] = s;
	lw->lnl[i] = l;
	lw->len += 1;
}

void 
ntl_store(lw, i, s, l)
	ntlwin          lw;
	char           *s;
	int             i, l;
{
	if (i < 0 || i >= lw->len)
		return;
	lw->lns[i] = s;
	lw->lnl[i] = l;
}

void 
ntl_update(lw)
	ntlwin          lw;
{
	redraw(NULL, (ClientData) lw, NULL);
}
