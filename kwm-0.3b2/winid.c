#include "kwm.h"

/******************** winID initialize **********************/

void 
get1stattr(wp)
	win             wp;
{
	XWindowAttributes atr;
	XWMHints       *wmh;
	if (XGetWindowAttributes(ntdisp, wp->mw, &atr) == 0) {
		wp->mw = 0;
		return;
	}
	wp->x = atr.x;
	wp->y = atr.y;
	wp->w = atr.width;
	wp->h = atr.height;
	wp->b = atr.border_width;
	wp->map = atr.map_state;
	if (atr.override_redirect) {
		wp->kind = IsReserved;
		return;
	}
	XFetchName(ntdisp, wp->mw, &wp->name);
	if (wp->name == NULL) {
		static          count = 0;
		static char     buf[100];
		snprintf(buf,100,"Win#%d\0", count++);
		XStoreName(ntdisp, wp->mw, buf);
		XFetchName(ntdisp, wp->mw, &wp->name);
	}
	XGetIconName(ntdisp, wp->mw, &wp->iname);
	if (wp->iname == NULL)
		wp->iname = wp->name;
	if (wp->kind == IsIcon)
		return;
	wmh = wp->wmh = XGetWMHints(ntdisp, wp->mw);
	if (wp->wmh == NULL) {
		if ( ( wmh = wp->wmh = (XWMHints *) malloc(sizeof(XWMHints))) == NULL ) 
			emerg_exit() ;
		wmh->flags = 0;
	}
	if ((wmh->flags & IconWindowHint) && wmh->icon_window != 0) {
		makeicon(wp, wmh->icon_window);
	} else {
		extern long     ntblbit, ntwhbit;
		makeicon(wp, NULL);
		wmh->icon_window = wp->iwp->mw;
		wmh->flags |= IconWindowHint;
	}
	if (wp->map == IsUnmapped) {
		XMapWindow(ntdisp, wp->iwp->mw);
		XLowerWindow(ntdisp, wp->iwp->mw);
	}
	XSetWMHints(ntdisp, wp->iwp->mw, wmh);
}

void 
get1stinfo()
{
	Window          w1, w2, *cws;
	int             i, n;
	XGrabServer(ntdisp);
	XQueryTree(ntdisp, ntroot, &w1, &w2, &cws, &n);
	XUngrabServer(ntdisp);
	if (dbg > 4)
		printf("no. of windows=%d\n", n);
	for (i = 0; i < n; ++i) {
		win             wp = lookwin1(cws[i]);
		if ( wp != NULL ){
		  get1stattr(wp);
		  if (wp->kind == IsUnknown)
			wp->kind = IsNormal;
		  if (dbg > 4)
			if ( wp -> iwp != NULL ){
			  printf("id=%x, ic=%x, name=%s\n", wp->mw, wp->iwp->mw, wp->name);
			}else{
			  printf("id=%x, ic=NULL, name=%s\n", wp->mw, wp->name);
			}			  
		}
	}
	XFree(cws);
	if (dbg > 4)
		showinfo();
}

/******************** end winID initialize ******************/

/******************** winID managers ************************/

static win_r    aw[200];
static int      naw = 0;
static int      awc = 0;

showinfo()
{
	register int    i;
	for (i = 0; i < naw; ++i) {
	  win             wp = &aw[i];
	  if ( wp != NULL ){
		if ( wp->iwp == NULL ){
		  printf("bw=%x ic=NULL kind=%d\n", wp->mw, wp->kind);
		}else{
		  printf("bw=%x ic=%x kind=%d\n", wp->mw, wp->iwp->mw, wp->kind);
		}
	  }
	}
}

getchange()
{
	return awc;
}

void 
getinfo(awp, nwp)
	win            *awp;
	int            *nwp;
{
	register int    i, j;
	for (i = j = 0; i < naw; ++i) {
		win             wp = &aw[i];
		if (wp->kind == IsNormal || wp->kind == IsPrepare)
			awp[j++] = wp;
	}
	if (dbg > 4)
		printf("getinfo, no=%d\n", j);
	if (dbg > 4)
		showinfo();
	*nwp = j;
}

