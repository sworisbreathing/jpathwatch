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

#ifndef JARGUMENT_H
#define	JARGUMENT_H

#include <jni.h>
#include <cassert>

/**
 * A little wrapper to manage calls to java methods and ensure they use the same
 * signature for obtaining the method reference as well as calling it and
 * passing the parameters in in the right order.
 *
 * Build an array of JArgument instances, then call getMethodID(), and then
 * callVoidMethod() both static methods. By passing in the same array you
 * ensure that method lookup and method call will always match. This avoids
 * the case where, for instance, you look for a method taking three ints,
 * but you're actually passing two ints and a long (which would stuff things
 * up quite badly).
 *
 * int value1, value2;
 * long value3;
 *
 * ...
 * JArgument args[] = {
 *	{ value1, 'I' }
 *  { value2, 'I' }
 *  { value3, 'J' }
 * };
 *
 * // lookup method
 * jmethodID mid = JArgument::getMethodID(env, "MyClass", "myMethod", "V", args);
 *
 * // call method
 * JArgument::callVoidMethod(env, javaObject, mid, args);
 */
struct JArgument{
	jvalue value;
	char signature;

private:
	static jmethodID getMethodID_impl(JNIEnv* env, const char* className, const char* methodName, const char* returnTypeName, JArgument* arguments, size_t argumentsSize);

	static void callVoidMethod_impl(JNIEnv* env, jobject obj, jmethodID mid, JArgument* arguments, size_t argumentsSize, jvalue* arglist);

public:
	template <typename T>
	JArgument(T v, char sig){
		this->signature = sig;
		switch(sig){
			case 'I':
				this->value.i = v;
				break;
			case 'J':
				this->value.j = v;
				break;
			default:
				assert(false && "argument type not handled");
				break;
		}
	}

	template<size_t N>
	static jmethodID getMethodID(JNIEnv* env, const char* className, const char* methodName, const char* returnTypeName, JArgument(&arguments)[N])
	{
		return getMethodID_impl(env, className, methodName, returnTypeName, arguments, N);
	}

	template <size_t N>
	static void callVoidMethod(JNIEnv* env, jobject obj, jmethodID mid, JArgument (&arguments)[N])
	{
		const size_t argumentsSize = N;
		jvalue arglist[argumentsSize] = {};

		callVoidMethod_impl(env, obj, mid, arguments, argumentsSize, arglist);
	}
};

#endif	/* JARGUMENT_H */

