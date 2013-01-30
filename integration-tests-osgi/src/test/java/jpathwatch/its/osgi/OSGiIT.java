/*
 * Copyright 2012 Steven Swor.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
package jpathwatch.its.osgi;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import name.pachler.nio.file.FileSystems;
import name.pachler.nio.file.Path;
import name.pachler.nio.file.Paths;
import name.pachler.nio.file.StandardWatchEventKind;
import name.pachler.nio.file.WatchEvent;
import name.pachler.nio.file.WatchEvent.Kind;
import name.pachler.nio.file.WatchKey;
import name.pachler.nio.file.WatchService;
import name.pachler.nio.file.ext.Bootstrapper;
import org.junit.After;
import static org.junit.Assert.*;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.ops4j.pax.exam.Constants;
import static org.ops4j.pax.exam.CoreOptions.*;
import org.ops4j.pax.exam.Option;
import org.ops4j.pax.exam.junit.Configuration;
import org.ops4j.pax.exam.junit.JUnit4TestRunner;
import org.ops4j.pax.exam.util.PathUtils;
import org.slf4j.LoggerFactory;

/**
 * OSGi Integration Tests.
 *
 * @author sswor
 */
@RunWith(JUnit4TestRunner.class)
public class OSGiIT {

    /**
     * Gets the file which was the source of a file system event.
     *
     * @param watchService the watch service
     * @param kind the event kind
     * @param timeout the polling timeout
     * @param timeUnit the polling time unit
     * @param parent the parent path, used to resolve the file's absolute path
     * @return the file which was the source of a file system event of the
     * specified kind
     * @throws TimeoutException if no such event is detected within the polling
     * timeout
     * @throws InterruptedException if the thread is interrupted while polling
     */
    private static File getEventFile(final WatchService watchService, final Kind kind, final long timeout, final TimeUnit timeUnit, final Path parent) throws TimeoutException, InterruptedException {
        File results = null;
        WatchKey key = watchService.poll(timeout, timeUnit);
        if (key != null) {
            try {
                List<WatchEvent<?>> events = key.pollEvents();
                for (WatchEvent event : events) {
                    if (kind.name().equals(event.kind().name())) {
                        Object context = event.context();
                        if (context instanceof Path) {
                            Path path = (Path) event.context();
                            path = parent.resolve(path);
                            results = Bootstrapper.pathToFile(path);
                            break;
                        }
                    }
                }
            } finally {
                key.reset();
            }
        } else {
            throw new TimeoutException("Did not detect event");
        }
        return results;
    }

    /**
     * Creates a temporary folder for testing purposes.
     *
     * @param prefix the folder prefix
     * @return a temporary folder whose name begins with the specified prefix
     * @throws IOException if the folder cannot be created
     */
    private static File createTempDir(String prefix) throws IOException {
        File file = File.createTempFile(prefix, "");
        file.deleteOnExit();
        File dir = new File(file.getParentFile(), file.getName() + ".dir");
        dir.mkdir();
        dir.deleteOnExit();
        return dir;
    }

    /**
     * Appends data to a file.
     * @param file the file to be appended
     * @param bytes the data to append
     * @throws IOException if the data cannot be appended to the file
     */
    private static void appendBytesToFile(final File file, final byte[] bytes) throws IOException {
        FileOutputStream fileOut = null;
        try {
            fileOut = new FileOutputStream(file, true);
            fileOut.write(bytes);
        } finally {
            if (fileOut != null) {
                try {
                    fileOut.close();
                } catch (Exception ex) {
                    //trap.  We'll have bigger fish to fry if this happens
                }
            }
        }
    }

    /**
     * The watch service we are testing.
     */
    private WatchService watchService;

