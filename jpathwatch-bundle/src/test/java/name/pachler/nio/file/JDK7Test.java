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

package name.pachler.nio.file;

import java.util.logging.Level;
import java.util.logging.Logger;
import name.pachler.nio.file.WatchEvent;
import name.pachler.nio.file.WatchKey;
import name.pachler.nio.file.WatchService;
import name.pachler.nio.file.StandardWatchEventKind;
import java.io.*;
import java.util.*;
import java.util.concurrent.TimeUnit;
import java.util.logging.Handler;
import name.pachler.nio.file.ClosedWatchServiceException;
import name.pachler.nio.file.FileSystems;
import name.pachler.nio.file.Path;
import name.pachler.nio.file.Paths;
import name.pachler.nio.file.ext.Bootstrapper;
import name.pachler.nio.file.ext.ExtendedWatchEventModifier;
import name.pachler.nio.file.impl.PollingPathWatchService;
import org.junit.Assert;
import org.junit.Test;
import static org.junit.Assert.*;
import static name.pachler.nio.file.StandardWatchEventKind.*;
import static name.pachler.nio.file.ext.ExtendedWatchEventKind.*;
import name.pachler.nio.file.test.logging.TestHandler;
import org.junit.AfterClass;
import org.junit.BeforeClass;

public class JDK7Test {

// enable this to test the PollingPathWatchService on platforms that would
// otherwise use their native implementation
/*
	static
	{
		Bootstrapper.setForcePollingEnabled(true);
		Bootstrapper.setDefaultPollingInterval(500);
	}
*/
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

	static class SingleWatchKeyTaker extends Thread{
		WatchService watcher;
		WatchKey watchKey;
		List<WatchEvent<?>> events;
		boolean noExceptions = false;
		boolean closedException = false;

		public SingleWatchKeyTaker(WatchService watcher) {
			this.watcher = watcher;
		}

		public SingleWatchKeyTaker(WatchService watcher, String threadName) {
			super(threadName);
			this.watcher = watcher;
		}

		@Override
		public void run() {
			try {
				watchKey = watcher.take();
				Assert.assertNotNull(watchKey);
				events = watchKey.pollEvents();
				Assert.assertTrue(watchKey.reset());

				noExceptions = true;
			} catch (InterruptedException ex) {
				Logger.getLogger(JDK7Test.class.getName()).log(Level.SEVERE, null, ex);
			} catch (ClosedWatchServiceException ex) {
				closedException = true;
			}
		}

	}

	static File createTempDir(String prefix) throws IOException{
		File file = File.createTempFile(prefix, "");
		file.deleteOnExit();
		File dir = new File(file.getParentFile(), file.getName()+".dir");
		dir.mkdir();
		dir.deleteOnExit();
		return dir;
	}

	static void appendBytesToFile(File file, byte[] bytes) throws IOException {
		FileOutputStream fos = new FileOutputStream(file, true);
		fos.write(bytes, 0, bytes.length);
		fos.close();
	}

	boolean listContainsEvent(List<WatchEvent<?>> list, WatchEvent.Kind kind, String fileName)
	{
		Path context = null;
		if(fileName !=null)
			context = Bootstrapper.newPath(new File(fileName));

		for(WatchEvent<?> key : list){
			boolean sameKind = key.kind() == kind;
			boolean sameContext = key.context()== null && context==null || key.context().equals(context);

			if(sameKind && sameContext)
				return true;
		}
		return false;
	}

	@Test
	public void testTwoTakerThreads() throws IOException, InterruptedException{
		// WatchKeys for two distinct directories are created. Two threads
		// are created, each calling take() once. In each observed directory,
		// a single file is created.
		// The expected result is that each thread receives one of the WatchKeys
		// as a return value from take().

		WatchService w = Bootstrapper.newWatchService();

		SingleWatchKeyTaker swkt1 = new SingleWatchKeyTaker(w, "swkt1");
		SingleWatchKeyTaker swkt2 = new SingleWatchKeyTaker(w, "swkt2");

		File dir1 = createTempDir("dir1");
		Path path1 = Bootstrapper.newPath(dir1);
		WatchKey wk1 = path1.register(w, StandardWatchEventKind.ENTRY_CREATE);

		File dir2 = createTempDir("dir2");
		Path path2 = Bootstrapper.newPath(dir2);
		WatchKey wk2 = path2.register(w, StandardWatchEventKind.ENTRY_CREATE);

		swkt1.start();
		swkt2.start();

		File file1 = new File(dir1, "file1");
		file1.createNewFile();
		file1.deleteOnExit();
		File file2 = new File(dir2, "file2");
		file2.createNewFile();
		file2.deleteOnExit();

		synchronized(this){
			wait(1000);	// wait a second to give threads time to process events
		}

		// because threads each take a single watch key and terminate straight
		// after, they should all be dead by now
		Assert.assertFalse(swkt1.isAlive());
		Assert.assertFalse(swkt2.isAlive());

		// put all received watch keys into a set
		Set<WatchKey> paths = new HashSet<WatchKey>();
		paths.add(swkt1.watchKey);
		paths.add(swkt2.watchKey);

		// check that the watch keys the threads received are distinct, and
		// that they were the keys that we used
		Assert.assertTrue(swkt1.watchKey != swkt2.watchKey);
		Assert.assertTrue(paths.contains(wk1));
		Assert.assertTrue(paths.contains(wk2));

		w.close();
	}

