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
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author count
 */
public class LinuxTest {
	static {

	}
	public static boolean isLinux(){
		String osName = System.getProperty("os.name");
		return osName.toLowerCase().contains("linux");
	}

	@Test
	public synchronized void testInotify() throws IOException, InterruptedException{
		if(!isLinux())
			return;
		
		int ifd = Linux.inotify_init();
		assertFalse(ifd == -1);

		File tmpFile = File.createTempFile("testInotify", "");
		File tmpDir = new File(tmpFile.getParentFile(), tmpFile.getName()+".dir");
		tmpDir.mkdir();
		tmpDir.deleteOnExit();
		assertTrue(tmpDir.exists());

		int watchDescriptor = Linux.inotify_add_watch(ifd, tmpDir.getPath(), Linux.IN_CREATE | Linux.IN_DELETE);
		assertTrue(watchDescriptor > 0);

		int bytesAvailable = Linux.ioctl_FIONREAD(ifd);
		assertEquals(bytesAvailable, 0);

		// create and delete test file - this should make bytes
		// available at our inotify FD
		File testFile = new File(tmpDir, "testfile");
		testFile.createNewFile();
		testFile.delete();

		// wait to give the kernel time to insert the event into the
		// inotify queue
		wait(200);

		// now we should have bytes available to read. We won't check
		// the format and contents of the payload data here though.
		bytesAvailable = Linux.ioctl_FIONREAD(ifd);
		assertTrue(bytesAvailable > 0);

		int result = Linux.close(ifd);
		assertTrue(result == 0);
	}

	@Test
	public void testStatfs() throws IOException{
		if(!isLinux())
			return;

		Linux.statfs stfs = new Linux.statfs();

		int result;

		// first, stat a path that doesn't exist (unless someone
		// creates a directory like that, which they shouldn't ;)
		result = Linux.statfs("/XXXXXXX", stfs);
		assertEquals(result, -1);
		assertEquals(Unix.errno(), Unix.ENOENT);

		// now see if we can get stat of a directory
		result = Linux.statfs("/tmp", stfs);
		assertTrue(stfs.f_namelen != 0);	// we'll test just one of the stat members here.
}

}
