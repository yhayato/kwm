#include "kwm.h"

void kw_init(void);
void kw_exchange(int , int);
void kw_invert(int);
void kw_pack(int);
void kw_move(XKeyEvent *, ClientData, ClientData);
void kw_resize(XKeyEvent *, ClientData, ClientData);
void kw_key(XKeyEvent *, ClientData , ClientData);
void kw_view(XEvent *, ClientData , ClientData);





/******************** kwm window routines *******************/

/*
 * kw structure corresponds to kwm window
 */

#define KwNormal 0
#define KwMoving 1
#define KwMoving1 2
#define KwResizing 3
#define KwResizing1 4

static int      kst = KwNormal;

win             kwt = NULL;
static win      kwp;
static win      kws[100];
static int      kwn, kwc, kwi;
static Font     kft;
static XFontStruct *kfi;
static int      kfh;

/*
 * kw_init creates kwm window
 */

void
kw_init(void)
{
  Window          mw;
  XSetWindowAttributes atr;
  atr.background_pixmap = ParentRelative;
  atr.background_pixel = BlackPixel(ntdisp, 0);
  atr.override_redirect = True; 
  mw = XCreateWindow(ntdisp, ntroot, ntroot_w -150, 0, 150, ntroot_h,
					 2, CopyFromParent, InputOutput, CopyFromParent,
					 CWBackPixel | CWOverrideRedirect, &atr);
  kwp = lookwin1(mw);
  if ( kwp != NULL ){
	kwp->kind = IsNormal;
	kwp->x = ntroot_w-150;
	kwp->y = 0;
	kwp->w = 150;
	kwp->h = ntroot_h;
	ntaddcb(KeyPress, kwp->mw, kw_key, NULL, NULL);
	ntaddcb(Expose, kwp->mw, kw_view, NULL, NULL);
	kft = ntftget("norm", &kfi);
	kfh = kfi->max_bounds.ascent+kfi->max_bounds.descent;
	XSelectInput(ntdisp, kwp->mw, KeyPressMask | ExposureMask);
  }else{
	fprintf(stderr,"%s : Failed to create kwm menu window\n",
			__PRETTY_FUNCTION__);
	exit(1);
  }
}

/*
 * kw_exchange -- swap i-th entry and j-th entry
 */

void
kw_exchange(int i, int j)
{
  win             wp;
  wp = kws[i];
  kws[i] = kws[j];
  kws[j] = wp;
}

/*
 * kw_invert inverts contents of a window.
 */

void
kw_invert(int i)
{
  win             wp;
  extern GC       ntxgc;
  if (i <= 0 || i > kwn)
	return;
  wp = kws[i-1];
  if ( wp == NULL ){
	kw_pack(i);
  }
  if (wp->map == IsUnmapped)
	wp = wp->iwp;
  if (wp != NULL && wp->mw != 0)
	XCopyArea(ntdisp, ntroot, wp->mw, ntxgc, 0, 0, wp->w, wp->h, 0, 0);
}

void
kw_pack(int i)
{
  int j, new_kwn;

  if ( i == 0 ){
	j=1;
  }else{
	j = i;
  }

  if ( dbg > 4 ){
	printf("%s : Current kwn = %d  : ",__PRETTY_FUNCTION__,kwn);
  }
  new_kwn = kwn;
  while ( j <= kwn ){
	printf("%s : kwm     = %d\n",__PRETTY_FUNCTION__,kwn);
	printf("%s : new_kwm = %d\n",__PRETTY_FUNCTION__,new_kwn);
	printf("%s : kws[%d] = 0x%.8x\n",__PRETTY_FUNCTION__,j-1,kws[j-1]);
	if ( kws[j-1] == NULL ){
	  kws[j-1] = kws[j];
	  kws[j] = NULL;
	  new_kwn--;
	}
	j++;
  }

  kwn = new_kwn;

  if ( dbg > 4 ){
	printf("New kwn = %d\n",new_kwn);
  }
}