	@Test
	public synchronized void testOneTaker() throws IOException, InterruptedException{
		WatchService watcher = Bootstrapper.newWatchService();

		File dir = createTempDir("dir1");
		Path path = Bootstrapper.newPath(dir);
		WatchKey wk = path.register(watcher, StandardWatchEventKind.ENTRY_CREATE);

		// wait for 200ms - should return no key
		WatchKey key = watcher.poll(200, TimeUnit.MILLISECONDS);
		assertNull(key);

		File file1 = new File(dir, "file1");
		file1.createNewFile();

		wait(200);

		key = watcher.take();
		assertNotNull(key);
		List<WatchEvent<?>> eventList = key.pollEvents();
		assertTrue(eventList.size() == 1);
		WatchEvent event = eventList.get(0);
		assertEquals(event.context(), Bootstrapper.newPath(new File("file1")));
		assertEquals(event.kind(), ENTRY_CREATE);

		key.reset();

		File file2 = new File(dir, "file2");
		file2.createNewFile();

		wait(200);

		key = watcher.take();
		assertNotNull(key);
		eventList = key.pollEvents();
		assertTrue(eventList.size() == 1);
		event = eventList.get(0);
		assertEquals(event.context(), Bootstrapper.newPath(new File("file2")));
		assertEquals(event.kind(), ENTRY_CREATE);

		key.reset();

		// test file with single-character file name
		File file3 = new File(dir, "3");
		file3.createNewFile();

		wait(200);

		key = watcher.take();
		assertNotNull(key);
		eventList = key.pollEvents();
		assertTrue(eventList.size() == 1);
		event = eventList.get(0);
		assertEquals(event.context(), Bootstrapper.newPath(new File("3")));
		assertEquals(event.kind(), ENTRY_CREATE);

		watcher.close();
	}

	@Test
	public synchronized void testOneTakerThreaded() throws IOException, InterruptedException{
		// WatchKeys for two distinct directories are created. Two threads
		// are created, each calling take() once. In each observed directory,
		// a single file is created.
		// The expected result is that each thread receives one of the WatchKeys
		// as a return value from take().

		WatchService watcher = Bootstrapper.newWatchService();

		SingleWatchKeyTaker swkt = new SingleWatchKeyTaker(watcher);

		File dir = createTempDir("dir1");
		Path path = Bootstrapper.newPath(dir);
		WatchKey wk = path.register(watcher, StandardWatchEventKind.ENTRY_CREATE);

		File file = new File(dir, "file1");
		file.createNewFile();
		file.deleteOnExit();

		WatchKey key = watcher.take();
		assertTrue(key != null);
		boolean resetSuccessful = key.reset();
		assertTrue(resetSuccessful);

		swkt.start();

		File file2 = new File(dir, "file2");
		file2.createNewFile();
		file2.deleteOnExit();

		wait(200);	// wait a second to give threads time to process events

		// because threads each take a single watch key and terminate straight
		// after, they should all be dead by now
		Assert.assertFalse(swkt.isAlive());

		// check that the watch keys the threads received are distinct, and
		// that they were the keys that we used
		Assert.assertTrue(swkt.watchKey == wk);

		watcher.close();
	}

