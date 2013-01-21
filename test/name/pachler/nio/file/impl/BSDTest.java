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

import name.pachler.nio.file.impl.BSD.kevent;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author count
 */
public class BSDTest {

    public BSDTest() {
    }

    @BeforeClass
    public static void setUpClass() throws Exception {
    }

    @AfterClass
    public static void tearDownClass() throws Exception {
    }

    @Before
    public void setUp() {
    }

    @After
    public void tearDown() {
    }

    /**
     * Test of kqueue method, of class BSD.
     */
    @Test
    public void testKqueue() {
		if(!isBSD())
			return;	// only run this on BSD

        System.out.println("kqueue");

        // all we can test here is that kqueue returns a valid value.
        int kqueueFD = BSD.kqueue();
        assertFalse(kqueueFD == -1);    // may not return error

        // clean up
        BSD.close(kqueueFD);
    }

	@Test
	public void testKqueueInThread() throws InterruptedException{
//int kqueueFD = BSD.kqueue();
        System.out.println("kevent in thread");
		Thread t = new Thread(new Runnable(){

			public void run() {
				testKqueue();
			}

		});
		t.start();
		t.join();
	}

    /**
     * Test of kevent method, of class BSD.
     */
    @Test
    public void testKevent() {
		if(!isBSD())
			return;	// only run this on BSD

        System.out.println("kevent");
        int kq = BSD.kqueue();
		int tmpDirFD = BSD.open("/tmp", BSD.O_RDONLY, 0);

        BSD.kevent change = new BSD.kevent();
        change.set_flags((short)(BSD.EV_ADD | BSD.EV_CLEAR));
		change.set_ident(tmpDirFD);
		change.set_filter(BSD.EVFILT_VNODE);
		change.set_fflags(BSD.NOTE_DELETE | BSD.NOTE_WRITE | BSD.NOTE_EXTEND | BSD.NOTE_ATTRIB | BSD.NOTE_LINK | BSD.NOTE_RENAME | BSD.NOTE_REVOKE);


        kevent[] changelist = { change };
        int result = BSD.kevent(kq, changelist, null, null);
		if(result == -1)
		{
			String errorMsg = BSD.strerror(BSD.errno());
			System.out.println(errorMsg);
		}
        assertTrue(result == 0);
		kevent[] events = new kevent[16];

		// error cases. First, provide all null arguments
		{
			
			// no changes and no reading should return 0
			result = BSD.kevent(kq, null, null, null);
			assertEquals(0, result);
			
			// an invalid descriptor should reaturn EBADF
			result = BSD.kevent(-1, null, null, null);
			assertEquals(-1, result);
			assertEquals(BSD.errno(), BSD.EBADF);
						
			// an invalid descriptor, but this time with an event list - 
			// should read 1 event that is an error
/*			{
				kevent c = new kevent();
				change.set_ident(-1);
				change.set_filter((short)BSD.EVFILT_VNODE);
				change.set_flags((short)BSD.EV_ADD);
				change.set_fflags((short)BSD.NOTE_FORK);
				kevent[] elist = new kevent[1];
				BSD.timespec ts = new BSD.timespec();	// is zero

				result = BSD.kevent(kq, new kevent[]{c}, elist, ts);
System.out.println(BSD.strerror(BSD.errno()));
				assertEquals(1, result);
				assertNotNull(elist[0]);
				int flags = elist[0].get_flags();
				long data = elist[0].get_data();
				assertTrue((elist[0].get_flags() & BSD.EV_ERROR) != 0);
				assertEquals(data, BSD.EINVAL);
			}
*/
			
			// no changes and no reading - should return 0
			result = BSD.kevent(kq, null, null, null);
			assertEquals(0, result);

			// providing a changelist with null elements is an error and
			// should throw a NullPointerException
			boolean nullPointerExceptionThrown = false;
			try {
				BSD.kevent(kq, new kevent[1], null, null);
			} catch(NullPointerException npx){
				nullPointerExceptionThrown = true;
			}
			assertTrue(nullPointerExceptionThrown);


		}
/*		for(;;){
			int[] readfds = {kq};
			BSD.select(readfds, null, null, -1);
			System.out.println("so something changed");
			if(readfds[0] != kq)
				continue;
			int nevents = BSD.kevent(kq, null, events, null);
			for(int i=0; i<nevents; ++i)
			{
				kevent e = events[i];
				if(e!=null)
					System.out.println("kevent:" + e.toString());
				else
					System.out.println("kevent: <null>");
			}
		}
*/    }

	static boolean isBSD() {
		String osName = System.getProperty("os.name");
		return osName.contains("BSD") || osName.equals("Mac OS X");
	}

}