static	int	Pointer_x, Pointer_y;
int	already_set_Pointer = 0;

/*
 * kw_move -- move window
 */

void
kw_move(XKeyEvent *ev, ClientData cd, ClientData bd)
{
  win             wp = kwt;
  int             l, step;
  char            buf[21];
  XWindowAttributes	attr;
  int				x_save, y_save;


  if (wp == NULL || wp->kind != IsNormal)
	return;
  if (dbg > 0)
	printf("move key.\n");
  /* XAllowEvents(ntdisp, SyncBoth, CurrentTime); */
  step = 0;
  if (ev->state & ShiftMask)
	step = 16;
  if (ev->state & ControlMask)
	step >>= 2;
  if (ev->state & Mod1Mask)
	step >>= 2;
  ev->state = 0;
  if (step)
	ev->state = ShiftMask;
  l = XLookupString(ev, buf, 20, NULL, NULL);
  if (l != 1)
	return;
  if ((*buf == '\r') || (*buf == '')) {
	invframe(wp->x, wp->y, wp->w, wp->h);
	if (*buf == '\r') {
	  XMoveWindow(ntdisp, wp->mw, wp->x, wp->y);
	} else {
	  XGetWindowAttributes(ntdisp, wp->mw, &attr);
	  wp->x = attr.x;
	  wp->y = attr.y;
	  wp->w = attr.width;
	  wp->h = attr.height;
	}

	kst = KwNormal;
	XUngrabKeyboard(ntdisp, CurrentTime);
	XUngrabServer(ntdisp);
	return;
  }
  invframe(wp->x, wp->y, wp->w, wp->h);

  x_save = wp->x;
  y_save = wp->y;

  switch (*buf) {
  case 'j':
	wp->y = ntroot_h-wp->h;
	break;
  case 'k':
	wp->y = 0;
	break;
  case 'l':
	wp->x = ntroot_w-wp->w;
	break;
  case 'h':
	wp->x = 0;
	break;
  case 'n':
	if (wp->y+wp->h < ntroot_h) {
	  wp->y = findedge(E_MIN, E_BOTTOM, ntroot_h, l = wp->y);
	  wp->y = findedge(E_MIN, E_TOP, wp->y+wp->h, l+wp->h)-wp->h;
	}
	break;
  case 'p':
	if (wp->y > 0) {
	  wp->y = findedge(E_MAX, E_BOTTOM, -wp->h, l = wp->y);
	  wp->y = findedge(E_MAX, E_TOP, wp->y+wp->h, l+wp->h)-wp->h;
	}
	break;
  case 'f':
	if (wp->x+wp->w < ntroot_w) {
	  wp->x = findedge(E_MIN, E_RIGHT, ntroot_w, l = wp->x);
	  wp->x = findedge(E_MIN, E_LEFT, wp->x+wp->w, l+wp->w)-wp->w;
	}
	break;
  case 'b':
	if (wp->x > 0) {
	  wp->x = findedge(E_MAX, E_RIGHT, -wp->w, l = wp->x);
	  wp->x = findedge(E_MAX, E_LEFT, wp->x+wp->w, l+wp->w)-wp->w;
	}
	break;
  case 'J':
  case 'N':
	if (wp->y < ntroot_h)
	  wp->y += step;
	break;
  case 'K':
  case 'P':
	if (wp->y+wp->h > 0)
	  wp->y -= step;
	break;
  case 'L':
  case 'F':
	if (wp->x < ntroot_w)
	  wp->x += step;
	break;
  case 'H':
  case 'B':
	if (wp->x+wp->w > 0)
	  wp->x -= step;
	break;
  default:;
  }

  invframe(wp->x, wp->y, wp->w, wp->h);
}

/*
 * kw_resize -- resize window
 */