	@Test
	public void testPollTimeout() throws IOException, ClosedWatchServiceException, InterruptedException{

		// test to check if the polling timeout works properly.
		// The timeouts don't need to be exact (they can't be), but
		// they should at least be reasonably close (we assume 10% delta here)
		// to the specified timeout value.
		WatchService watcher = null;
		try {
			watcher = Bootstrapper.newWatchService();

			File dir = createTempDir("testPollTimeout");

			Path path = Paths.get(dir.getPath());
			WatchKey key = path.register(watcher, ENTRY_CREATE);

			long timeout = 2500;

			long startTime = System.currentTimeMillis();

			watcher.poll(timeout, TimeUnit.MILLISECONDS);

			long endTime = System.currentTimeMillis();

			long elapsed = endTime - startTime;
			assertEquals(timeout, elapsed, timeout*.1);

			timeout = 2000;
			startTime = System.currentTimeMillis();
			watcher.poll(timeout/1000, TimeUnit.SECONDS);

			endTime = System.currentTimeMillis();

			elapsed = endTime - startTime;
			assertEquals(timeout, elapsed, timeout*.1);
		} finally {
			if(watcher != null)
				watcher.close();
		}
	}

	@Test
	public synchronized void testOneTakerClosing() throws IOException, InterruptedException{
		// test that closing a WatchService makes it raise a
		// ClosedWatchServiceException from a take() call
		WatchService watcher = Bootstrapper.newWatchService();
		SingleWatchKeyTaker swkt = new SingleWatchKeyTaker(watcher);

		swkt.start();

		// this should make sure that swkt is actually running and waiting in WatchService.take().
		wait(200);

		watcher.close();

		swkt.join(1000);
		Assert.assertFalse(swkt.isAlive());
		Assert.assertTrue(swkt.closedException);
	}

	@Test
	public synchronized void testTakeFirstThenRegister() throws IOException, InterruptedException{
		// Tests the following case:
		// * A thread calls take on a WatchService that has no paths registered
		//	yet. take() will block.
		// * Another thread registers a path afterwards, and then creates
		//	a file in the watched path.
		// * The first thread should return a watch key now
		WatchService watcher = Bootstrapper.newWatchService();
		SingleWatchKeyTaker swkt = new SingleWatchKeyTaker(watcher);

		swkt.start();

		wait(200);	// give the swkt thread time to call take()...

		File dir = createTempDir("testTakeFirstThenRegister");
		Path path = Bootstrapper.newPath(dir);
		WatchKey key = path.register(watcher, ENTRY_CREATE, ENTRY_DELETE, ENTRY_MODIFY);
		File file = new File(dir, "test");
		file.createNewFile();
		file.deleteOnExit();

		// the take() call in the swkt thread should have returned by now
		// and returned the key
		long joinTimeout = 1000;

		if(watcher instanceof PollingPathWatchService)
			joinTimeout = (long)(1.5f * ((PollingPathWatchService)watcher).getPollInterval());	// longer timeout if we have a polling service

		swkt.join(joinTimeout);

		Assert.assertFalse(swkt.isAlive());
		Assert.assertTrue(swkt.watchKey == key);

		watcher.close();
	}

	@Test
	public synchronized void testRename() throws IOException, InterruptedException{
		// Tests watching a directory within which a file is renamed.
		// Only standard event kinds are to be used.
		WatchService watcher = Bootstrapper.newWatchService();

		File dir = createTempDir("testRename");
		Path path = Bootstrapper.newPath(dir);
		File file = new File(dir, "test1");
		file.createNewFile();
		file.deleteOnExit();

		WatchKey key1 = path.register(watcher, ENTRY_CREATE, ENTRY_DELETE);

		boolean couldRename;
		{
			File f = new File(dir, "test2");
			couldRename = file.renameTo(f);
			file = f;
		}
		assertTrue(couldRename);

		wait(200);
		SingleWatchKeyTaker swkt1 = new SingleWatchKeyTaker(watcher);
		swkt1.start();

		swkt1.join(200);

		Assert.assertFalse(swkt1.isAlive());
		Assert.assertTrue(swkt1.watchKey == key1);
		Assert.assertTrue(listContainsEvent(swkt1.events, ENTRY_DELETE, "test1"));
		Assert.assertTrue(listContainsEvent(swkt1.events, ENTRY_CREATE, "test2"));
		key1.cancel();

		boolean platformSupportsRenameEvents =
			System.getProperty("os.name").contains("Windows")
		||	System.getProperty("os.name").contains("Linux");


		if(platformSupportsRenameEvents && !Bootstrapper.isForcePollingEnabled())
		{
			WatchKey key2 = path.register(watcher, ENTRY_CREATE, ENTRY_DELETE, ENTRY_RENAME_FROM, ENTRY_RENAME_TO);


			{
				File f = new File(dir, "test3");
				couldRename = file.renameTo(f);
				file = f;
			}
			assertTrue(couldRename);

			wait(200);

			SingleWatchKeyTaker swkt2 = new SingleWatchKeyTaker(watcher);
			swkt2.start();
			wait(200);

			Assert.assertFalse(swkt2.isAlive());
			Assert.assertTrue(listContainsEvent(swkt2.events, ENTRY_RENAME_FROM, "test2"));
			Assert.assertTrue(listContainsEvent(swkt2.events, ENTRY_RENAME_TO, "test3"));
		}

		watcher.close();
	}

