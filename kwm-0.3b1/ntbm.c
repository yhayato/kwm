#include "nt.h"
#include <X11/Xutil.h>

typedef struct bent {
	char           *name;
	Pixmap          pix;
	int             w, h, hx, hy;
}               bent_r, *bent;

bent_r          bm[10];
static int      bmn = 0;

extern Display *ntdisp;
extern Window   ntroot;

static char    *bms[] = {"wins", 0};

void 
ntbminit()
{
	int             stat;
	char            f[100], **p = bms;
	while (*p) {
		bent            b = &bm[bmn];
		b->name = *p;
		sprintf(f, "/ua/kuno/lib/bm/%s.bm", *p);
		stat = XReadBitmapFile(ntdisp, ntroot, f,
				     &b->w, &b->h, &b->pix, &b->hx, &b->hy);
		if (stat == BitmapSuccess)
			++bmn;
		else
			printf("%s: readbitmap failed.\n", f);
		++p;
	}
}

Pixmap 
ntbmget(n, w, h)
	int            *w, *h;
{
	register int    i;
	for (i = 0; i < bmn; ++i)
		if (strcmp(bm[i].name, n) == 0) {
			if (w)
				*w = bm[i].w;
			if (h)
				*h = bm[i].h;
			return bm[i].pix;
		}
	if (w)
		*w = bm[0].w;
	if (h)
		*h = bm[0].h;
	return bm[0].pix;
}