void
kw_resize(XKeyEvent *ev, ClientData cd, ClientData bd)
{
  win             wp = kwt;
  int             l, step;
  char            buf[21];
  XWindowAttributes	attr;
  int				w_save, h_save;

  if (wp == NULL || wp->kind != IsNormal)
	return;
  if (dbg > 0)
	printf("move key.\n");
  /* XAllowEvents(ntdisp, SyncBoth, CurrentTime); */
  step = 0;
  if (ev->state & ShiftMask)
	step = 16;
  if (ev->state & ControlMask)
	step >>= 2;
  if (ev->state & Mod1Mask)
	step >>= 2;
  ev->state = 0;
  if (step)
	ev->state = ShiftMask;
  l = XLookupString(ev, buf, 20, NULL, NULL);
  if (l != 1)
	return;
  if ((*buf == '\r') || (*buf == '')) {
	invframe(wp->x, wp->y, wp->w, wp->h);
	if (*buf == '\r') {
	  XMoveResizeWindow(ntdisp, wp->mw, wp->x, wp->y, wp->w, wp->h);
	} else {
	  XGetWindowAttributes(ntdisp, wp->mw, &attr);
	  wp->x = attr.x;
	  wp->y = attr.y;
	  wp->w = attr.width;
	  wp->h = attr.height;
	}
	kst = KwNormal;
	XUngrabKeyboard(ntdisp, CurrentTime);
	XUngrabServer(ntdisp);
	return;
  }
  invframe(wp->x, wp->y, wp->w, wp->h);

  w_save = wp->w;
  h_save = wp->h;

  switch (*buf) {
  case 'n':
  case 'j':
	wp->h = findedge(E_MIN, E_TOP, ntroot_h, wp->y+wp->h)-wp->y;
	if (wp->y+wp->h >= ntroot_h)
	  wp->h = ntroot_h-wp->y;
	break;
  case 'p':
  case 'k':
	wp->h = findedge(E_MAX, E_TOP, wp->y+(wp->h/2), wp->y+wp->h)-wp->y;
	if (wp->h < 2)
	  wp->h = 2;
	break;
  case 'f':
  case 'l':
	wp->w = findedge(E_MIN, E_RIGHT, ntroot_w, wp->x+wp->w)-wp->x;
	if (wp->x+wp->w > ntroot_w)
	  wp->w = ntroot_w-wp->x;
	break;
  case 'h':
  case 'b':
	wp->w = findedge(E_MAX, E_RIGHT, wp->x+(wp->w/2), wp->x+wp->w)-wp->x;
	if (wp->w < 2)
	  wp->w = 2;
	break;
  case 'N':
  case 'J':
	if ((wp->y+wp->h+step) < ntroot_h)
	  wp->h += step;
	break;
  case 'K':
  case 'P':
	if (wp->h-step > 2)
	  wp->h -= step;
	break;
  case 'F':
  case 'L':
	if ((wp->x+wp->w+step) < ntroot_w)
	  wp->w += step;
	break;
  case 'B':
  case 'H':
	if (wp->w-step > 2)
	  wp->w -= step;
	break;
  default:;
  }

  invframe(wp->x, wp->y, wp->w, wp->h);
}

/*
 * kw-key handles keypress within kwm window.
 */