	@Test
	public void testForeignRename() throws IOException, InterruptedException{
		// This test checks if files that are moved into and out of a watched
		// directory are correctly recognized as ENTRY_CREATE / ENTRY_DELETE
		// (and not as ENTRY_RENAME_TO / ENTRY_RENAME_FROM that don't have a
		// corresponding _FROM / _TO event, as in bug #3309509
		// (http://sourceforge.net/tracker/?func=detail&aid=3309509&group_id=245450&atid=1419016)
		WatchService watcher = Bootstrapper.newWatchService();

		File watchedDir = createTempDir("testForeignRename-watched");
		Path path = Bootstrapper.newPath(watchedDir);

		File unwatchedDir = createTempDir("testForeignRename-not-watched");

		File file1 = new File(watchedDir, "test1-watched");
		file1.createNewFile();
		file1.deleteOnExit();

		File file2 = new File(unwatchedDir, "test2-unwatched");
		file2.createNewFile();
		file2.deleteOnExit();

		boolean platformSupportsRenameEvents =
			System.getProperty("os.name").contains("Windows")
		||	System.getProperty("os.name").contains("Linux");


		WatchKey key1;
		if(platformSupportsRenameEvents)
			key1 = path.register(watcher, ENTRY_CREATE, ENTRY_DELETE, ENTRY_RENAME_FROM, ENTRY_RENAME_TO);
		else
			key1 = path.register(watcher, ENTRY_CREATE, ENTRY_DELETE);

		boolean couldRename;
		{
			File f = new File(unwatchedDir, "test1-unwatched");
			couldRename = file1.renameTo(f);
			f.deleteOnExit();
			file1 = f;
		}
		assertTrue(couldRename);

		{
			File f = new File(watchedDir, "test2-watched");
			couldRename = file1.renameTo(f);
			f.deleteOnExit();
			file2 = f;
		}
		assertTrue(couldRename);

		Thread.sleep(200);
		SingleWatchKeyTaker swkt1 = new SingleWatchKeyTaker(watcher);
		swkt1.start();

		swkt1.join(200);

		Assert.assertFalse(swkt1.isAlive());
		Assert.assertTrue(swkt1.watchKey == key1);
		Assert.assertTrue(listContainsEvent(swkt1.events, ENTRY_DELETE, "test1-watched"));
		Assert.assertTrue(listContainsEvent(swkt1.events, ENTRY_CREATE, "test2-watched"));
		key1.cancel();

		watcher.close();

	}


	@Test
	public void testRecursiveWatch() throws IOException, InterruptedException{
		WatchService service = Bootstrapper.newWatchService();

		// test non-recursive watch first: make sure that changes
		// to files in a child directory are ignored

		File dir = createTempDir("watched-dir");
		Path path = Bootstrapper.newPath(dir);
		File subdir = new File(dir, "subdir");
		subdir.mkdir();

		WatchKey key1 = path.register(service, StandardWatchEventKind.ENTRY_CREATE);

		File file1 = new File(subdir, "file1");
		file1.deleteOnExit();
		file1.createNewFile();

		// the file that has just been created must be ignored.
		WatchKey result1 = service.poll(200, TimeUnit.MILLISECONDS);
		assertNull(result1);

		WatchKey key2;
		try {
			WatchEvent.Kind[] eventKinds = { StandardWatchEventKind.ENTRY_CREATE };
			key2 = path.register(service, eventKinds, ExtendedWatchEventModifier.FILE_TREE);
		} catch (UnsupportedOperationException uox){
			// on platforms other than Windows, we expect an illegal argument
			// exception, because only Windows currently implements the
			// FILE_TREE modifier.
			if(!System.getProperty("os.name").contains("Windows") || Bootstrapper.isForcePollingEnabled())
				return;
			throw uox;
		}

		File file2 = new File(subdir, "file2");
		file2.deleteOnExit();
		file2.createNewFile();

		WatchKey result2 = service.poll(200, TimeUnit.MILLISECONDS);
		assertTrue(result2 == key2);

		service.close();
	}

