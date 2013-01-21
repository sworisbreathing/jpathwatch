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

#include "name_pachler_nio_file_impl_BSD.h"

#include "BSD_kevent.h"

#include "Unix.h"
#include "Unix_timespec.h"
#include "nativelib.h"

#include <algorithm>
#include <errno.h>
/*
extern "C" int kqueue(void);

extern "C" int kevent(int kq, int nchanges, struct kevent **changelist, int nevents,
             struct kevent *eventlist, struct timespec *timeout);
*/


static Unix_IntDefine bsdIntDefines[] = {
	// actions
	UNIX_INTDEFINE(EV_ADD),
	UNIX_INTDEFINE(EV_DELETE),
	UNIX_INTDEFINE(EV_ENABLE),
	UNIX_INTDEFINE(EV_DISABLE),
	UNIX_INTDEFINE(EV_ONESHOT),
	UNIX_INTDEFINE(EV_CLEAR),
	UNIX_INTDEFINE(EV_EOF),
	UNIX_INTDEFINE(EV_ERROR),

	// filters
	UNIX_INTDEFINE(EVFILT_READ),
	UNIX_INTDEFINE(EVFILT_WRITE),
	UNIX_INTDEFINE(EVFILT_AIO),
	UNIX_INTDEFINE(EVFILT_VNODE),
	UNIX_INTDEFINE(EVFILT_PROC),
	UNIX_INTDEFINE(EVFILT_SIGNAL),
	UNIX_INTDEFINE(EVFILT_TIMER),
// OSX doesn't have it :(
//	UNIX_INTDEFINE(EVFILT_NETDEV),

	// for EVFILT_VNODE
	UNIX_INTDEFINE(NOTE_DELETE),
	UNIX_INTDEFINE(NOTE_WRITE),
	UNIX_INTDEFINE(NOTE_EXTEND),
	UNIX_INTDEFINE(NOTE_ATTRIB),
	UNIX_INTDEFINE(NOTE_LINK),
	UNIX_INTDEFINE(NOTE_RENAME),
	UNIX_INTDEFINE(NOTE_REVOKE),

	// for EVFILT_PROC
	UNIX_INTDEFINE(NOTE_EXIT),
	UNIX_INTDEFINE(NOTE_FORK),
	UNIX_INTDEFINE(NOTE_EXEC),
	UNIX_INTDEFINE(NOTE_TRACK),
	UNIX_INTDEFINE(NOTE_TRACKERR),

	// for EVFILT_NETDEV
// OSX doesn't have it :(
//	UNIX_INTDEFINE(NOTE_LINKUP),
//	UNIX_INTDEFINE(NOTE_LINKDOWN),
//	UNIX_INTDEFINE(NOTE_LINKINV),
};
static const size_t bsdIntDefinesSize = sizeof(bsdIntDefines)/sizeof(*bsdIntDefines);

static bool bsdIntDefinesInitialized = Unix_addIntDefineList(bsdIntDefines, bsdIntDefinesSize);

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_BSD_kqueue
  (JNIEnv* env, jclass)
{
	int result = kqueue();
	Unix_cacheErrno();
	return result;
}




template <size_t N>
void fillKeventArray(JNIEnv* env, struct kevent(&eventArray)[N], jobjectArray jkeventArray, size_t off, size_t len)
{
	for(size_t n=off; n<std::min(off+len,N); ++n){
		jobject obj = env->GetObjectArrayElement(jkeventArray, n);
		if(obj==0)
		{
			nativelib_throwNullPointerException(env);
			return;
		}
		struct kevent* ke = BSD_Jkevent_getPeer(env, obj);
		eventArray[n] = *ke;
		env->DeleteLocalRef(obj);
	}
}


JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_BSD_kevent
  (JNIEnv* env, jclass, jint kq, jobjectArray changelist, jobjectArray eventlist, jobject timeout)
{
	const size_t lchangelistCapacity = 32;
	size_t lchangelistSize = 0;
	struct kevent lchangelist[lchangelistCapacity];

	size_t eventlistLength = eventlist!=0 ? env->GetArrayLength(eventlist) : 0;
	size_t changelistLength = changelist!=0 ? env->GetArrayLength(changelist) : 0;

	size_t changelistOff = 0;
	// if there are many changes to apply (more than lchangelistCapacity),
	// only apply lchangelistCapacity changes at a time by filling it
	// up entirely and calling kevent
	while(changelistLength - changelistOff > lchangelistCapacity){
		fillKeventArray(env, lchangelist, changelist, changelistOff, lchangelistCapacity);
		if(env->ExceptionCheck())
			return -1;
		lchangelistSize = lchangelistCapacity;
		timespec ts = {};
		int result = kevent(kq, lchangelist, lchangelistSize, 0, NULL, &ts);

		// if there is an error, emulate kevent()'s error behaviour
		// (we haven't supplied an eventlist to kevent(), so we have to fill
		// the event list ourselves if one was provided to this function.
		// Otherwise, simply cache errno and return -1.
		if(result == -1){
			if(eventlistLength > 0){
				jobject jkevent = BSD_Jkevent_create(env);
				struct kevent* ke = BSD_Jkevent_getPeer(env, jkevent);
				ke->flags = EV_ERROR;
				ke->data = errno;
				env->SetObjectArrayElement(eventlist, 1, jkevent);
				env->DeleteLocalRef(jkevent);
				return 1;
			} else {
				Unix_cacheErrno();
				return -1;
			}
		}
		changelistOff += lchangelistSize;
	}

	// fill remaining change elements into local changelist and call
	// kevent 'normally'.
	lchangelistSize = changelistLength-changelistOff;
	fillKeventArray(env, lchangelist, changelist, changelistOff, lchangelistSize);
	if(env->ExceptionCheck())
		return -1;

	// we only support receiving leventlistCapacity events at a time.
	const size_t leventlistCapacity = 32;
	struct kevent leventlist[leventlistCapacity];
	int leventlistMaxSize = std::min(leventlistCapacity, eventlistLength);

	timespec* ts = 0;
	if(timeout != 0)
		ts = Unix_Jtimespec_getPeer(env, timeout);

	// call kevent. This will apply the (remaining) changes and read events
	// from the event queue.
	int result = kevent(kq, lchangelist, lchangelistSize, leventlist, leventlistMaxSize, ts);

	Unix_cacheErrno();

	// if the result is positive, this loop will transform the C kevent structs
	// into Java kevent instances (they'll be generated in the target array
	// in case they don't exist)
	for(int i=0; i<result; ++i)
	{
		jobject jkevent = env->GetObjectArrayElement(eventlist, i);
		bool keventExists = jkevent != 0;
		if(!keventExists)
		{
			jkevent = BSD_Jkevent_create(env);
			if(env->ExceptionCheck())
				return -1;
			env->SetObjectArrayElement(eventlist, i, jkevent);
		}

		struct kevent* k = BSD_Jkevent_getPeer(env, jkevent);
		*k = leventlist[i];

		if(!keventExists)
			env->SetObjectArrayElement(eventlist, i, jkevent);

		env->DeleteLocalRef(jkevent);
 
	}

	return result;
}