void 
kw_key(XKeyEvent *ev, ClientData cd, ClientData bd)
{
  win             wp;
  int             l;
  char            buf[21];
  l = XLookupString(ev, buf, 20, NULL, NULL);

  if (kwi < 1){
	kwi = 1;
  }
  if (l == 1){
	switch(*buf) {
	case 'w' :
	case '' :
	  if ( kwp != NULL ){
		XUnmapWindow(ntdisp, kwp->mw);
	  }else{
		fprintf(stderr,"%s : Failed to unmap menu window ( ESC )\n",
				__PRETTY_FUNCTION__);
	  }
	  break;
	case 'n' :
	case 'j' :
	  if (kwi < kwn)
		++kwi;
	  if ( kwt == NULL ){
		kw_pack(kwi);
	  }
	  if (kwi > kwn)
		kwi = kwn;
	  kwt = kws[kwi-1];
	  kw_view(NULL, NULL, NULL);
	  XGrabServer(ntdisp);
	  kw_invert(kwi);
	  XSync(ntdisp,0);
	  kw_invert(kwi);
	  XUngrabServer(ntdisp);
	  break;
	case 'p' :
	case 'k' :
	  if (kwi > 1)
		--kwi;
	  if ( kwt == NULL ){
		kw_pack(kwi);
	  }
	  if (kwi > kwn)
		kwi = kwn;
	  kwt = kws[kwi-1];
	  kw_view(NULL, NULL, NULL);
	  XGrabServer(ntdisp);
	  kw_invert(kwi);
	  XSync(ntdisp,0);
	  kw_invert(kwi);
	  XUngrabServer(ntdisp);
	  break;
	case '0' :
	case '1' :
	case '2' :
	case '3' :
	case '4' :
	case '5' :
	case '6' :
	case '7' :
	case '8' :
	case '9' :
	  kwt = kws[kwi-1];
	  if ( kwt != NULL ){
		wp_setid(kwt, *buf);
		kw_view(NULL, NULL, NULL);
	  }
	  break;
	case 'S' :
	  if ( kwp != NULL ){
		XUnmapWindow(ntdisp, kwp->mw);
	  }else{
		fprintf(stderr,"%s : Failed to unmap menu window ( S )\n",
				__PRETTY_FUNCTION__);
	  }
	  XSetInputFocus(ntdisp, PointerRoot, None, CurrentTime);
	  break;
	}
  }
  if (kwi < 1){
	kwi = 1;
	return;
  }
  if (kwi > kwn){
	kwi = kwn;
	return;
  }
  wp = kwt = kws[kwi-1];
  if ( wp == NULL ){
	kw_pack(kwi);
	return;
  }
  if ( wp->iwp == NULL ){
	fprintf(stderr,
			"%s : Something strange : wp != 0x%.8x but wp->iwp == NULL",
			__PRETTY_FUNCTION__,wp);
	fprintf(stderr,"%s : Clearing this entry\n",__PRETTY_FUNCTION__);
	kws[kwi-1] = NULL;
	kwt = NULL;
	wp = NULL;
	kw_pack(kwi);
	return;
  }
  switch(*buf) {
  case 'I' :
	XMapWindow(ntdisp, wp->mw);
	XUnmapWindow(ntdisp, wp->iwp->mw);
	wp->map = IsViewable;
	break;
  case 'i' :
	XUnmapWindow(ntdisp, wp->mw);
	break;
  case 'h' :
	XMoveWindow(ntdisp, wp->mw, 0, 0);
	break;
  case 'H' :
	XMoveResizeWindow(ntdisp, wp->mw, 0, 0, 100, 100);
  case 'f' :
	XRaiseWindow(ntdisp, wp->mw);
	break;
  case 'F' :
	XLowerWindow(ntdisp, wp->mw);
	break;
  case 'm' :
	kst = KwMoving;
	XGrabKeyboard(ntdisp, ntroot, False, GrabModeAsync,
				  GrabModeAsync, CurrentTime);
	XGrabServer(ntdisp);
	invframe(wp->x, wp->y, wp->w, wp->h);
	break;
  case 'r' :
	kst = KwResizing;
	XGrabKeyboard(ntdisp, ntroot, False, GrabModeAsync,
				  GrabModeAsync, CurrentTime);
	XGrabServer(ntdisp);
	invframe(wp->x, wp->y, wp->w, wp->h);
	break;
  case 's' :
	XMapRaised(ntdisp, wp->mw);
	XUnmapWindow(ntdisp, wp->iwp->mw);
	XSync(ntdisp,0);
	wp_select(wp);
	XSetInputFocus(ntdisp, wp->mw, RevertToPointerRoot, CurrentTime);
	/*	XWarpPointer(ntdisp, None, ntroot, 0, 0, 0, 0, wp->x+5, wp->y+5);*/
	already_set_Pointer = 0;
	if ( kwp != NULL ){
	  XUnmapWindow(ntdisp, kwp->mw);
	}else{
	  fprintf(stderr,"%s : Failed to unmap menu window ( s )\n",
			  __PRETTY_FUNCTION__);
	}
	break;
  }
}

