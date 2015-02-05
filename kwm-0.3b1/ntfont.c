#include "nt.h"

typedef struct fent {
	char           *name;
	Font            font;
	XFontStruct    *info;
}               fent_r, *fent;

fent_r          ft[10];
static int      ftn = 0;

extern Display *ntdisp;

static void 
load(s, t)
	char           *s, *t;
{
	ft[ftn].name = s;
	ft[ftn].font = XLoadFont(ntdisp, t);
	ft[ftn].info = XQueryFont(ntdisp, ft[ftn].font);
	++ftn;
}

void 
ntftinit()
{
	load("fix", "variable");
	load("icon", "variable");
	load("norm", "variable");
	load("large", "12x24");
}

Font 
ntftget(n, info)
	char           *n;
	XFontStruct   **info;
{
	register int    i;
	for (i = 0; i < ftn; ++i)
		if (strcmp(ft[i].name, n) == 0) {
			if (info)
				*info = ft[i].info;
			return ft[i].font;
		}
	if (info)
		*info = ft[0].info;
	return ft[0].font;
}
