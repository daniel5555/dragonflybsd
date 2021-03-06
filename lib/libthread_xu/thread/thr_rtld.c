/*
 * Copyright (c) 2001 Alexander Kabaev
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in this position and unchanged.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD: src/lib/libpthread/thread/thr_rtld.c,v 1.5 2003/11/05 18:19:24 deischen Exp $
 * $DragonFly: src/lib/libthread_xu/thread/thr_rtld.c,v 1.2 2005/03/29 19:26:20 joerg Exp $
 */
#include <machine/tls.h>

#include <stdlib.h>
#include <pthread.h>

#include "rtld_lock.h"
#include "thr_private.h"

static int	_thr_rtld_clr_flag(int);
static void	*_thr_rtld_lock_create(void);
static void	_thr_rtld_lock_destroy(void *);
static void	_thr_rtld_lock_release(void *);
static void	_thr_rtld_rlock_acquire(void *);
static int	_thr_rtld_set_flag(int);
static void	_thr_rtld_wlock_acquire(void *);

static void *
_thr_rtld_lock_create(void)
{
	pthread_rwlock_t prwlock;
	if (_pthread_rwlock_init(&prwlock, NULL))
		return (NULL);
	return (prwlock);
}

static void
_thr_rtld_lock_destroy(void *lock)
{
	pthread_rwlock_t prwlock;

	prwlock = (pthread_rwlock_t)lock;
	if (prwlock != NULL)
		_pthread_rwlock_destroy(&prwlock);
}

static void
_thr_rtld_rlock_acquire(void *lock)
{
	pthread_rwlock_t prwlock;

	prwlock = (pthread_rwlock_t)lock;
	_thr_rwlock_rdlock(&prwlock);
}

static void
_thr_rtld_wlock_acquire(void *lock)
{
	pthread_rwlock_t prwlock;

	prwlock = (pthread_rwlock_t)lock;
	_thr_rwlock_wrlock(&prwlock);
}

static void
_thr_rtld_lock_release(void *lock)
{
	pthread_rwlock_t prwlock;

	prwlock = (pthread_rwlock_t)lock;
	_thr_rwlock_unlock(&prwlock);
}


static int
_thr_rtld_set_flag(int mask)
{
	struct pthread *curthread;
	int bits;

	curthread = tls_get_curthread();
	if (curthread != NULL) {
		bits = curthread->rtld_bits;
		curthread->rtld_bits |= mask;
	} else {
		bits = 0;
		PANIC("No current thread in rtld call");
	}

	return (bits);
}

static int
_thr_rtld_clr_flag(int mask)
{
	struct pthread *curthread;
	int bits;

	curthread = tls_get_curthread();
	if (curthread != NULL) {
		bits = curthread->rtld_bits;
		curthread->rtld_bits &= ~mask;
	} else {
		bits = 0;
		PANIC("No current thread in rtld call");
	}
	return (bits);
}

void
_thr_rtld_init(void)
{
	struct RtldLockInfo li;

	li.lock_create  = _thr_rtld_lock_create;
	li.lock_destroy = _thr_rtld_lock_destroy;
	li.rlock_acquire = _thr_rtld_rlock_acquire;
	li.wlock_acquire = _thr_rtld_wlock_acquire;
	li.lock_release  = _thr_rtld_lock_release;
	li.thread_set_flag = _thr_rtld_set_flag;
	li.thread_clr_flag = _thr_rtld_clr_flag;
	li.at_fork = NULL;
	_rtld_thread_init(&li);
}

void
_thr_rtld_fini(void)
{
	_rtld_thread_init(NULL);
}
