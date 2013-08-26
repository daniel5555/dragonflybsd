# Makefile for hammer2 vfs
#
#
.PATH: ${.CURDIR}

CFLAGS+= -DINVARIANTS -DSMP
KMOD=	hammer2
SRCS=	hammer2_vfsops.c hammer2_vnops.c hammer2_inode.c hammer2_ccms.c
SRCS+=	hammer2_chain.c hammer2_flush.c hammer2_freemap.c
SRCS+=	hammer2_ioctl.c hammer2_msgops.c hammer2_subr.c
SRCS+=  hammer2_lz4.c
SRCS+=  zlib/adler32.c zlib/deflate.c
SRCS+=  zlib/infback.c zlib/inffast.c zlib/inflate.c zlib/inftrees.c zlib/trees.c
SRCS+=  zlib/zutil.c

.include <bsd.kmod.mk>
