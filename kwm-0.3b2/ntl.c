::::::::
ntloop.c
::::::::
#include "nt.h"

typedef struct _cbent {
	Window          w;
	int             t;
	void            (*f) ();
	ClientData      cd, bd;
}               CbEnt;

CbEnt         **ce;
int             ces, ceu;

#define hash(t,w,s) ((t*(int)w)%s)

int 
ntaddcb(t, w, f, cd, bd)
	int             t;
	Window          w;
	void            (*f) ();
ClientData      cd, bd; {
	register int    i, k;
	register CbEnt *ent;
	if (ce == NULL) {
		ce = (CbEnt **) malloc(sizeof(CbEnt *) * 997);
		if ( ce == NULL )
			emerg_exit() ;
		ces = 997;
		ceu = 0;
		for (i = 0; i < 997; ++i)
			ce[i] = NULL;
	}
	k = hash(t, w, ces);
	while (ce[k] != NULL)
		if (ce[k]->t == t && ce[k]->w == w)
			break;
		else if (++k >= ces)
			k = 0;
	if ((ent = ce[k]) == NULL) {
		if (++ceu >= ces)
		  fprintf(stderr,"%s : handler table overflow.\n",
				  __PRETTY_FUNCTION__);
		ce[k] = ent = (CbEnt *) malloc(sizeof(CbEnt));
		if ( ce[k] == NULL )
			emerg_exit() ;
	}
	ent->f = f;
	ent->t = t;
	ent->w = w;
	ent->cd = cd;
	ent->bd = bd;
}

int 
ntloop()
{
	extern Display *ntdisp;
	XEvent          ev;
	register XAnyEvent *av;
	register int    i, k;
	register CbEnt *ent;
	while (TRUE) {
		XNextEvent(ntdisp, &ev);
		av = (XAnyEvent *) & ev;
		k = hash(av->type, av->window, ces);
		while ((ent = ce[k]) != NULL)
			if (ent->t == av->type && ent->w == av->window) {
				if (*ent->f)
					(*ent->f) (&ev, ent->cd, ent->bd);
				break;
			} else if (++k >= ces)
				k = 0;
		if (ent == NULL)
			printf("event discarded, type=%d, win=%x\n", av->type, av->window);
	}
}
