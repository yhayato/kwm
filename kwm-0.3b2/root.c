#include "kwm.h"

/******************** root window callbacks *****************/

int      moving;
static int      xoff, yoff;
static win      mwp;

void 
rt_press(ev, cd, bd)
	XButtonEvent   *ev;
	ClientData      cd, bd;
{
	/* XAllowEvents(ntdisp, SyncBoth, CurrentTime); */
	if (ev->state > 0x100)
		emerg_exit();
	if (ev->subwindow != 0) {
		mwp = lookwin(ev->subwindow);
		xoff = mwp->x - ev->x;
		yoff = mwp->y - ev->y;
		if (mwp->kind == IsIcon) {
			XUnmapWindow(ntdisp, mwp->mw);
			XMapRaised(ntdisp, mwp->iwp->mw);
			return;
		}
		/*
		if (-yoff > mwp->h / 2) {
			XUnmapWindow(ntdisp, mwp->mw);
			XMapWindow(ntdisp, mwp->iwp->mw);
			return;
		}
		*/
		moving = True;
		XGrabServer(ntdisp);
		invframe(mwp->x, mwp->y, mwp->w, mwp->h);
	}
}

void
rt_enter(ev, cd, bd)
	XCrossingEvent   *ev;
	ClientData      cd, bd;
{
	extern	int	already_set_Pointer ;
	extern	win	kwt ;

	already_set_Pointer = 0 ;

	if ( ev->subwindow != 0 ) {
		wp_select(kwt = lookwin(ev->subwindow) );
		XSetInputFocus(ntdisp, kwt->mw,
					RevertToPointerRoot, CurrentTime);
		/*
		XWarpPointer(ntdisp, None, ntroot, 0, 0, 0, 0, kwt->x+5, kwt->y+5) ;
		*/
	} else {
		XSetInputFocus(ntdisp, PointerRoot, None, CurrentTime);
	}
}

void 
rt_move(ev, cd, bd)
	XMotionEvent   *ev;
	ClientData      cd, bd;
{
	extern	int	already_set_Pointer ;
	extern	win	kwt ;
	static	win		wp ;
	static	Window window ;

	if (!moving) {
		already_set_Pointer = 0 ;
		XSetInputFocus(ntdisp, PointerRoot, None, CurrentTime);
		return ;
	}
	if (mwp->x == ev->x + xoff && mwp->y == ev->y + yoff)
		return;
	invframe(mwp->x, mwp->y, mwp->w, mwp->h);
	invframe(mwp->x = ev->x + xoff, mwp->y = ev->y + yoff, mwp->w, mwp->h);
}

void 
rt_release(ev, cd, bd)
	XButtonEvent   *ev;
	ClientData      cd, bd;
{
	if (!moving)
		return;
	invframe(mwp->x, mwp->y, mwp->w, mwp->h);
	XMoveWindow(ntdisp, mwp->mw, ev->x + xoff, ev->y + yoff);
	XUngrabServer(ntdisp);
	/* XAllowEvents(ntdisp, SyncBoth, CurrentTime); */
	moving = False;
}

void 
rt_resize(ev, cd, bd)
	XResizeRequestEvent *ev;
	ClientData      cd, bd;
{
	XResizeWindow(ntdisp, ev->window, ev->width, ev->height);
}

void 
rt_config(ev, cd, bd)
	XConfigureRequestEvent *ev;
	ClientData      cd, bd;
{
	XWindowChanges  ch;
	XWindowAttributes attr;

	ch.x = ev->x;
	ch.y = ev->y;
	ch.width = ev->width;
	ch.height = ev->height;
	ch.border_width = ev->border_width;
	ch.sibling = ch.stack_mode = TopIf ;
	XConfigureWindow(ntdisp, ev->parent, ev->value_mask, &ch);
	XConfigureWindow(ntdisp, ev->window, ev->value_mask, &ch);

	/* try to get additional info */
	XGetWindowAttributes(ntdisp,ev->window,&attr);
	ch.x = attr.x;
	ch.y = attr.y;
	ch.width = attr.width;
	ch.height = attr.height;
	ch.border_width = attr.border_width;

	XResizeWindow(ntdisp, ev->window, ch.width, ch.height) ;
}

void 
rt_configed(ev, cd, bd)
	XConfigureEvent *ev;
	ClientData      cd, bd;
{
	win             wp = lookwin(ev->window);
	wp->x = ev->x;
	wp->w = ev->y;
	wp->w = ev->width;
	wp->h = ev->height;
	if (wp->kind == IsNormal)
		XMoveWindow(ntdisp, wp->nw, wp->w - 12, 0);
}

void 
rt_map(ev, cd, bd)
	XMapRequestEvent *ev;
	ClientData      cd, bd;
{
	Window          w1;
	int             x, y, w, h, b, d;
	XWindowAttributes attr;

	win             wp = lookwin(ev->window);

	/* try to get correct info */
	XGetWindowAttributes(ntdisp,ev->window,&attr);
	wp->w = attr.width;
	wp->h = attr.height;

	if (wp->kind == IsPrepare) {
		if ( wp->x == 0 && wp->y == 0 ) {
			placing(wp->w, wp->h, &wp->x, &wp->y);
		}
		wp->kind = IsNormal;
	}
	if (dbg > 4)
		printf("mapreq, id=%x, kind=%d\n", wp->mw, wp->kind);
	XMoveWindow(ntdisp, ev->window, wp->x, wp->y);
	XMapRaised(ntdisp, ev->window);
}

void 
rt_mapped(ev, cd, bd)
	XMapEvent      *ev;
	ClientData      cd, bd;
{
	win             wp = lookwin(ev->window);
	if (dbg > 4)
		printf("mapped, id=%x, kind=%d\n", wp->mw, wp->kind);
	wp->map = IsViewable;
	if (wp->kind == IsNormal)
		XUnmapWindow(ntdisp, wp->iwp->mw);
}

void 
rt_unmap(ev, cd, bd)
	XUnmapEvent    *ev;
	ClientData      cd, bd;
{
	win             wp = lookwin(ev->window);
	wp->map = IsUnmapped;
}

void 
rt_create(ev, cd, bd)
	XCreateWindowEvent *ev;
	ClientData      cd, bd;
{
	win             wp = lookwin(ev->window);
	wp->nm = 0;
	if (wp->kind == IsUnknown)
		wp->kind = IsPrepare;
	if (dbg > 4)
		printf("create, id=%x\n", wp->mw);
}

void 
rt_destroy(ev, cd, bd)
	XDestroyWindowEvent *ev;
	ClientData      cd, bd;
{
	delwin(ev->window);
}

void 
rt_screenresize(ev, cd, bd)
	XDestroyWindowEvent *ev;
	ClientData      cd, bd;
{
  int             x, y, b, d;  
  Window          w1;
  XGetGeometry(ntdisp, ntroot, &w1, &x, &y, &ntroot_w, &ntroot_h, &b, &d);
  ntroot_w-=2;
  ntroot_h-=2;
}

/******************** end root window callbacks *************/
