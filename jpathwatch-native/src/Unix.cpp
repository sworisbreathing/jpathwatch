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

#include "Unix.h"
#include "JArgument.h"
#include "nativelib.h"

#include "name_pachler_nio_file_impl_Unix.h"

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#if defined __linux__
	#include <sys/vfs.h>
#elif defined __FreeBSD__ || defined __APPLE__
	#include <sys/param.h>
	#include <sys/mount.h>
#endif

#include <jni.h>

#include <algorithm>
#include <cassert>
#include <string>

#include <string.h>
#include <errno.h>

#ifdef __APPLE__
#include <pthread.h>
#endif

enum StatType
{
	StatType_stat,
	StatType_lstat,
};

static jint stat_impl(StatType statType, JNIEnv* env, jclass clazz, jstring jpath, jobject jstat);

static Unix_IntDefine unixIntDefines[] = {
	// errno values
	UNIX_INTDEFINE(EBADF),
	UNIX_INTDEFINE(EINVAL),
	UNIX_INTDEFINE(EINTR),
	UNIX_INTDEFINE(ENOENT),

	// open() flags - flags specific unix flavours should
	// be declared in their respective wrapper implementations
	UNIX_INTDEFINE(O_RDONLY),
	UNIX_INTDEFINE(O_WRONLY),
	UNIX_INTDEFINE(O_RDWR),
	UNIX_INTDEFINE(O_APPEND),
	UNIX_INTDEFINE(O_CREAT),
	UNIX_INTDEFINE(O_EXCL),
	UNIX_INTDEFINE(O_NOCTTY),
	UNIX_INTDEFINE(O_NONBLOCK),
	UNIX_INTDEFINE(O_SYNC),
	UNIX_INTDEFINE(O_TRUNC),

	// mode_t masks
	UNIX_INTDEFINE(S_ISUID),	// set UID bit
	UNIX_INTDEFINE(S_ISGID),	// set-group-ID bit
	UNIX_INTDEFINE(S_IRWXU),	// mask for file owner permissions
	UNIX_INTDEFINE(S_IRUSR),	// owner has read permission
	UNIX_INTDEFINE(S_IWUSR),	// owner has write permission
	UNIX_INTDEFINE(S_IXUSR),	// owner has execute permission
	UNIX_INTDEFINE(S_IRWXG),	// mask for group permissions
	UNIX_INTDEFINE(S_IRGRP),	// group has read permission
	UNIX_INTDEFINE(S_IWGRP),	// group has write permission
	UNIX_INTDEFINE(S_IXGRP),	// group has execute permission
	UNIX_INTDEFINE(S_IRWXO),	// mask for permissions for others (not in group)
	UNIX_INTDEFINE(S_IROTH),	// others have read permission
	UNIX_INTDEFINE(S_IWOTH),	// others have write permission
	UNIX_INTDEFINE(S_IXOTH),	// others have execute permission
};
static const size_t unixIntDefinesSize = sizeof(unixIntDefines)/sizeof(*unixIntDefines);

struct Unix_IntDefineSequence
{
	const Unix_IntDefine* begin;
	const Unix_IntDefine* end;
};

static Unix_IntDefineSequence intDefineSequenceSet[8] = {};
static const size_t intDefineSequenceSetCapacity = sizeof(intDefineSequenceSet)/sizeof(*intDefineSequenceSet);
static size_t intDefineSequenceSetSize = 0;

static bool Unix_IntDefine_less(const Unix_IntDefine& lhs, const Unix_IntDefine& rhs)
{
    return strcmp(lhs.name, rhs.name) < 0;
}


bool Unix_addIntDefineList(Unix_IntDefine* intDefineList, size_t intDefineListSize)
{
    // there's not much we can do; we'll simply not add the list in case
    // we're out of space - but this should never really happen; it would mean
    // that we have subclassed the Unix wrapper too many times, or something
    // else went really wrong...
    if(intDefineSequenceSetSize >= intDefineSequenceSetCapacity)
        return false;

    // make sure list is sorted so we can use binary_search()
	Unix_IntDefine* begin = intDefineList;
	Unix_IntDefine* end = intDefineList + intDefineListSize;

    std::sort(begin, end, Unix_IntDefine_less);

	Unix_IntDefineSequence seq = {};
	seq.begin = begin;
	seq.end = end;

    intDefineSequenceSet[intDefineSequenceSetSize++] = seq;

	return true;
}


JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_getIntDefine
  (JNIEnv* env, jclass, jstring defineNameString)
{
	if(defineNameString == 0)
	{
		nativelib_throwNullPointerException(env);
		fprintf(stderr, "string given to Unix.getIntDefine() is null\n");
		return -1;
	}

    int defineNameStringLength = env->GetStringLength(defineNameString);
    int defineNameStringUTFLength = env->GetStringUTFLength(defineNameString);

    char defineName[128] = {0};
    size_t defineNameCapacity = sizeof(defineName)/sizeof(*defineName) - 1;

    const Unix_IntDefine* fnd = 0;

    if(defineNameStringUTFLength < int(defineNameCapacity))
    {
        env->GetStringUTFRegion(defineNameString, 0, defineNameStringLength, defineName);
        for(size_t i=0; i<intDefineSequenceSetSize; ++i)
        {
			Unix_IntDefine v = {};
			v.name = defineName;
			const Unix_IntDefineSequence& seq = intDefineSequenceSet[i];
            const Unix_IntDefine* lb = std::lower_bound(seq.begin, seq.end, v, Unix_IntDefine_less);
			if(lb != seq.end && strcmp(lb->name, v.name)==0)
			{
				// found it!
				fnd = lb;
				break;
			}
        }
    }

	if(fnd != 0)
	{
		return fnd->value;
	}
	else
	{
		char msg[256] = {};
		const size_t msgCapacity = sizeof(msg)/sizeof(*msg)-1;
		snprintf(msg, msgCapacity, "the given string '%s' is not a define name recognized by the native implementation", defineName);
		fprintf(stderr, "%s\n", msg);
		nativelib_throwIllegalArgumentException(env, msg);
		return -1;
	}
}

// We cache errno in TLS storage, because once the native function call
// returns to the calling Java code, the JVM might have made a unix OS call
// that changed errno. So every unix call wrapper calls Unix_cacheErrno right
// after calling the OS routine to store errno in cachedErrno.
// the Unix.errno() native method implementation then returns this cached errno
// value.
#ifdef __APPLE__
struct TLSData{
	int cachedErrno;
};
static pthread_key_t tlskey = {0};
static void tlskeyDestructor(void* tlskeyValue){
	TLSData* tlsdata = reinterpret_cast<TLSData*>(tlskeyValue);
	delete tlsdata;
	pthread_setspecific(tlskey, 0);
}

TLSData* getTLSData()
{
	TLSData* tlsdata = reinterpret_cast<TLSData*>(pthread_getspecific(tlskey));
	if(tlsdata == 0)
	{
		tlsdata = new TLSData();
		pthread_setspecific(tlskey, tlsdata);
	}
	return tlsdata;
}
#else
static int __thread cachedErrno;
#endif	// __APPLE__


void Unix_cacheErrno()
{
#ifdef __APPLE__
	TLSData* tlsdata = getTLSData();
	tlsdata->cachedErrno = errno;
#else
	cachedErrno = errno;
#endif
}


void Unix_init()
{
	Unix_addIntDefineList(unixIntDefines, unixIntDefinesSize);

#ifdef __APPLE__
	int result = pthread_key_create(&tlskey, tlskeyDestructor);
	assert(result == 0);
#endif	// __APPLE__
}


void Unix_shutdown()
{
#ifdef __APPLE__
	int result = pthread_key_delete(tlskey);
	assert(result == 0);
#endif
}



JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_open
  (JNIEnv* env, jclass, jstring jpathname, jint flags, jint mode)
{
	const char* pathname = env->GetStringUTFChars(jpathname, 0);
	if(pathname == 0)
		return -1;	// java throws an out of memory error in this case
	int result = open(pathname, flags, mode);
	Unix_cacheErrno();
	env->ReleaseStringUTFChars(jpathname, pathname);
	return result;
}


jint JNICALL Java_name_pachler_nio_file_impl_Unix_close
  (JNIEnv *, jclass, jint fd)
{
	return close(fd);
	Unix_cacheErrno();
}


static fd_set* jintArrayToFD_SET(jint* fdArrayp, jsize fdArraySize, fd_set* fdset, int* nfds, bool* validReturn)
{
	if(validReturn != 0)
		*validReturn = true;	// assume valid
	if(fdArraySize == 0)
		return 0;
	for(jsize n=0; n<fdArraySize; ++n)
	{
		int fd = fdArrayp[n];
		if(fd < 0 || fd >= FD_SETSIZE)
		{
			if(validReturn != 0)
				*validReturn = false;
			continue;
		}
		*nfds = std::max(*nfds, fd+1);
		FD_SET(fd, fdset);
	}
	return fdset;
}