	@Test
	public synchronized void testRegister() throws InterruptedException, IOException{
		WatchService service = Bootstrapper.newWatchService();

		File dir = createTempDir("watched-dir");
		Path path = Bootstrapper.newPath(dir);

		WatchKey key1 = path.register(service, StandardWatchEventKind.ENTRY_CREATE);

		File file = new File(dir, "file");
		file.createNewFile();

		wait(200);

		WatchKey resultKey1 = service.poll(200, TimeUnit.MILLISECONDS);
		assertTrue(resultKey1 == key1);
		List<WatchEvent<?>> list1 = resultKey1.pollEvents();
		assertTrue(listContainsEvent(list1, ENTRY_CREATE, "file"));
		assertTrue(resultKey1.reset());

		WatchKey key2 = path.register(service, ENTRY_CREATE, ENTRY_DELETE);

		assertTrue(key1 == key2);

		file.delete();

		WatchKey resultKey2 = service.poll(200, TimeUnit.MILLISECONDS);
		assertTrue(resultKey2 == key2);
		List<WatchEvent<?>> list2 = resultKey2.pollEvents();
		assertTrue(listContainsEvent(list2, ENTRY_DELETE, "file"));
		assertTrue(resultKey2.reset());

		service.close();
	}

	@Test
	public synchronized void testACCURATE() throws InterruptedException, IOException{
		WatchService service = Bootstrapper.newWatchService();

		File dir = createTempDir("watched-dir");
		Path path = Bootstrapper.newPath(dir);

		// ensure that selected platforms support ACCURATE
		boolean platformSupportsACCURATE =
			System.getProperty("os.name").contains("Windows")
		||	System.getProperty("os.name").contains("Linux");
		WatchKey key1;
		try {
			key1 = path.register(service, new WatchEvent.Kind[]{StandardWatchEventKind.ENTRY_CREATE}, ExtendedWatchEventModifier.ACCURATE);
		}catch(UnsupportedOperationException uox){
			// implementation reports that ACCURATE is not supported,
			// so ensure that that's true:
			assertFalse(platformSupportsACCURATE);
			return;
		}

		File file = new File(dir, "file");
		file.createNewFile();
		file.delete();

		wait(200);

		WatchKey resultKey1 = service.poll(200, TimeUnit.MILLISECONDS);
		List<WatchEvent<?>> list1 = resultKey1.pollEvents();
		assertTrue(resultKey1 == key1);
		assertTrue(listContainsEvent(list1, ENTRY_CREATE, "file"));
		assertFalse(listContainsEvent(list1, ENTRY_DELETE, "file"));
		assertTrue(resultKey1.reset());

		service.close();
	}

	@Test
	public synchronized void testReset() throws IOException, InterruptedException{
		WatchService service = FileSystems.getDefault().newWatchService();

		File dir = createTempDir("testReset");
		Path path = Paths.get(dir.toString());

		WatchKey key = path.register(service, ENTRY_CREATE);

		// this event should now be picked up by the WatchService
		File file1 = new File(dir, "file1");
		file1.createNewFile();

		WatchKey k = service.poll(200, TimeUnit.MILLISECONDS);
		assertTrue(k == key);

		List<WatchEvent<?>> list = k.pollEvents();
		assertTrue(listContainsEvent(list, ENTRY_CREATE, "file1"));

		// THIS IS THE IMPORTANT PART:
		// before we call reset, we wait a wee bit. Then we create another file,
		// wait a wee bit more so that the event can be delivered to the key,
		// and then call reset. poll() should now return the key immediately
		// (because it has events queued up)
		wait(200);
		File file2  = new File(dir, "file2");
		file2.createNewFile();
		wait(200);

		boolean resetResult = k.reset();
		assertTrue(resetResult);

		// poll should return the key now
		k = service.poll();
		assertTrue(k == key);

		list = k.pollEvents();
		assertTrue(listContainsEvent(list, ENTRY_CREATE, "file2"));

		service.close();
	}

	@Test
	public synchronized void testInvalidationByCancel() throws IOException, InterruptedException{
		// test for KEY_INVALID events in case the user manually cancels
		// the watch key by calling WatchKey.cancel().
		WatchService service = FileSystems.getDefault().newWatchService();

		File dir = createTempDir("testInvalidationByCancel");
		Path path = Paths.get(dir.toString());

		WatchKey key = path.register(service, ENTRY_CREATE, KEY_INVALID);

		// invalidation by user
		key.cancel();

		assertFalse(key.isValid());

		// Because we're listening for KEY_INVALID events, the key still
		// needs to be queued
		WatchKey k = service.poll();
		assertSame(key, k);

		List<WatchEvent<?>> events = k.pollEvents();
		assertTrue(events.size()==1);
		assertTrue(listContainsEvent(events, KEY_INVALID, null));

		service.close();
	}