/*
 * kw_view update contents of the kwm window.
 */

void 
kw_view(XEvent *ev, ClientData cd, ClientData bd)
{
  extern GC       ntngc, ntngci;
  register int    v = kfh;
  register int    i;
  if (kwc < getchange()) {
	getinfo(kws, &kwn);
	kwc = getchange();
  }
  if (dbg > 4)
	printf("kwview, n=%d.\n", kwn);
  if ( kwp != NULL ){
	XClearWindow(ntdisp, kwp->mw);
	kw_pack(0);
	for (i = 0; i < kwn; ++i) {
	  register win    wp = kws[i];
	  GC              g = (i == kwi-1) ? ntngci : ntngc;
	if ((wp != NULL) && (wp->nm != 0) &&
		(kwp != NULL) && (kwp->mw != 0))
	  XDrawImageString(ntdisp, kwp->mw, g, 4, v, &wp->nm, 1);
	if ((kwp != NULL) && (kwp->mw != 0)
		&& (wp != NULL) && (wp->name != NULL))
	  XDrawImageString(ntdisp, kwp->mw, g, 16, v,
					   wp->name, strlen(wp->name));
	v += kfh;
	}
  }else{
	fprintf(stderr,
			"%s : Failed to refresh kwm menu window\n",__PRETTY_FUNCTION__);
  }
}

/******************** end kwm window routines ***************/

/******************** root key handler **********************/

/*
 * rt_key handles grabbed key (default shift-ctrl-X).
 */