static void fd_setToJintArrayBuffer(const fd_set* fdset, jint* buffer, jsize bufferSize)
{
	for(int i=0; i<bufferSize; ++i)
	{
		int fd = buffer[i];

		// need the const_cast<> here because OSX's FD_ISSET() wants a non-const fd_set* *sigh*
                if(!FD_ISSET(fd, const_cast<fd_set*>(fdset)))
			buffer[i] = -1;
	}
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_select
  (JNIEnv * env, jclass, jintArray readfdArray, jintArray writefdArray, jintArray exceptfdArray, jlong timeout)
{
	assert(sizeof(jint)==sizeof(int));
	fd_set readfds = {};
	fd_set writefds = {};
	fd_set exceptfds = {};

	int nfds = 0;

	jint* readfdArrayp = 0;
	jint* writefdArrayp = 0;
	jint* exceptfdArrayp = 0;
	fd_set* readfdsp = 0;
	fd_set* writedsp = 0;
	fd_set* exceptfdsp = 0;
	bool readfdsValid = true;
	bool writefdsValid = true;
	bool exceptfdsValid = true;
	if(readfdArray != 0)
	{
		readfdArrayp = env->GetIntArrayElements(readfdArray, 0);
		readfdsp = jintArrayToFD_SET(readfdArrayp, env->GetArrayLength(readfdArray), &readfds, &nfds, &readfdsValid);
	}

	if(writefdArray != 0)
	{
		writefdArrayp =env->GetIntArrayElements(writefdArray, 0);
		writedsp = jintArrayToFD_SET(writefdArrayp, env->GetArrayLength(writefdArray), &writefds, &nfds, &writefdsValid);
	}

	if(exceptfdArray != 0)
	{
		exceptfdArrayp = env->GetIntArrayElements(exceptfdArray, 0);
		exceptfdsp = jintArrayToFD_SET(exceptfdArrayp, env->GetArrayLength(exceptfdArray), &exceptfds, &nfds, &exceptfdsValid);
	}

	// that's our own doing: if an FD is invalid, return EINVAL (unix doesn't
	// check this, but would normally crash if you call FD_SET() with an
	// invalid file descriptor that is out of the range [0,FD_SETSIZE[
	if(!readfdsValid || !writefdsValid || !exceptfdsValid)
	{
		errno = EINVAL;
		Unix_cacheErrno();
		return -1;
	}

	timeval* timeoutstructp = 0;
	timeval timevalstruct = {0};
	if(timeout >= 0){
		timeoutstructp = &timevalstruct;
		timevalstruct.tv_sec = timeout / 1000;
		timevalstruct.tv_usec = (timeout-1000*timevalstruct.tv_sec) * 1000;
	}

	int result = select(nfds, readfdsp, writedsp, exceptfdsp, timeoutstructp);
	Unix_cacheErrno();

	bool success = result != -1;
	jint mode = success ? 0 : JNI_ABORT;	// on success, we copy the array contents back, otherwise we discard it
	if(readfdArrayp != 0)
	{
		if(success)
			fd_setToJintArrayBuffer(&readfds, readfdArrayp, env->GetArrayLength(readfdArray));
		env->ReleaseIntArrayElements(readfdArray, readfdArrayp, mode);
	}
	if(writefdArrayp != 0)
	{
		if(success)
			fd_setToJintArrayBuffer(&writefds, writefdArrayp, env->GetArrayLength(writefdArray));
		env->ReleaseIntArrayElements(writefdArray, writefdArrayp, mode);
	}
	if(exceptfdArrayp != 0)
	{
		if(success)
			fd_setToJintArrayBuffer(&exceptfds, exceptfdArrayp, env->GetArrayLength(exceptfdArray));
		env->ReleaseIntArrayElements(exceptfdArray, exceptfdArrayp, mode);
	}

	return result;
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_ioctl_1FIONREAD
  (JNIEnv *, jclass, jint fd)
{
	// ioctl(fd, FIONREAD,..) returns the number of bytes available for reading
	// without causing read() to block
	int available = 0;
	int result = ioctl(fd, FIONREAD, &available);
	Unix_cacheErrno();
	if(result == -1)
		return result;
	else
		return available;
}


JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_read
  (JNIEnv* env, jclass, jint fd, jbyteArray buffer, jint nbytes)
{
	nbytes = std::min(nbytes, env->GetArrayLength(buffer));
	jbyte* bufferp = env->GetByteArrayElements(buffer, 0);
	jint result = read(fd, bufferp, nbytes);
	Unix_cacheErrno();
	env->ReleaseByteArrayElements(buffer, bufferp, 0);

	return result;
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_write
  (JNIEnv* env, jclass, jint fd, jbyteArray buffer, jint nbytes)
{
	nbytes = std::min(nbytes, env->GetArrayLength(buffer));
	jbyte* bufferp = env->GetByteArrayElements(buffer, 0);
	jint result = write(fd, bufferp, nbytes);
	Unix_cacheErrno();
	env->ReleaseByteArrayElements(buffer, bufferp, JNI_ABORT);

	return result;
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_pipe
  (JNIEnv * env, jclass, jintArray fdarray)
{
	assert(sizeof(jint)==sizeof(int));
	if(env->GetArrayLength(fdarray)<2)
	{
		jclass exceptionClass = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
		if(exceptionClass == 0)
			return -1;
		env->ThrowNew(exceptionClass, "array passed into pipe() must have two or more elements");
	}

	jint* fdarrayp = env->GetIntArrayElements(fdarray, 0);

	jint result = pipe(reinterpret_cast<int*>(fdarrayp));
	Unix_cacheErrno();

	env->ReleaseIntArrayElements(fdarray, fdarrayp, 0);

	return result;
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_errno
  (JNIEnv *, jclass)
{
#ifdef __APPLE__
	TLSData* tlsdata = getTLSData();
	return tlsdata->cachedErrno;
#else
	return cachedErrno;
#endif
}

JNIEXPORT jstring JNICALL Java_name_pachler_nio_file_impl_Unix_strerror
  (JNIEnv* env, jclass, jint error)
{
	const char* msg = strerror(error);
	Unix_cacheErrno();
	jstring s = env->NewStringUTF(msg);
	return s;
}


JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_symlink
  (JNIEnv* env, jclass, jstring jlinkTo, jstring jlinkFrom)
{
	if(jlinkTo == 0 || jlinkFrom == 0)
		nativelib_throwNullPointerException(env, "link path parameters cannot be null");

	const char* linkTo = env->GetStringUTFChars(jlinkTo, 0);
	const char* linkFrom = env->GetStringUTFChars(jlinkFrom, 0);

	int result = symlink(linkTo, linkFrom);

	Unix_cacheErrno();

	env->ReleaseStringUTFChars(jlinkTo, linkTo);
	env->ReleaseStringUTFChars(jlinkFrom, linkFrom);

	return result;
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_lstat
  (JNIEnv* env, jclass clazz, jstring jpath, jobject jstat)
{
	return stat_impl(StatType_lstat, env, clazz, jpath, jstat);
}

JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Unix_stat
  (JNIEnv* env, jclass clazz, jstring jpath, jobject jstat)
{
	return stat_impl(StatType_stat, env, clazz, jpath, jstat);
}

static jint stat_impl(StatType statType, JNIEnv* env, jclass clazz, jstring jpath, jobject jstat)
{
	if(jpath==0 || jstat==0)	// early exit if a parameter is null
	{
		nativelib_throwNullPointerException(env, "one of the parameters is null");
		return -1;
	}

	struct stat st = {0};
	const char* path = env->GetStringUTFChars(jpath, 0);

	int result = -1;
	switch(statType){
		case StatType_stat:
			result = stat(path, &st);
			break;
		case StatType_lstat:
			result = lstat(path, &st);
			break;
		default:
			assert(false && "unimplemented stat type");
			break;
	}

	Unix_cacheErrno();

	env->ReleaseStringUTFChars(jpath, path);

	if(result != -1)
	{
		static jmethodID setMethodID = 0;

		JArgument arguments[] = {
			JArgument(st.st_dev,     'I'),
			JArgument(st.st_ino,     'J'),
			JArgument(st.st_mode,    'I'),
			JArgument(st.st_nlink,   'I'),
			JArgument(st.st_uid,     'I'),
			JArgument(st.st_gid,     'I'),
			JArgument(st.st_rdev,    'I'),
			JArgument(st.st_size,    'J'),
			JArgument(st.st_blksize, 'I'),
			JArgument(st.st_blocks,  'J'),
			JArgument(st.st_atime,   'J'),
			JArgument(st.st_mtime,   'J'),
			JArgument(st.st_ctime,   'J'),
		};

		if(setMethodID==0)
		{
			setMethodID = JArgument::getMethodID(env, "name/pachler/nio/file/impl/Unix$stat", "set", "V", arguments);
		}


		if(setMethodID == 0){
			errno = ENOSYS;
			Unix_cacheErrno();
			return -1;
		}

		JArgument::callVoidMethod(env, jstat, setMethodID, arguments);
		if(env->ExceptionCheck())
			return -1;

	}

	return result;
}



JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Unix_S_1ISREG
  (JNIEnv *, jclass, jint m)
{
	return S_ISREG(m);
}

JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Unix_S_1ISDIR
  (JNIEnv *, jclass, jint m)
{
	return S_ISDIR(m);
}

JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Unix_S_1ISCHR
  (JNIEnv *, jclass, jint m)
{
	return S_ISCHR(m);
}

JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Unix_S_1ISBLK
  (JNIEnv *, jclass, jint m)
{
	return S_ISBLK(m);
}

JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Unix_S_1ISFIFO
  (JNIEnv *, jclass, jint m)
{
	return S_ISFIFO(m);
}

JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Unix_S_1ISLNK
  (JNIEnv *, jclass, jint m)
{
	return S_ISLNK(m);
}

JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Unix_S_1ISSOCK
  (JNIEnv *, jclass, jint m)
{
	return S_ISSOCK(m);
}

