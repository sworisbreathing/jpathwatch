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

#ifndef WINDOWS_INCLUDE_HPP
#define WINDOWS_INCLUDE_HPP

#define WIN32_LEAN_AND_MEAN

// we need these macros so that the API functions we need are defined
#define _WIN32_WINNT 0x0501
#define NTDDI_VERSION NTDDI_WIN2K
// *yelp*
#include <windows.h>

// no thanks for defining these macros, M$!
#undef min
#undef max

#endif	// WINDOWS_INCLUDE_HPP
