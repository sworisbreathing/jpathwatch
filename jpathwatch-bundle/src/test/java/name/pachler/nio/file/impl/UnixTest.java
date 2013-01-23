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
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.logging.Handler;
import java.util.logging.Level;
import java.util.logging.Logger;
import name.pachler.nio.file.test.logging.TestHandler;
import org.junit.AfterClass;
import org.junit.Test;
import static org.junit.Assert.*;
import org.junit.BeforeClass;

/**
 *
 * @author count
 */
public class UnixTest {
	static boolean isUnix(){
		return LinuxTest.isLinux() || BSDTest.isBSD() || SolarisTest.isSolaris();
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

	@Test(expected=java.lang.NullPointerException.class)
	public void testStat_FirstParamNull(){
		if(!isUnix())
			throw new NullPointerException();	// this is a dummy for platforms other than unix to satisfy the test
		Unix.stat(null, null);
	}

	@Test(expected=java.lang.NullPointerException.class)
	public void testStat_SecondParamNull(){
		if(!isUnix())
			throw new NullPointerException();	// this is a dummy for platforms other than unix to satisfy the test
		Unix.stat("/tmp", null);
	}

	@Test
	public void testStat() throws IOException{
		if(!isUnix())
			return;

		Unix.stat st = new Unix.stat();

		int result;

		// first, stat a path that doesn't exist (unless someone
		// creates a directory like that, which they shouldn't ;)
		result = Unix.stat("/XXXXXXX", st);
		assertEquals(result, -1);
		assertEquals(Unix.errno(), Unix.ENOENT);

		// now see if we can get stat of a directory
		result = Unix.stat("/tmp", st);
		assertTrue(st.st_atime != 0);	// we'll test just one of the stat members here.
		assertTrue(Unix.S_ISDIR(st.st_mode));

		File tempFile = File.createTempFile("jpathwatch-stattest", "tst");
		Unix.stat st1 = new Unix.stat();
		result = Unix.stat(tempFile.getPath(), st1);

		FileOutputStream fos = new FileOutputStream(tempFile);
		fos.write(1);
		fos.close();

		Unix.stat st2 = new Unix.stat();
		result = Unix.stat(tempFile.getPath(), st2);

		assertNotSame(st1, st2);
	}

	@Test
	public void testSymlink() throws IOException{
		if(!isUnix())
			return;

		File f = File.createTempFile("testSymlink", ".test");
		File d = new File(f.getParentFile(), f.getName()+".dir");
		d.mkdir();

		File a = new File(d, "a");
		a.createNewFile();

		File b = new File(d, "b");
		int result = Unix.symlink(a.getPath(), b.getPath());
		assertTrue(result == 0);

		assertTrue(b.exists());
	}

	@Test
	public synchronized void testSelect() throws InterruptedException{
		if(!isUnix())
			return;

		// this is a very simple test for the Unix select() function wrapper:
		// We create a pipe, let a thread write to it and then check
		// if select() detects data on the read end of the pipe.
		final int[] pipefds = new int[2];
		Unix.pipe(pipefds);

		Unix.write(pipefds[1], new byte[]{0}, 1);

		int[] readfds = new int[] {pipefds[0], pipefds[1]};
		int selectresult = Unix.select(readfds, null, null, -1);
		assertEquals(selectresult, 1);

		Unix.close(pipefds[0]);
		Unix.close(pipefds[1]);

	}
}