    /**
     * Gets the Pax Exam configuration for the OSGi tests.
     *
     * @return the Pax Exam configuration to run the tests
     */
    @Configuration
    public Option[] config() {
        return options(
                systemProperty("org.ops4j.pax.logging.DefaultServiceLog.level").value("NONE"),
                systemProperty("logback.configurationFile").value("file:" + PathUtils.getBaseDir() + "/src/test/resources/logback.groovy"),
                // copy most options from PaxExamRuntime.defaultTestSystemOptions(),
                // except RBC and Pax Logging.  This is so we can use slf4j/logback instead of JUL
                bootDelegationPackage("sun.*"),
                frameworkStartLevel(Constants.START_LEVEL_TEST_BUNDLE),
                url("link:classpath:META-INF/links/org.ops4j.pax.exam.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.ops4j.pax.exam.inject.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.ops4j.pax.extender.service.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.osgi.compendium.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.ops4j.base.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.ops4j.pax.swissbox.core.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.ops4j.pax.swissbox.extender.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.ops4j.pax.swissbox.framework.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.ops4j.pax.swissbox.lifecycle.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                url("link:classpath:META-INF/links/org.apache.geronimo.specs.atinject.link").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                mavenBundle("org.slf4j", "slf4j-api", "1.7.2").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                mavenBundle("org.codehaus.groovy", "groovy-all", "2.0.5").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                mavenBundle("ch.qos.logback", "logback-core", "1.0.9").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                mavenBundle("ch.qos.logback", "logback-classic", "1.0.9").startLevel(Constants.START_LEVEL_SYSTEM_BUNDLES),
                mavenBundle("org.apache.felix", "org.apache.felix.configadmin", "1.2.4"),
                mavenBundle("org.apache.aries", "org.apache.aries.util", "1.0.0"),
                mavenBundle("org.apache.aries.proxy", "org.apache.aries.proxy", "1.0.0"),
                mavenBundle("jpathwatch", "jpathwatch-bundle", "0.96-SNAPSHOT"),
                junitBundles());
    }

    /**
     * Initializes the watch service before running the tests.
     */
    @Before
    public void setUp() {
        watchService = FileSystems.getDefault().newWatchService();
    }

    /**
     * Shuts down the watch service after running the tests.
     */
    @After
    public void tearDown() {
        if (watchService != null) {
            try {
                watchService.close();
            } catch (IOException ex) {
                //trap
            }
        }
        watchService = null;
    }

    /**
     * Gets the maximum timeout used when polling for events.
     *
     * @return the maximum timeout used when polling for events
     */
    protected long eventTimeoutDuration() {
        return 2L;
    }

    /**
     * Gets the time unit for the maximum timeout used when polling for events.
     *
     * @return the time unit for the maximum timeout used when polling for
     * events
     */
    protected TimeUnit eventTimeoutTimeUnit() {
        return TimeUnit.MINUTES;
    }

    /**
     * Runs the file system monitoring tests in the OSGi container.
     *
     * @throws Throwable if the test fails
     */
    @Test
    public void testFileMonitoring() throws Throwable {
        final org.slf4j.Logger logger = LoggerFactory.getLogger(getClass());
        /*
         * Start watching a folder.
         */
        File folder = createTempDir("testOSGi");
        Path folderPath = Paths.get(folder.getAbsolutePath());
        folderPath.register(watchService, StandardWatchEventKind.ENTRY_CREATE, StandardWatchEventKind.ENTRY_MODIFY, StandardWatchEventKind.ENTRY_DELETE);

        /*
         * Create a new file in that folder and verify that we got a created
         * event for that file.
         */
        File newFile = new File(folder, "testFile");
        logger.debug("Test file: {}", newFile.getAbsolutePath());
        newFile.createNewFile();
        newFile.deleteOnExit(); // in case the test fails
        logger.info("Testing for created event.");
        File created = getEventFile(watchService, StandardWatchEventKind.ENTRY_CREATE, eventTimeoutDuration(), eventTimeoutTimeUnit(), folderPath);
        assertNotNull(created);
        assertEquals(newFile.getAbsolutePath(), created.getAbsolutePath());

        /*
         * Write to the file and verify that we got a modification event for
         * that file.
         */
        logger.info("Testing for modification event.");
        appendBytesToFile(newFile, new byte[4096]);
        File modified = getEventFile(watchService, StandardWatchEventKind.ENTRY_MODIFY, eventTimeoutDuration(), eventTimeoutTimeUnit(), folderPath);
        assertNotNull(modified);
        assertEquals(newFile.getAbsolutePath(), modified.getAbsolutePath());

        /*
         * Delete the file and verify that we got a deletion event for that
         * file.
         */
        newFile.delete();
        logger.info("Testing for deletion event.");
        File deleted = getEventFile(watchService, StandardWatchEventKind.ENTRY_DELETE, eventTimeoutDuration(), eventTimeoutTimeUnit(), folderPath);
        assertNotNull(deleted);
        assertEquals(newFile.getAbsolutePath(), deleted.getAbsolutePath());
    }
}