	@Test
	public synchronized void testInvalidationByDelete() throws IOException, InterruptedException{
		// test for KEY_INVALID events in case the Path for which they
		// key was registered is removed
		WatchService service = FileSystems.getDefault().newWatchService();

		File dir = createTempDir("testInvalidationByDelete");
		Path path = Paths.get(dir.toString());

		WatchKey key = path.register(service, ENTRY_CREATE, KEY_INVALID);

		// invalidation by deleting the watched path
		boolean deleted = dir.delete();
		assertTrue(deleted);	// if delete fails, we can't test

		wait(500);	// wait a bit so that the deletion can propagate
		// (Windows appears to be specifically slow here...)

		// Because we're listening for KEY_INVALID events, the key still
		// should still be queued
		WatchKey k = service.poll();
		assertSame(key, k);

		// key should now be invalid
		assertFalse(key.isValid());

		List<WatchEvent<?>> events = k.pollEvents();
		assertTrue(events.size()==1);
		assertTrue(listContainsEvent(events, KEY_INVALID, null));

		service.close();
	}

	@Test
	public synchronized void testMODIFY() throws IOException, InterruptedException{
		// test for ENTRY_MODIFY events.
		// The test is taylored for kernel event driven WatchServices as well
		// as for poll driven ones (like all other tests).
		//
		// We test two things:
		// * changes that affect the file size
		// * changes that affect the modtime
		// * file creation and modification right after register()
		//
		// Polling implementations have a couple of challenges to face here:
		// Often systems store modtime as seconds from the epoch,
		// which means that there can be a case where a write occurs that does
		// not change modtime:
		// Assume you write to a file, call register() and then write again.
		// The first write sets modtime to 12:00:01, but register and the
		// second write follow so quickly that they still happen in the same
		// second - so the modtime is still 12:00:01. If the file size does
		// not change as well, the change is undetectable by a polling
		// implementation.
		// Another case is when you create a file AFTER calling register and then
		// modify it:
		// Assume you call register on a directory, then create a file and
		// write to it straight after: Creating the file sets it's modtime to
		// 10:00:01, and the write happens so quick it's still set to 10:00:01.
		// But because we know that the file has been created and therefore its
		// initial size can only be 0, if it's different from that we can assume
		// it has been written to as well, and therefore expect a modification
		// event to be detected by a polling implementation.
		WatchService service = FileSystems.getDefault().newWatchService();

		File dir = createTempDir("testMODIFY");
		Path path = Paths.get(dir.toString());

		// first, we create a new file (size: 0 bytes)
		File file1 = new File(dir, "file1");
		file1.createNewFile();

		// then, register() the key. Polling implementations will take a
		// snapshot here, non-pollers will receive modification events
		// hereafter - so we can expect modifications to be detected that happen
		// after calling register().
		WatchKey key = path.register(service, ENTRY_MODIFY);

		// modify file - here: write one byte to it.
		FileOutputStream fos = new FileOutputStream(file1);
		fos.write(1);
		fos.close();

		wait(100);	// wait a bit here, as for event driven WatchService
			// implementations it might take a bit for the event to propagate
			// through the kernel (100ms is very generous here)

		// get key with events
		WatchKey k = service.poll();

		// we expect an event to be reported by poll().
		assertNotNull(k);
		List<WatchEvent<?>> elist = k.pollEvents();
		assertTrue(listContainsEvent(elist, ENTRY_MODIFY, "file1"));
		k.reset();

		// wait more than a second here, to allow for the modtime to be
		// different when writing to the file again (for pollers)
		wait(1100);

		// write one byte to the file; this will not change the file size, but
		// should change the modification date, so pollers can pick it up too.
		fos = new FileOutputStream(file1);
		fos.write(1);
		fos.close();

		// wait a bit for event driven WatchService implementations
			// to allow events trickling through the kernel
		k = service.poll(100, TimeUnit.MILLISECONDS);

		// again, we expect an event to be reported by poll().
		assertNotNull(k);
		elist = k.pollEvents();
		assertTrue(listContainsEvent(elist, ENTRY_MODIFY, "file1"));
		k.reset();

		// now create an entirely new file and write to it straight away.
		// pollers should pick this up too.
		File file2 = new File(dir, "file2");
		file2.createNewFile();
		fos = new FileOutputStream(file2);
		fos.write(1);
		fos.close();

		// wait a bit for event driven WatchService implementations
			// to allow events trickling through the kernel
		k = service.poll(100, TimeUnit.MILLISECONDS);

		// we expect this modification to be picked up too. Pollers need to
		// take special care here, because the creation event may not shadow
		// the modification that has taken place too.
		assertNotNull(k);
		elist = k.pollEvents();
		assertTrue(listContainsEvent(elist, ENTRY_MODIFY, "file2"));
		k.reset();

		// try again with writing to the file, this time reading from the
		// service with take()
		fos = new FileOutputStream(file2);
		fos.write(new byte[]{1,3,4});
		fos.close();

		k = service.take();

		assertNotNull(k);
		elist = k.pollEvents();
		assertTrue(listContainsEvent(elist, ENTRY_MODIFY, "file2"));
		k.reset();

		service.close();
	}