void 
rt_key(XKeyEvent *ev, ClientData cd, ClientData bd)
{
  int             l, step = 32;
  char            buf[21];
  win             wp = kwt;
  Window		  focus;
  int			  revert;


  if (kst == KwMoving) {
	kw_move(ev, cd, bd);
	return;
  }

  if (kst == KwResizing) {
	kw_resize(ev, cd, bd);
	return;
  }

  if (ev->state & Mod1Mask)
	step >>= 2;

  ev->state = 0;
  l = XLookupString(ev, buf, 20, NULL, NULL);

  if (l == 1){
	switch (*buf) {
	case '\\' :
	  restart();
	  break;
	case 's' :
	  XSetInputFocus(ntdisp, PointerRoot, None, CurrentTime);
	  break;
	case 'w' :
	  if ( kwp != NULL ){
		XMapRaised(ntdisp, kwp->mw);
		XSetInputFocus(ntdisp, kwp->mw, RevertToPointerRoot, CurrentTime);
	  }else{
		fprintf(stderr,"%s : Failed to map_raised menu window ( s )\n",
				__PRETTY_FUNCTION__);
	  }
	  break;
	case 'm' :
	  if (wp != NULL && wp->mw != 0 && wp->nw != 0) {
		if (dbg > 0)
		  printf("move start.\n");
		kst = KwMoving;
		XGrabKeyboard(ntdisp, ntroot, False,
					  GrabModeAsync, GrabModeAsync, CurrentTime);
		XGrabServer(ntdisp);
		invframe(wp->x, wp->y, wp->w, wp->h);
	  }
	  break;
	case 'z' :
	  if (wp != NULL && wp->mw != 0 && wp->nw != 0)
		XMoveResizeWindow(ntdisp, wp->mw, wp->x, 0, wp->w, ntroot_h);
	  break;
	case 'r' :
	  if (wp != NULL && wp->mw != 0 && wp->nw != 0) {
		kst = KwResizing;
		XGrabKeyboard(ntdisp, ntroot, False,
					  GrabModeAsync, GrabModeAsync, CurrentTime);
		XGrabServer(ntdisp);
		invframe(wp->x, wp->y, wp->w, wp->h);
	  }
	  break;
	case 'u' :
	  if (wp != NULL && wp->mw != 0)
		XRaiseWindow(ntdisp, wp->mw);
	  break;
	case 'd' :
	  if (wp != NULL && wp->mw != 0)
		XLowerWindow(ntdisp, wp->mw);
	  break;
	case 'i' :
	  if (wp != NULL && wp->mw != 0 && wp->iwp->mw != 0) {
		XMapWindow(ntdisp, wp->iwp->mw);
		XLowerWindow(ntdisp, wp->iwp->mw);
		XUnmapWindow(ntdisp, wp->mw);
	  }
	  break;
	case 'o' :
	  if (wp != NULL & wp->mw != 0) {
		XMapWindow(ntdisp, wp->mw);
		XUnmapWindow(ntdisp, wp->iwp->mw);
		wp->map = IsViewable;
	  }
	  break;
	case ' ' :
	  if (ev->subwindow != 0)
		wp = lookwin(ev->subwindow);
	  else
		wp = NULL;
	  if (wp != NULL) {
		wp_select(kwt = wp);
		XSetInputFocus(ntdisp, wp->mw,
					   RevertToPointerRoot, CurrentTime);
	  }
	  break;
	case 'x' :
	  if (wp != NULL && wp->mw != 0 && wp->nw != 0)
		XWarpPointer(ntdisp, None, ntroot, 0, 0, 0, 0,
					 wp->x+wp->w/2, wp->y+wp->h/2);
	  break;
	case '0' :
	case '1' :
	case '2' :
	case '3' :
	case '4' :
	case '5' :
	case '6' :
	case '7' :
	case '8' :
	case '9' :
	  wp = wp_lookup(*buf);
	  if ((wp == NULL) || (wp->mw == 0) || (wp->nw == 0))
		return;
	  XGetInputFocus(ntdisp, &focus, &revert);
	  if (focus == wp->mw)
		XRaiseWindow(ntdisp, wp->mw);
	  else
		XMapWindow(ntdisp, wp->mw);
	  XUnmapWindow(ntdisp, wp->iwp->mw);
	  wp->map = IsViewable;
	  XSync(ntdisp,0);
	  wp_select(kwt = wp);
	  XSetInputFocus(ntdisp, wp->mw, RevertToPointerRoot, CurrentTime);
	  break;
	case 'h' :
	case 'j' :
	case 'k' :
	case 'l' :
	case 'p' :
	case 'n' :
	case 'f' :
	case 'b' :
	  if (! already_set_Pointer) {
		setPointer();
		already_set_Pointer = 1; 
	  }
	  switch (*buf) {
	  case 'n':
		Pointer_y += 3*step;
	  case 'j':
		Pointer_y += step;
		break;
	  case 'p':
		Pointer_y -= 3*step;
	  case 'k':
		Pointer_y -= step;
		break;
	  case 'f':
		Pointer_x += 3*step;
	  case 'l':
		Pointer_x += step;
		break;
	  case 'b':
		Pointer_x -= 3*step;
	  case 'h':
		Pointer_x -= step;
		break;
	  }

	  if (Pointer_x > ntroot_w)
		Pointer_x = ntroot_w;

	  if (Pointer_y > ntroot_h)
		Pointer_y = ntroot_h;

	  if (Pointer_x < 0)
		Pointer_x = 0;

	  if (Pointer_y < 0)
		Pointer_y = 0;

	  XWarpPointer(ntdisp, None, ntroot, 0, 0, 0, 0,
				   Pointer_x, Pointer_y);
	}
  }
}
/******************* end root key handler **********************/

int
setPointer(void)
{
  int			chilx, chily;
  int			winx, winy;
  unsigned int	button;
  Window		root, child;

  XQueryPointer(ntdisp, ntroot, &root, &child,
				&Pointer_x, &Pointer_y, &winx, &winy, &button);

  if (Pointer_x < 0)
	Pointer_x = 0;

  if (Pointer_y < 0)
	Pointer_y = 0;
}
