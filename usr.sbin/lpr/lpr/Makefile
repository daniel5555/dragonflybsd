#	From: @(#)Makefile	8.1 (Berkeley) 6/6/93
# $FreeBSD: src/usr.sbin/lpr/lpr/Makefile,v 1.8.2.4 2002/06/30 03:59:16 gad Exp $
# $DragonFly: src/usr.sbin/lpr/lpr/Makefile,v 1.2 2003/06/17 04:29:56 dillon Exp $

.PATH: ${.CURDIR}/../common_source

BINDIR=	/usr/bin

PROG=	lpr
MAN=	lpr.1 printcap.5
BINOWN=	root
BINGRP=	daemon
BINMODE= 6555

CFLAGS+= -I${.CURDIR}/../common_source

DPADD=	${LIBLPR}
LDADD=	${LIBLPR}

.include <bsd.prog.mk>