win 
lookwin1(mw)
	Window          mw;
{
	register int    i;
	register win    wp;
	register win    wp1 = NULL;

	for (i = 0; i < naw; ++i) {
		wp = &aw[i];
		if (wp->mw == mw)
			return wp;
		if ((wp1 == NULL) && (wp->mw == 0)) {
			wp1 = wp;
		}
	}
	if (wp1 == NULL)
		wp1 = &aw[naw++];
	if (dbg > 4)
		printf("#%d, id=%x\n", naw, mw);
	wp1->mw = mw;
	wp1->kind = IsUnknown;
	wp1->nm = 0;
	wp1->sel = False;
	++awc;
	return wp1;
}

win 
lookwin(mw)
	Window          mw;
{
	register win    wp = lookwin1(mw);
	if (wp->kind == IsUnknown)
		get1stattr(wp);
	return wp;
}

void 
delwin(mw)
	Window          mw;
{
	register int    i;

	for (i = 0; i < naw; ++i) {
		win             wp = &aw[i];
		if (wp->mw == mw) {
			if (wp->kind != IsDeleted) {
				if (wp != NULL && wp->iwp != 0 && wp->iwp->mw != 0) {
					XUnmapWindow(ntdisp, wp->iwp->mw);
					XDestroyWindow(ntdisp, wp->iwp->mw);
				}
				if (wp != NULL && wp->mw != 0) {
					XUnmapWindow(ntdisp, wp->mw);
					XDestroyWindow(ntdisp, wp->mw);
				}
			}
			wp->kind = IsDeleted;
			wp->mw = 0;
			wp->nw = 0;
			wp->sel = False;
			if (wp->iwp != NULL) {
				wp->iwp->mw = 0;
				wp->iwp = NULL;
			}
		}
	}
	++awc;
}
 
/******************** end winID managers ********************/

/******************** wp id managers ************************/

win 
wp_lookup(c)
	char            c;
{
	register int    i;
	for (i = 0; i < naw; ++i) {
		win             wp = &aw[i];
		if (wp->nm == c)
			return wp;
	}
	return NULL;
}

win 
wp_setid(bwp, c)
	win             bwp;
	char            c;
{
	register int    i;
	for (i = 0; i < naw; ++i) {
		win             wp = &aw[i];
		if (wp->nm == c) {
			XUnmapWindow(ntdisp, wp->nw);
			wp->nm = 0;
		}
	}
	bwp->nm = c;
	XUnmapWindow(ntdisp, bwp->nw);
	XMapRaised(ntdisp, bwp->nw);
}

wp_select(bwp)
	win             bwp;
{
	win             wp;

	register int    i;
	for (i = 0; i < naw; ++i) {
		win             wp = &aw[i];
		if (wp != bwp && wp->kind == IsNormal && wp->sel && wp->nm) {
			wp->sel = False;
			XUnmapWindow(ntdisp, wp->nw);
			XMapRaised(ntdisp, wp->nw);
		}
	}
	bwp->sel = True;
	if (bwp->nm) {
		XUnmapWindow(ntdisp, bwp->nw);
		XMapRaised(ntdisp, bwp->nw);
	}
}

/******************** end id managers ***********************/

/******************** window edge routines ******************/

findedge(dir, edge, init, lim)
{
	register int    v = init;
	register int    z, i;
	for (i = -1; i < naw; ++i) {
		if (i == -1)
			switch (edge) {
			case E_LEFT:
				z = ntroot_w;
				break;
			case E_RIGHT:
				z = 0;
				break;
			case E_TOP:
				z = ntroot_h;
				break;
			case E_BOTTOM:
				z = 0;
				break;
			}
		else {
			win             wp = &aw[i];
			if (wp->kind != IsNormal || wp->map != IsViewable)
				continue;
			switch (edge) {
			case E_LEFT:
				z = wp->x;
				break;
			case E_RIGHT:
				z = wp->x+wp->w;
				break;
			case E_TOP:
				z = wp->y;
				break;
			case E_BOTTOM:
				z = wp->y+wp->h;
				break;
			}
		}
		if (dir == E_MAX) {
			if (lim > z && z > v)
				v = z;
		} else {
			if (lim < z && z < v)
				v = z;
		}
	}
	return v;
}

/******************** end window edge routines **************/

/******************** window placing routines ***************/

typedef struct _box {
	int             x, y, w, h;
}               box_r, *box;

