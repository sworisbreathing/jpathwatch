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

#include "JArgument.h"

#include <string>

jmethodID JArgument::getMethodID_impl(JNIEnv* env, const char* className, const char* methodName, const char* returnTypeName, JArgument* arguments, size_t argumentsSize)
{
	jclass clazz = env->FindClass(className);
	jmethodID methodID = 0;

	if(clazz == 0)
		return 0;

	std::string s;
	s.append("(");
	for(size_t i=0; i<argumentsSize; ++i)
		s.push_back(arguments[i].signature);
	s.append(")");
	s.append(returnTypeName);	// void return type
	methodID = env->GetMethodID(clazz, methodName, s.c_str());

	env->DeleteLocalRef(clazz);

	return methodID;
}


void JArgument::callVoidMethod_impl(JNIEnv* env, jobject obj, jmethodID mid, JArgument* arguments, size_t argumentsSize, jvalue* arglist)
{
	for(size_t i=0; i<argumentsSize; ++i)
		arglist[i] = arguments[i].value;

	env->CallVoidMethodA(obj, mid, arglist);
}
