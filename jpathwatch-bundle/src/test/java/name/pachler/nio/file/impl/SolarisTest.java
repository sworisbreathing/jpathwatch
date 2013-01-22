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

import org.junit.Test;
import static org.junit.Assert.*;

/**
 *
 * @author count
 */
public class SolarisTest {
	static boolean isSolaris(){
		return System.getProperty("os.name").toLowerCase().contains("solaris");
	}

	@Test
	public void testPortCreate(){
		if(!isSolaris())
			return;

		int port = Solaris.port_create();
		assertNotSame(port, -1);
		Solaris.close(port);
	}

	@Test
	public void testPortAssociate(){
		if(!isSolaris())
			return;

		int port = Solaris.port_create();
		assertNotSame(port, -1);

		Solaris.stat st = new Solaris.stat();
		Solaris.stat("/tmp", st);
		Solaris.file_obj fileobj = new Solaris.file_obj();

		int events = -1;	// FIXME: needs proper value
		Solaris.port_associate(port, Solaris.PORT_SOURCE_FILE, fileobj, events, null);
	}
}
