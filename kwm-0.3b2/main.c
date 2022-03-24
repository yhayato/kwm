#include	<signal.h>
#include "kwm.h"

/******************** main routine **************************/
extern	void	emerg_exit() ;

int             dbg;

static	char ** restart_arg;

void 
ignore(ev, cd, bd)
	XEvent         *ev;
	ClientData      cd, bd;
{;
}

static 
handler(dpy, ev)
	Display        *dpy;
	XErrorEvent    *ev;
{
}

void
restart()
{
	execvp(restart_arg[0], restart_arg);
}

main(argc, argv)
	int		argc;
	char ** argv;
{
	Bool have_rr;
	int rr_major, rr_minor, rr_mask, rr_event_base, rr_error_base;
	
	restart_arg = argv ;

	close(0);
	close(1);

	if (signal(SIGBUS, SIG_IGN) != SIG_IGN)
		signal(SIGBUS, emerg_exit) ;

	if (signal(SIGFPE, SIG_IGN) != SIG_IGN)
		signal(SIGFPE, emerg_exit) ;


	if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
		signal(SIGHUP, restart) ;

	if (signal(SIGIOT, SIG_IGN) != SIG_IGN)
		signal(SIGIOT, emerg_exit) ;

	if (signal(SIGILL, SIG_IGN) != SIG_IGN)
		signal(SIGILL, emerg_exit) ;

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, emerg_exit) ;

	ntinit();
	XSetErrorHandler(handler);
	/*	dbg = 10; */
	ntaddcb(CreateNotify, ntroot, rt_create, NULL, NULL);
	ntaddcb(DestroyNotify, ntroot, rt_destroy, NULL, NULL);
	ntaddcb(MapRequest, ntroot, rt_map, NULL, NULL);
	ntaddcb(MapNotify, ntroot, rt_mapped, NULL, NULL);
	ntaddcb(ConfigureRequest, ntroot, rt_config, NULL, NULL);
	ntaddcb(ConfigureNotify, ntroot, rt_configed, NULL, NULL);
	ntaddcb(ResizeRequest, ntroot, rt_resize, NULL, NULL);
	ntaddcb(UnmapNotify, ntroot, rt_unmap, NULL, NULL);
	ntaddcb(ButtonPress, ntroot, rt_press, NULL, NULL);
	ntaddcb(ButtonRelease, ntroot, rt_release, NULL, NULL);
	ntaddcb(EnterNotify, ntroot, rt_enter, NULL, NULL);
	ntaddcb(LeaveNotify, ntroot, rt_enter, NULL, NULL);
	ntaddcb(MotionNotify, ntroot, rt_move, NULL, NULL);
	XSelectInput(ntdisp, ntroot,
		  PointerMotionMask
		| LeaveWindowMask
		| EnterWindowMask
		| SubstructureRedirectMask
		| SubstructureNotifyMask);
	XGrabButton(ntdisp, AnyButton, ShiftMask | ControlMask , ntroot, False,
		    ButtonPressMask | ButtonReleaseMask | ButtonMotionMask,
		    GrabModeAsync, GrabModeAsync, None, 0);
	ntaddcb(KeyPress, ntroot, rt_key, NULL, NULL);
	ntaddcb(KeyRelease, ntroot, ignore, NULL, NULL);

	have_rr = XRRQueryExtension(ntdisp, &rr_event_base, &rr_error_base);
	if (have_rr){
	  XRRQueryVersion(ntdisp, &rr_major, &rr_minor);
	  rr_mask = ( RRScreenChangeNotifyMask |
				  RRCrtcChangeNotifyMask |
				  RROutputChangeNotifyMask |
				  RROutputPropertyNotifyMask);
	  if ((rr_major == 1)&&(rr_minor <= 1)){
		rr_mask &= ~(RRCrtcChangeNotifyMask |
					 RROutputChangeNotifyMask |
					 RROutputPropertyNotifyMask);
	  }
	  XRRSelectInput(ntdisp,ntroot,rr_mask);
	}	  

	ntaddcb(rr_event_base + RRNotify, ntroot, rt_screenresize, NULL, NULL);
	XGrabKey(ntdisp, AnyKey, ShiftMask | ControlMask, ntroot, False,
		 GrabModeAsync, GrabModeAsync);
	kw_init();
	get1stinfo();
	ntloop();
}

/******************** end main routine **********************/