	@Test(timeout=15000)
	public synchronized void testManyDirectories() throws IOException, InterruptedException{
		// this is a basic test which attempts to create a large number of
		// monitored directories. In each directory, a file is created, and
		// the tests ensures that this creation event is picked up by all
		// registered watch keys.
		// Note that the main reason to implement this test is to make sure
		// that the Windows implementation can handle more than 63 directories
		// (which is a limit imposed by the WaitForMultipleObjects() win32
		// kernel function). The Windows implementation must find a way around
		// this limit.
		// Note that there will be limits like this on every platform; we won't
		// test for them here at this stage.
		int numDirectories = 200;

		File parent = createTempDir("testManyDirectories");

		WatchService ws = FileSystems.getDefault().newWatchService();
		Map<WatchKey, Integer> createdFiles = new HashMap();

		for(int n=0; n<numDirectories; ++n){
			File dir = new File(parent, Integer.toString(n));
			dir.mkdir();
			dir.deleteOnExit();
			Path path = Paths.get(dir.getAbsolutePath());
			WatchKey key = path.register(ws, ENTRY_CREATE);

			File file = new File(dir, "file");
			boolean created = file.createNewFile();

			assert(created);

			createdFiles.put(key, n);
		}

		wait(1000);

		// check that we saw all created files
		WatchKey k = ws.poll();
		while(k != null){
			List<WatchEvent<?>> elist = k.pollEvents();
			assert(listContainsEvent(elist, ENTRY_CREATE, "file"));

			assert(createdFiles.containsKey(k));
			createdFiles.remove(k);

			k = ws.poll();
		}

		assert(createdFiles.isEmpty());

		// shut down
		ws.close();

		for(int n=0; n<numDirectories; ++n){
			File dir = new File(parent, Integer.toString(n));
			File file = new File(dir, "file");
			file.delete();

			dir.delete();
		}

	}

	@Test
	public void testWatchNonAsciiPath() throws IOException{
		File parent = createTempDir("testManyDirectories");

		WatchService ws = FileSystems.getDefault().newWatchService();

		File unicodeFile = new File(parent, "test\u001feb");
		unicodeFile.mkdir();

		Path path = Paths.get(unicodeFile.getAbsolutePath());

		WatchKey k = path.register(ws, ENTRY_CREATE);
		assert(k != null);

		ws.close();
	}


	@Test
	public synchronized void testOverflow() throws IOException, InterruptedException{
		// won't perform test for poller fallback; it has an unlimited
		// queue and will therefore never report overflow...
		if(Bootstrapper.isForcePollingEnabled())
			return;

		// BSD based platforms use kevent(), which provides so little that
		// all we can do is to poll (at least when know when to poll, so it's
		// better than nothing).
		// But this means there cannot be overflow.
                // This cannot reliably be reproduced in Windows.  However,
                // we still get overflow during testRecursiveWatch on Windows.
		String osname = System.getProperty("os.name");
		if(osname.contains("Mac OS X") || osname.contains("FreeBSD") || osname.contains("Windows"))
			return;

		// aim to provoke overflow event by giving this thread maximum priority
		// and dumping thousands of files into a folder without reading events.
		// some platform
		//

		File parent = createTempDir("testOverflow");

		WatchService ws = FileSystems.getDefault().newWatchService();
		Path p = Paths.get(parent.getAbsolutePath());

		WatchKey k = p.register(ws, ENTRY_CREATE, ENTRY_DELETE);

		// we have a key. now hammer the directory with new files; but give
		// the current thread max priority so that the chance is higher to
		// starve any thread that the WatchService is running (like the
		// WindowsPathWatchService does)

		Thread currentThread = Thread.currentThread();
		int initialPriority = currentThread.getPriority();

		final int numFiles = 30000;	// this is insanely high; but aims to ensure we're actually hitting the queue limit
		ArrayList<File> files = new ArrayList(numFiles);

		try{
			currentThread.setPriority(Thread.MAX_PRIORITY);


			for(int i=0; i<numFiles; ++i){
				String fileName = String.format("%05d", i);
				files.add(new File(parent, fileName));
			}

			for(File f : files)
				f.createNewFile();

			// allow events to propagate via kernel to the WatchService
			wait(200);

			// we might not be able to read all events at once, so we do
			// what every client code should be doing: loop around
			// take() or poll():
			WatchKey resultKey = null;
			boolean overflowDetected = false;
			int numEvents = 0;
			do{
				resultKey = ws.poll();
				if(resultKey != null){
					List<WatchEvent<?>> list = resultKey.pollEvents();
					numEvents += list.size();
					if(!overflowDetected)
						overflowDetected = listContainsEvent(list, OVERFLOW, null);
					resultKey.reset();
				}
			}while(!overflowDetected && resultKey != null);

			assertTrue(overflowDetected);
		} finally {
			// reset priority
			currentThread.setPriority(initialPriority);

			// clean up files
			for(File f : files)
				f.delete();
		}
	}

