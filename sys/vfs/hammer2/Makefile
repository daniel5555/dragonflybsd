# Makefile for hammer2 vfs
#
#
.PATH: ${.CURDIR} ${.CURDIR/zlib/}

CFLAGS+= -DINVARIANTS -DSMP
KMOD=	hammer2
SRCS=	hammer2_vfsops.c hammer2_vnops.c hammer2_inode.c hammer2_ccms.c
SRCS+=	hammer2_chain.c hammer2_flush.c hammer2_freemap.c
SRCS+=	hammer2_ioctl.c hammer2_msgops.c hammer2_subr.c
SRCS+=  hammer2_lz4.c
SRCS+=  zlib/hammer2_zlib_adler32.c zlib/hammer2_zlib_deflate.c
SRCS+=  zlib/hammer2_zlib_inffast.c zlib/hammer2_zlib_inflate.c
SRCS+=  zlib/hammer2_zlib_inftrees.c zlib/hammer2_zlib_trees.c
SRCS+=  zlib/hammer2_zlib_zutil.c

.include <bsd.kmod.mk>
