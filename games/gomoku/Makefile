#	@(#)Makefile	8.1 (Berkeley) 7/24/94

PROG=	gomoku
SRCS=	bdinit.c bdisp.c main.c makemove.c pickmove.c stoc.c
MAN6=	gomoku.0
DPADD=	${LIBOCURSES} ${LIBTERM}
LDADD=	-locurses -ltermlib
HIDEGAME=hidegame
CFLAGS+=-DOLD_CURSES

.include <bsd.prog.mk>
