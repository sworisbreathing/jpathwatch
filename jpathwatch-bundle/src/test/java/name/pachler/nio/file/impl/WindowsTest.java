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

package name.pachler.nio.file.impl;

import java.io.File;
import java.io.IOException;
import java.util.concurrent.TimeUnit;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;
import name.pachler.nio.file.WatchEvent.Kind;
import name.pachler.nio.file.WatchEvent.Modifier;
import name.pachler.nio.file.WatchKey;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;
import name.pachler.nio.file.impl.Windows;
import name.pachler.nio.file.impl.Windows.ByteBuffer;
import name.pachler.nio.file.test.logging.TestHandler;

/**
 *
 * @author count
 */
public class WindowsTest {

	static boolean isWindows(){
		return System.getProperty("os.name").toLowerCase().contains("windows");
	}
    public WindowsTest() {
    }

        private static Level originalLevel = null;

        private static Handler loggingHandler = null;

	@BeforeClass
	public static void setUpClass() throws Exception {
            originalLevel = Logger.getLogger("").getLevel();
            Logger.getLogger("").setLevel(Level.ALL);
            loggingHandler = new TestHandler();
            Logger.getLogger("").addHandler(loggingHandler);
	}

	@AfterClass
	public static void tearDownClass() throws Exception {
            Logger.getLogger("").setLevel(originalLevel);
            originalLevel = null;
            Logger.getLogger("").removeHandler(loggingHandler);
            loggingHandler = null;
	}

    @Before
    public void setUp() {
    }

    @After
    public void tearDown() {
    }

	/**
	 * Test of CreateFile method, of class Windows.
	 */
	@Test
	public void testCreateFile() throws IOException {
		// this test is only relevant on the windows platform
		if(!isWindows())
			return;

		File f = File.createTempFile("testCreateFile", "");
		f.deleteOnExit();
		File d = new File(f.getParentFile(), f.getName()+".dir");
		d.mkdir();
		d.deleteOnExit();

		File asciiFile = new File(d, "test");
		String asciiFileName = asciiFile.getAbsolutePath();
		int desiredAccess = Windows.GENERIC_WRITE;
		int shareMode = 0;
		Windows.SECURITY_ATTRIBUTES securityAttributes = null;
		int creationDisposition = Windows.CREATE_NEW;
		int flagsAndAttributes = 0;
		long templateFileHandle = 0L;

		long handle = Windows.CreateFile(asciiFileName, desiredAccess, shareMode, securityAttributes, creationDisposition, flagsAndAttributes, templateFileHandle);
		assertFalse(handle == Windows.INVALID_HANDLE_VALUE);
		assertTrue(asciiFile.exists());

		boolean couldCloseHandle = Windows.CloseHandle(handle);
		assertTrue(couldCloseHandle);

		// again, should return error now; can't open file twice
		long handle2 = Windows.CreateFile(asciiFileName, desiredAccess, shareMode, securityAttributes, creationDisposition, flagsAndAttributes, templateFileHandle);
		assertTrue(handle2 == Windows.INVALID_HANDLE_VALUE);

		int error = Windows.GetLastError();
		assertTrue(error == Windows.ERROR_FILE_EXISTS);

		// now, try creating a file with a unicode file name
		File unicodeFile = new File(d, "test\u00e0");
		String unicodeFileName = unicodeFile.getAbsolutePath();
		long handle3 = Windows.CreateFile(unicodeFileName, desiredAccess, shareMode, securityAttributes, creationDisposition, flagsAndAttributes, templateFileHandle);
		assertFalse(handle3 == Windows.INVALID_HANDLE_VALUE);
		assertTrue(unicodeFile.exists());

		unicodeFile.delete();
		asciiFile.delete();
	}

	@Test
	/**
	 * tests synchronous file watch
	 */
	public void testSynchronousFileNotification() throws IOException{
		// this test is only relevant on the windows platform
		if(!isWindows())
			return;

		File f = File.createTempFile("testSynchronousFileNotification", "");
		f.deleteOnExit();
		final File d = new File(f.getParentFile(), f.getName()+".dir");
		d.mkdir();

		String fileName = d.getAbsolutePath();
		int desiredAccess = Windows.FILE_LIST_DIRECTORY;
		int shareMode = Windows.FILE_SHARE_READ | Windows.FILE_SHARE_WRITE | Windows.FILE_SHARE_DELETE;
		Windows.SECURITY_ATTRIBUTES securityAttributes = null;
		int creationDisposition = Windows.OPEN_EXISTING;
		int flagsAndAttributes = Windows.FILE_FLAG_BACKUP_SEMANTICS;
		long templateFileHandle = 0L;

		long dirHandle = Windows.CreateFile(fileName, desiredAccess, shareMode, securityAttributes, creationDisposition, flagsAndAttributes, templateFileHandle);
		assertTrue(dirHandle != 0);

		// make thread that should create a directory in a second
		new Thread(){
			@Override
			public synchronized void run() {
				try {
					wait(1000);
					File f = new File(d, "test");
					f.createNewFile();
					f.deleteOnExit();
				} catch (InterruptedException ex) {
					Logger.getLogger(WindowsTest.class.getName()).log(Level.SEVERE, null, ex);
				} catch (IOException ex) {
					Logger.getLogger(WindowsTest.class.getName()).log(Level.SEVERE, null, ex);
				}
			}
		}.start();

		ByteBuffer buffer = new ByteBuffer(8192);
		boolean watchSubtree = false;
		int notifyFilter = Windows.FILE_NOTIFY_CHANGE_DIR_NAME | Windows.FILE_NOTIFY_CHANGE_FILE_NAME;
		int[] bytesReturned = {0};
		Windows.OVERLAPPED overlapped = null;
		Runnable completionRoutine = null;
		boolean result = Windows.ReadDirectoryChanges(dirHandle, buffer, watchSubtree, notifyFilter, bytesReturned, overlapped, completionRoutine);

		assertTrue(result);
		assertTrue(bytesReturned[0] > 0);

		result = Windows.CloseHandle(dirHandle);

		assertTrue(result);
	}


