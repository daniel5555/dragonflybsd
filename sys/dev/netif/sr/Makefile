# $FreeBSD: src/sys/modules/sr/Makefile,v 1.2.2.1 2002/06/17 15:11:05 jhay Exp $
# $DragonFly: src/sys/dev/netif/sr/Makefile,v 1.3 2005/07/28 00:06:10 joerg Exp $
 
KMOD    = if_sr
SRCS    = if_sr.c if_sr_pci.c
SRCS    += device_if.h bus_if.h pci_if.h isa_if.h opt_netgraph.h
  
.include <bsd.kmod.mk>
