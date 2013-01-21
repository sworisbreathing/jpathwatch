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

#include "name_pachler_nio_file_impl_Windows.h"

#include "Windows_OVERLAPPED.h"
#include "Windows_ByteBuffer.h"
#include "windows_include.hpp"
#include "Windows_tls.h"

#include "nativelib.h"

#include <cassert>

static void setLastErrorInTLS(DWORD lastError){
	void* tlsBuffer = Windows_tls_GetBuffer();
	*reinterpret_cast<DWORD*>(tlsBuffer) = lastError;
}


static DWORD getLastErrorFromTLS(){
	void* tlsBuffer = Windows_tls_GetBuffer();
	return *reinterpret_cast<DWORD*>(tlsBuffer);
}



extern "C" {
	JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_Windows_getINVALID_1HANDLE_1VALUE
	  (JNIEnv *, jclass)
	{
		HANDLE inv = INVALID_HANDLE_VALUE;
		return reinterpret_cast<jlong>(inv);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    CreateFile
	 * Signature: (Ljava/lang/String;IILname/pachler/nio/file/impl/WindowsPathWatchService$SECURITY_ATTRIBUTES;IILname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;)Lname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;
	 */
	JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_Windows_CreateFile
	  (JNIEnv* env, jclass, jstring fileName, jint desiredAccess, jint shareMode, jobject securityAttributes, jint creationDisposition, jint flagsAndAttributes, jlong templateFile)
	{
            // convert file name into a zero-terminated UTF-16 string
            const jchar* s = env->GetStringChars(fileName, 0);
            jsize sLength = env->GetStringLength(fileName);
            wchar_t* lpFileName = new wchar_t[sLength+1];
            if(lpFileName == 0)
            {
                env->ReleaseStringChars(fileName, s);
                nativelib_throwOutOfMemoryError(env, "no memory to convert file name to zero terminated UTF-16");
                return reinterpret_cast<jlong>(INVALID_HANDLE_VALUE);    // it actually doesn't matter what we return here; the JVM will call the exception handler anyways
            }

            wcsncpy(lpFileName, reinterpret_cast<const wchar_t*>(s), sLength);
            lpFileName[sLength] = L'\0';

            // parameters
            DWORD dwDesiredAccess = static_cast<DWORD>(desiredAccess);
            DWORD dwShareMode = static_cast<DWORD>(shareMode);
            LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL;	// this parameter is ignored for now
            DWORD dwCreationDisposition = static_cast<DWORD>(creationDisposition);
            DWORD dwFlagsAndAttributes = static_cast<DWORD>(flagsAndAttributes);
            HANDLE hTemplateFile = reinterpret_cast<HANDLE>(templateFile);

            HANDLE result = CreateFileW(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
            setLastErrorInTLS(GetLastError());

            env->ReleaseStringChars(fileName, s);
            delete[] lpFileName;
            
            return reinterpret_cast<jlong>(result);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    CreateEvent
	 * Signature: (Lname/pachler/nio/file/impl/WindowsPathWatchService$SECURITY_ATTRIBUTES;ZZLjava/lang/String;)Lname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;
	 */
	JNIEXPORT jlong JNICALL Java_name_pachler_nio_file_impl_Windows_CreateEvent
	  (JNIEnv* env, jclass, jobject securityAttributes, jboolean manualReset, jboolean initialState, jstring name)
	{
		LPSECURITY_ATTRIBUTES lpEventAttributes = 0;	// unsupported right now
		BOOL bManualReset = manualReset != 0 ? TRUE : FALSE;
		BOOL bInitialState = initialState != 0 ? TRUE : FALSE;
		LPCSTR lpName = 0;
		if(name != 0)
			lpName = env->GetStringUTFChars(name, 0);

		HANDLE event = CreateEventA(lpEventAttributes, bManualReset, bInitialState, lpName);
		setLastErrorInTLS(GetLastError());

		if(name != 0)
			env->ReleaseStringUTFChars(name, lpName);

		return reinterpret_cast<jlong>(event);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    SetEvent
	 * Signature: (Lname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;)Z
	 */
	JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Windows_SetEvent
	  (JNIEnv *, jclass, jlong jHANDLE)
	{
		HANDLE handle = reinterpret_cast<HANDLE>(jHANDLE);
		BOOL result = SetEvent(handle);
		setLastErrorInTLS(GetLastError());
		return static_cast<jboolean>(result);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    ResetEvent
	 * Signature: (Lname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;)Z
	 */
	JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Windows_ResetEvent
	  (JNIEnv *, jclass, jlong jHANDLE)
	{
		HANDLE handle = reinterpret_cast<HANDLE>(jHANDLE);
		BOOL result = ResetEvent(handle);
		setLastErrorInTLS(GetLastError());
		return static_cast<jboolean>(result);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    ReadDirectoryChanges
	 * Signature: (Lname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;Lname/pachler/nio/file/impl/WindowsPathWatchService$ByteBuffer;ZI[ILname/pachler/nio/file/impl/WindowsPathWatchService$OVERLAPPED;Ljava/lang/Runnable;)I
	 */
	JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Windows_ReadDirectoryChanges
	  (JNIEnv* env, jclass, jlong directory, jobject byteBuffer, jboolean watchSubtree, jint notifyFilter, jintArray bytesReturned, jobject overlapped, jobject completionRoutine)
	{
		// check that bytesReturned array can hold at least one element
		// (we'll fill it with the value returned from ReadDirectoryChangesW())
		if(bytesReturned != 0 && env->GetArrayLength(bytesReturned) < 1){
			jclass exclass = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
			if(exclass == 0)
				return false;
			env->ThrowNew(exclass, "bytesReturned array must have at least one element");
			return false;
		}

		DWORD dwBytesReturned = 0;

		HANDLE hDirectory = reinterpret_cast<HANDLE>(directory);
		LPVOID lpBuffer = Windows_ByteBuffer_getBytes(env, byteBuffer);
		DWORD nBufferLength = Windows_ByteBuffer_getSize(env, byteBuffer);
		BOOL bWatchSubtree = static_cast<BOOL>(watchSubtree);
		DWORD dwNotifyFilter = static_cast<DWORD>(notifyFilter);
		LPDWORD lpBytesReturned = &dwBytesReturned;
		LPOVERLAPPED lpOverlapped = 0;
		LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine = 0;	// currently unsupported

		if(overlapped != 0)
			lpOverlapped = reinterpret_cast<LPOVERLAPPED>(Windows_OVERLAPPED_getPeer(env, overlapped));

		BOOL result = ReadDirectoryChangesW(hDirectory, lpBuffer, nBufferLength, bWatchSubtree, dwNotifyFilter, lpBytesReturned, lpOverlapped, lpCompletionRoutine);
		setLastErrorInTLS(GetLastError());

		if(bytesReturned != 0)
		{
			jint bytesReturnedJInt = static_cast<jint>(dwBytesReturned);
			env->SetIntArrayRegion(bytesReturned, 0, 1, &bytesReturnedJInt);
		}

		return static_cast<jboolean>(result);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    WaitForMultipleObjects
	 * Signature: ([Lname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;ZI)I
	 */
	JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Windows_WaitForMultipleObjects
	  (JNIEnv* env, jclass, jlongArray handles, jboolean waitAll, jint milliseconds)
	{
		jsize handlesSize = env->GetArrayLength(handles);
		if(handlesSize > MAXIMUM_WAIT_OBJECTS){
			jclass exclass = env->FindClass("java/lang/IllegalArgumentException");
			env->ThrowNew(exclass, "handles array is longer than MAXIMUM_WAIT_OBJECTS (64)");
			return WAIT_FAILED;
		}

		// convert the java handle array into a local array of Windows HANDLEs
		HANDLE handleArray[MAXIMUM_WAIT_OBJECTS] = {};
		jlong* handlesBuffer = env->GetLongArrayElements(handles, 0);
		for(jsize n=0; n<handlesSize; ++n){
			HANDLE h = reinterpret_cast<HANDLE>(handlesBuffer[n]);
			handleArray[n] = h;
		}
		env->ReleaseLongArrayElements(handles, handlesBuffer, JNI_ABORT);
		handlesBuffer = 0;

		DWORD nCount = static_cast<DWORD>(handlesSize);
		const HANDLE *lpHandles = handleArray;
		BOOL bWaitAll = static_cast<BOOL>(waitAll);
		DWORD dwMilliseconds = static_cast<DWORD>(milliseconds);

		DWORD result = WaitForMultipleObjects(nCount, lpHandles, bWaitAll, dwMilliseconds);
		setLastErrorInTLS(GetLastError());

		return result;
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    GetLastError
	 * Signature: ()I
	 */
	JNIEXPORT jint JNICALL Java_name_pachler_nio_file_impl_Windows_GetLastError
	  (JNIEnv *, jclass)
	{
		// because the Java VM calls Win32 functions with JNI calls, we update
		// the lastError variable that's residing in TLS after every actual
		// Windows API call and just return it here. By the time the java
		// code calls this function the Windows API's own last error code
		// might already have been overwritten by another function call that
		// the VM made.
		return getLastErrorFromTLS();
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    GetLastError_toString
	 * Signature: (I)Ljava/lang/String;
	 */
	JNIEXPORT jstring JNICALL Java_name_pachler_nio_file_impl_Windows_GetLastError_1toString
	  (JNIEnv* env, jclass, jint errorCode)
	{
		char messageBuffer[FORMAT_MESSAGE_MAX_WIDTH_MASK] = {};
		DWORD messageBufferCapacity = FORMAT_MESSAGE_MAX_WIDTH_MASK-1;

		// yuk, what a horrible function!
		DWORD result = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, errorCode, 0, messageBuffer, messageBufferCapacity, 0);
		if(result == 0)
			return 0;

		return env->NewStringUTF(messageBuffer);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    CloseHandle
	 * Signature: (Lname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;)I
	 */
	JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Windows_CloseHandle
	  (JNIEnv *, jclass, jlong handle)
	{
		HANDLE h = reinterpret_cast<HANDLE>(handle);
		BOOL result = CloseHandle(h);
		setLastErrorInTLS(GetLastError());

		return static_cast<jboolean>(result);
	}

	/*
	 * Class:     name_pachler_nio_file_impl_WindowsPathWatchService
	 * Method:    GetOverlappedResult
	 * Signature: (Lname/pachler/nio/file/impl/WindowsPathWatchService$HANDLE;Lname/pachler/nio/file/impl/WindowsPathWatchService$OVERLAPPED;[IZ)Z
	 */
	JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Windows_GetOverlappedResult
	  (JNIEnv* env, jclass, jlong file, jobject overlapped, jintArray numberOfBytesTransferred, jboolean wait)
	{
		if(numberOfBytesTransferred == 0)
		{
			jclass exclass = env->FindClass("java/lang/NullPointerException");
			if(exclass == 0)
				return false;
			env->ThrowNew(exclass, "numberOfBytesTransferred must not be null");
			return false;
		}

		// check that the numberOfBytesTransferred array can hold at least one element
		// (we'll fill it with the value returned from GetOverlappedResult())
		if(env->GetArrayLength(numberOfBytesTransferred) < 1){
			jclass exclass = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
			if(exclass == 0)
				return false;
			env->ThrowNew(exclass, "bytesReturned array must have at least one element");
			return false;
		}

		LPOVERLAPPED lpOverlapped = reinterpret_cast<LPOVERLAPPED>(Windows_OVERLAPPED_getPeer(env, overlapped));
		DWORD nbytes = 0;
		HANDLE hFile = reinterpret_cast<HANDLE>(file);
		BOOL result = GetOverlappedResult(hFile, lpOverlapped, &nbytes, wait);
		setLastErrorInTLS(GetLastError());

		jint nbytesJInt = static_cast<jint>(nbytes);
		env->SetIntArrayRegion(numberOfBytesTransferred, 0, 1, &nbytesJInt);

		return static_cast<jboolean>(result);
	}

	JNIEXPORT jstring JNICALL Java_name_pachler_nio_file_impl_Windows_GetShortPathName
	  (JNIEnv* env, jclass, jstring jfileName)
	{
		const jchar* fileName = env->GetStringChars(jfileName, 0);
		assert(sizeof(jchar)==sizeof(WCHAR));

		jchar buffer[1024];
		const DWORD bufferCapacity = sizeof(buffer)/sizeof(*buffer)-1;
		DWORD bufferSize = GetShortPathNameW((LPCWSTR)fileName, (LPWSTR)buffer, bufferCapacity);
		setLastErrorInTLS(GetLastError());

		jstring jshortFileName = 0;
		if(bufferSize != 0 && bufferSize != bufferCapacity)
			jshortFileName = env->NewString(buffer, bufferSize);

		env->ReleaseStringChars(jfileName, fileName);

		return jshortFileName;
	}

	JNIEXPORT jstring JNICALL Java_name_pachler_nio_file_impl_Windows_GetLongPathName
	  (JNIEnv* env, jclass, jstring jfileName)
	{
		const jchar* fileName = env->GetStringChars(jfileName, 0);

		assert(sizeof(jchar)==sizeof(WCHAR));
		jchar buffer[1024];
		const DWORD bufferCapacity = sizeof(buffer)/sizeof(*buffer)-1;
		DWORD bufferSize = GetLongPathNameW((LPCWSTR)fileName, (LPWSTR)buffer, bufferCapacity);
		setLastErrorInTLS(GetLastError());

		jstring jlongFileName = 0;
		if(bufferSize != 0 && bufferSize != bufferCapacity)
			jlongFileName = env->NewString(buffer, bufferSize);

		env->ReleaseStringChars(jfileName, fileName);

		return jlongFileName;
	}


	JNIEXPORT jboolean JNICALL Java_name_pachler_nio_file_impl_Windows_CancelIo
	  (JNIEnv* env, jclass, jlong fileHandle)
	{
		HANDLE h = reinterpret_cast<HANDLE>(fileHandle);

		BOOL result = CancelIo(h);
		setLastErrorInTLS(GetLastError());

		return result != 0;
	}

}	// extern "C"