	/**
	 * tests synchronous file watch
	 */
	@Test
	public void testAsynchronousFileNotification() throws IOException, InterruptedException{
		// this test is only relevant on the windows platform
		if(!isWindows())
			return;

		File f = File.createTempFile("testAsynchronousFileNotification", "");
		f.deleteOnExit();
		final File d = new File(f.getParentFile(), f.getName()+".dir");
		d.mkdir();

		String fileName = d.getAbsolutePath();
		int desiredAccess = Windows.FILE_LIST_DIRECTORY;
		int shareMode = Windows.FILE_SHARE_READ | Windows.FILE_SHARE_WRITE | Windows.FILE_SHARE_DELETE;
		Windows.SECURITY_ATTRIBUTES securityAttributes = null;
		int creationDisposition = Windows.OPEN_EXISTING;
		int flagsAndAttributes = Windows.FILE_FLAG_BACKUP_SEMANTICS | Windows.FILE_FLAG_OVERLAPPED;
		long templateFileHandle = 0L;

		long dirHandle = Windows.CreateFile(fileName, desiredAccess, shareMode, securityAttributes, creationDisposition, flagsAndAttributes, templateFileHandle);
		assertTrue(dirHandle != 0);

		ByteBuffer buffer = new ByteBuffer(8192);
		boolean watchSubtree = false;
		int notifyFilter = Windows.FILE_NOTIFY_CHANGE_FILE_NAME | Windows.FILE_NOTIFY_CHANGE_DIR_NAME;
		int[] bytesReturned = {0};
		Windows.OVERLAPPED overlapped = new Windows.OVERLAPPED();
		Runnable completionRoutine = null;

		// test it by synchronizing with the directory handle
		// (OVERLAPPED has no event handle set)
		{
			boolean result = Windows.ReadDirectoryChanges(dirHandle, buffer, watchSubtree, notifyFilter, bytesReturned, overlapped, completionRoutine);
			assertTrue(result);
			assertTrue(bytesReturned[0] == 0);

			long[] handles = { dirHandle };

			int wfmoresult = Windows.WaitForMultipleObjects(handles, false, 200);
			assertTrue(wfmoresult == Windows.WAIT_TIMEOUT);

			File testFile = new File(d, "test");
			testFile.createNewFile();
			testFile.deleteOnExit();
			synchronized(this){
				wait(1000);
			}

			wfmoresult = Windows.WaitForMultipleObjects(handles, false, Windows.INFINITE);
			assertTrue(wfmoresult == Windows.WAIT_OBJECT_0);

			boolean gorresult = Windows.GetOverlappedResult(dirHandle, overlapped, bytesReturned, false);
			assertTrue(gorresult);
			assertTrue(bytesReturned[0] > 0);
		}

		// now test synchronization with the event handle rather than the
		// directory handle
		{
			long eh = Windows.CreateEvent(null, true, false, null);
			assertTrue(eh != 0);

			overlapped.setEvent(eh);
			boolean result = Windows.ReadDirectoryChanges(dirHandle, buffer, watchSubtree, notifyFilter, bytesReturned, overlapped, completionRoutine);
			assertTrue(result);
			assertTrue(bytesReturned[0] == 0);

			long[] handles = {eh};

			int wfmoresult = Windows.WaitForMultipleObjects(handles, false, 200);
			assertTrue(wfmoresult == Windows.WAIT_TIMEOUT);

			File testFile2 = new File(d, "test2");
			testFile2.createNewFile();
			testFile2.deleteOnExit();

			wfmoresult = Windows.WaitForMultipleObjects(handles, false, 200);
			assertTrue(wfmoresult == Windows.WAIT_OBJECT_0);

			boolean gorresult = Windows.GetOverlappedResult(dirHandle, overlapped, bytesReturned, false);
			assertTrue(gorresult);
			assertTrue(bytesReturned[0] > 0);
		}

		boolean result = Windows.CloseHandle(dirHandle);
		assertTrue(result);
	}


	@Test
	public void testEventFunctions(){
		// this test is only relevant on the windows platform
		if(!isWindows())
			return;

		long h = Windows.CreateEvent(null, true, false, null);
		assertTrue(h != 0);

		long[] handles = { h };
		int wfmoresult = Windows.WaitForMultipleObjects(handles, false, 200);
		assertTrue(wfmoresult == Windows.WAIT_TIMEOUT);

		boolean seresult = Windows.SetEvent(h);
		assertTrue(seresult);

		wfmoresult = Windows.WaitForMultipleObjects(handles, false, 200);
		assertTrue(wfmoresult == Windows.WAIT_OBJECT_0);

		boolean reresult = Windows.ResetEvent(h);
		assertTrue(seresult);

		wfmoresult = Windows.WaitForMultipleObjects(handles, false, 200);
		assertTrue(wfmoresult == Windows.WAIT_TIMEOUT);
	}
}