/*
 * Copyright 2008-2011 Uwe Pachler
 *
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation. This particular file is
 * subject to the "Classpath" exception as provided in the LICENSE file
 * that accompanied this code.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef _BSD_KEVENT_H
#define	_BSD_KEVENT_H

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>


/*
// BSD kqueue forward declarations
typedef unsigned int uintptr_t;
typedef int intptr_t;
#define EV_ERROR (0x0001)

struct kevent {
	 uintptr_t ident;        // identifier for this event
	 short     filter;       // filter for event
	 u_short   flags;        // action flags for kqueue
	 u_int     fflags;       // filter flag value
	 intptr_t  data;         // filter data value
	 void      *udata;       // opaque user data identifier
};
*/

/**
 * Creates a new kevent java object (note: this is a Java-side peer for the
 * BSD kevent kernel structure.
 * @return	the new kevent java object
 */
jobject BSD_Jkevent_create(JNIEnv* env);

/**
 * The function takes a kevent Java object and returns it's native BSD kevent
 * structure (note that both have the same name, however, one is a java class
 * while the other is the C kernel structure used in BSD, which is a peer to
 * the java class.). Note that the Java kevent owns it's peer.
 * @returns the value of the peer field cast to a kevent
 */
struct kevent* BSD_Jkevent_getPeer(JNIEnv* env, jobject jkevent);


#endif	/* _BSD_KEVENT_H */

