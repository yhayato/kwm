CC  = cc
OBJ = icon.o kwin.o main.o root.o winid.o
HDR = kwm.h nt.h
LIBOBJS = ntinit.o ntutil.o ntloop.o ntbtn.o ntbm.o ntfont.o ntlwin.o

kwm: nt.a ${OBJ}
	${CC} -g -o kwm ${OBJ} nt.a -L/usr/X11R6/lib64 -lX11 

nt.a: ${LIBOBJS}
	ar cru nt.a ${LIBOBJS}
	ranlib nt.a

.c.o:
	${CC} -I/usr/X11R6/include -I./ -O0 -g -c $*.c

clean:
	rm -f kwm nt.a ${OBJ} ${LIBOBJS}

