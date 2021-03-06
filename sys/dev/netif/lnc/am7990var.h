/*	$NetBSD: am7990var.h,v 1.23 2005/12/11 12:21:25 christos Exp $	*/
/*	$FreeBSD: src/sys/dev/le/am7990var.h,v 1.1 2006/01/31 14:48:58 marius Exp $	*/
/*	$DragonFly: src/sys/dev/netif/lnc/am7990var.h,v 1.1 2006/07/07 14:16:29 sephe Exp $	*/

/*-
 * Copyright (c) 1997, 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum and by Jason R. Thorpe of the Numerical Aerospace
 * Simulation Facility, NASA Ames Research Center.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the NetBSD
 *	Foundation, Inc. and its contributors.
 * 4. Neither the name of The NetBSD Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DEV_LE_AM7990VAR_H_
#define	_DEV_LE_AM7990VAR_H_

/*
 * Ethernet software status per device.
 *
 * NOTE: this structure MUST be the first element in machine-dependent
 * le_softc structures!  This is designed SPECIFICALLY to make it possible
 * to simply cast a "void *" to "struct le_softc *" or to
 * "struct am7990_softc *".  Among other things, this saves a lot of hair
 * in the interrupt handlers.
 */
struct am7990_softc {
	struct lance_softc lsc;
};

int	am7990_config(struct am7990_softc *, const char*, int);
void	am7990_detach(struct am7990_softc *);
void	am7990_intr(void *);

#endif /* !_DEV_LE_AM7990VAR_H_ */