	@Test
	public void testCancel() throws IOException, InterruptedException{
		// NOTE: this test case was written specifically with the BSDPathWatchService
		// in mind, which is why it might do things that aren't too straight-forward.
		// However, it must hold for all platforms.

		// WatchKey.cancel() is supposed to invalidate the key. Therefore,
		// the following conditions must hold:
		// * (1)prior to calling WatchKey.cancel()
		//	 - (1.1)WatchKey.isValid()==true
		//   - (1.2)a second call to Path.register() for the same path should
		//     return the same key
		// * (2)after calling to calling WatchKey.cancel()
		//   - (2.1)WatchKey.isValid()==false
		//   - (2.2)a second call to Path.register() for the same path returns a
		//     DIFFERENT key
		// Note that these conditions must also hold if there are other keys
		// registered
		File parent = createTempDir("testCancel");
		File modifiedFile = new File(parent, "modifiedFile");

		//NOTE: instead of using the line below (calling File.createNewFile),
		// we use a new FileOutputStream that we immediately close to create
		// the file. The Reason: When debugging, File.createNewFile() apparently
		// didn't create the file, even though it claimed it did. JDK bug?
		// The whole episode only seems to happen in debug..
		//
		//assert(modifiedFile.createNewFile());
		new FileOutputStream(modifiedFile).close();
		assert(modifiedFile.exists());

		File otherWatchedDir = createTempDir("testCancel_background");

		WatchService ws = FileSystems.getDefault().newWatchService();
		Path p = Paths.get(parent.getAbsolutePath());

		// register background key, which is only there so that the watch service
		// doesn't run completely empty if we cancel the main key we use for
		// testing. Otherwise, we ignore the key (this is why we don't store it)
		Paths.get(otherWatchedDir.getAbsolutePath()).register(ws, ENTRY_CREATE, ENTRY_MODIFY);

		WatchKey k1 = p.register(ws, ENTRY_CREATE, ENTRY_DELETE, ENTRY_MODIFY);
		// check for (1.1)assert
		assert(k1.isValid());

		{
			Path p2 = Paths.get(parent.getAbsolutePath());
			WatchKey k0 = p2.register(ws, ENTRY_CREATE, ENTRY_DELETE, ENTRY_MODIFY);
			assert(k0 == k1); // check for (1.2)
		}

		appendBytesToFile(modifiedFile, new byte[1]);

		WatchKey k = ws.poll(200, TimeUnit.MILLISECONDS);
		assert(k == k1);

		// poll events and discard them
		List<WatchEvent<?>> events = k.pollEvents();
		assertTrue(events.size()==1);
		assertTrue(listContainsEvent(events, ENTRY_MODIFY, modifiedFile.getName()));

		k.reset();

		// CANCELLING KEY
		k.cancel();

		// check for (2.1)
		assert(!k.isValid());

		appendBytesToFile(modifiedFile, new byte[1]);
		k = ws.poll(200, TimeUnit.MILLISECONDS);
		assert(k == null);

		// check for (2.2)
		WatchKey k2 = p.register(ws, ENTRY_CREATE, ENTRY_DELETE, ENTRY_MODIFY);
		assert(k1 != k2);

		appendBytesToFile(modifiedFile, new byte[1]);

		k = ws.poll(200, TimeUnit.MILLISECONDS);
		assert(k == k2);
		k.reset();

		ws.close();

	}

}
