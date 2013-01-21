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

#include "windows_include.hpp"

static DWORD dwTlsIndex; // address of shared memory

static const DWORD TLS_size = 256;


// DllMain() is the entry-point function for this DLL.

BOOL WINAPI DllMain(HINSTANCE hinstDLL, // DLL module handle
    DWORD fdwReason,                    // reason called
    LPVOID lpvReserved)                 // reserved
{
    LPVOID lpvData;
    BOOL fIgnore;

    switch (fdwReason)
    {
        // The DLL is loading due to process
        // initialization or a call to LoadLibrary.

        case DLL_PROCESS_ATTACH:

            // Allocate a TLS index.

            if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES)
                return FALSE;

            // No break: Initialize the index for first thread.

        // The attached process creates a new thread.

        case DLL_THREAD_ATTACH:

            // Initialize the TLS index for this thread.

            lpvData = (LPVOID) LocalAlloc(LPTR, TLS_size);
            if (lpvData != NULL)
                fIgnore = TlsSetValue(dwTlsIndex, lpvData);

            break;

        // The thread of the attached process terminates.

        case DLL_THREAD_DETACH:

            // Release the allocated memory for this thread.

            lpvData = TlsGetValue(dwTlsIndex);
            if (lpvData != NULL)
                LocalFree((HLOCAL) lpvData);

            break;

        // DLL unload due to process termination or FreeLibrary.

        case DLL_PROCESS_DETACH:

            // Release the allocated memory for this thread.

            lpvData = TlsGetValue(dwTlsIndex);
            if (lpvData != NULL)
                LocalFree((HLOCAL) lpvData);

            // Release the TLS index.

            TlsFree(dwTlsIndex);
            break;

        default:
            break;
    }

    return TRUE;
    UNREFERENCED_PARAMETER(hinstDLL);
    UNREFERENCED_PARAMETER(lpvReserved);
}


size_t Windows_tls_GetSize()
{
	return TLS_size;
}

void* Windows_tls_GetBuffer()
{
   LPVOID lpvData;

   lpvData = TlsGetValue(dwTlsIndex);
   if (lpvData == NULL)
   {
      lpvData = (LPVOID) LocalAlloc(LPTR, TLS_size);
      if (lpvData == NULL)
         return FALSE;
      if (!TlsSetValue(dwTlsIndex, lpvData))
         return FALSE;
   }

   return lpvData;
}