static box     *ws;
static int      wn;
bool            split();

void 
placing(w, h, xpos, ypos)
	int             w, h, *xpos, *ypos;
{
	register int    i, j, k, s;
	box_r           b, *wsa[200];
	for (i = 0; i < 200; ++i)
		wsa[i] = NULL;
	ws = wsa;
	wn = 1;
	if ( NULL == ( ws[0] = (box) malloc(sizeof(box_r)) ) )
		emerg_exit() ;
	ws[0]->x = ws[0]->y = 0;
	ws[0]->w = ntroot_w;
	ws[0]->h = ntroot_h;
	for (i = 0; i < naw; ++i) {
		win             wp = &aw[i];
		if (wp->map != IsViewable || wp->kind != IsNormal)
			continue;
		b.x = wp->x;
		b.y = wp->y;
		b.w = wp->w;
		b.h = wp->h;
		for (j = 0; j < wn; ++j)
			if (split(ws[j], &b, w, h))
				ws[j] = NULL;
		for (j = k = 0; j < wn; ++j)
			if (ws[j] != NULL)
				ws[k++] = ws[j];
		wn = k;
	}
	s = 0;
	i = -1;
	for (j = 0; j < wn; ++j) {
		register box    wp = ws[j];
		if (wp->w * wp->h > s) {
			s = wp->w * wp->h;
			i = j;
		}
	}
	*xpos = *ypos = 0;
	if (i >= 0) {
		*xpos = ws[i]->x;
		*ypos = ws[i]->y;
	}
}

#define hix(p) (p->x+p->w)
#define hiy(p) (p->y+p->h)

bool 
split(p, q, tx, ty)
	box             p, q;
	int             tx, ty;
{
	if (hix(p) <= q->x || p->x >= hix(q) || hiy(p) <= q->y || p->y >= hiy(q))
		return FALSE;
	if (p->x+tx < q->x) {
		box             r = (box) malloc(sizeof(box_r));
		if ( r == NULL )
			emerg_exit() ;
		r->x = p->x;
		r->y = p->y;
		r->w = q->x-p->x;
		r->h = p->h;
		ws[wn++] = r;
	}
	if (hix(p) > hix(q)+tx) {
		box             r = (box) malloc(sizeof(box_r));
		if ( r == NULL )
			emerg_exit() ;
		r->x = hix(q);
		r->y = p->y;
		r->w = hix(p)-hix(q);
		r->h = p->h;
		ws[wn++] = r;
	}
	if (p->y+ty < q->y) {
		box             r = (box) malloc(sizeof(box_r));
		if ( r == NULL )
			emerg_exit() ;
		r->x = p->x;
		r->y = p->y;
		r->w = p->w;
		r->h = q->y-p->y;
		ws[wn++] = r;
	}
	if (hiy(p) > hiy(q)+ty) {
		box             r = (box) malloc(sizeof(box_r));
		if ( r == NULL )
			emerg_exit() ;
		r->x = p->x;
		r->y = hiy(q);
		r->w = p->w;
		r->h = hiy(p)-hiy(q);
		ws[wn++] = r;
	}
	free(p);
	return TRUE;
}

/******************** end window placing routines ***********/

/******************** utility routines **********************/

/*
 * invframe -- draw wireframe by xor.
 */

invframe(x, y, w, h)
	int             x, y, w, h;
{
	extern GC       ntxgc;
	XPoint			pts[5];

	h--;

	pts[0].x = x;
	pts[0].y = y;
	pts[1].x = w;
	pts[1].y = 0;
	pts[2].x = 0;
	pts[2].y = h;
	pts[3].x = -w;
	pts[3].y = 0;
	pts[4].x = 0;
	pts[4].y = -h;

	XDrawLines(ntdisp, ntroot, ntxgc, pts, 5, CoordModePrevious);

}

/*
 * emerg_exit -- reset input focus and exit
 */

emerg_exit()
{
	register int    i;
	for (i = 0; i < naw; ++i) {
		win             wp = &aw[i];
		if (wp->mw && wp->kind == IsNormal)
			XMapWindow(ntdisp, wp->mw);
	}
	XSetInputFocus(ntdisp, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSync(ntdisp, 0);
	exit(1);
}

/******************** end utility routines ******************/
