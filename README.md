# kwm
KWM Keyboard Window Manager 

------- What is kwm? -------
kwm is an window manager for X11.  See manual pages for
detail. This program wass originally made by Prof.Kuno at
Tsukuba University. (Please read the file "README.1988")

You can freely re-distribute this software.  See 'NOTICE'
about this issue. But ABSOLUTELY NO WARRANTY, as always.


------- Before you compile... -------
I didn't prepare any writeups which describe how to 
customize, compile and run this software. Because this
program is expected to be used by the people who can read
the codes by themselves:p

------- SYSTEM requirements -------
X11
  X11R4 or later (if you want to compile kwm with X11R5 or 
  before, some modification is needed but you can easily 
  find out the way by refering the man pages of X11 or 
  include file of X11.)

OS
  I've been using kwm with sunos4, solaris2, ultrix, HP-UX
  (9.xx & 10.xx), FreeBSD and linux. 


------- ABSOLUTELY NO WARRANTY -------
This program is not so stable compared to the other window
managers. ( Because I don't care if it works fine in my
environment. )
Actually, there are no boundary checks in many places.
Therefore, this version known to crash if you open many
windows. 
If you find bugs, please fix them and send me the patches:)
I'll include those patchs in the future releases ( if exists )


------- Another "VERY" important thing -------
If you want to use this window manger with PCs, you have
to check whether your keyboard can generate the correct
keycodes or not with xev or some other utilities when you
press more than 3 keys at the same time. As far as I know,
most of the cheap keyboards can not generate the correct
keycodes... But kwm seems to work fine with the most of 
the notebook PCs and workstations.

Good Luck and enjoy hacking,

  Yoshinari Hayato
