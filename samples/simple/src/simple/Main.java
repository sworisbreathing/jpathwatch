package simple;

import java.io.IOException;
import java.util.List;
import name.pachler.nio.file.*;

public class Main {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        WatchService watchService = FileSystems.getDefault().newWatchService();

		String tempDir = System.getProperty("java.io.tmpdir");
		Path path = Paths.get(tempDir);

		System.out.println("using tempdir: " + path.toString());

		WatchKey key = null;
		try {
			key = path.register(watchService, StandardWatchEventKind.ENTRY_CREATE, StandardWatchEventKind.ENTRY_DELETE);
		} catch (UnsupportedOperationException uox){
			System.err.println("file watching not supported!");
			// handle this error here
		} catch (IOException iox){
			System.err.println("I/O errors");
			// handle this error here
		}

		// typically, real world applications will run this loop in a
		// separate thread and signal directory changes to another thread
		// (often, this will be Swing's event dispatcher thread; use
		// SwingUtilities.invokeLater())
		for(;;){
			// take() will block until a file has been created/deleted
			WatchKey signalledKey = null;
			try {
				signalledKey = watchService.take();
			} catch (InterruptedException ix){
				// we'll ignore being interrupted
				continue;
			} catch (ClosedWatchServiceException cwse){
				// other thread closed watch service
				System.out.println("watch service closed, terminating.");
				break;
			}

			// get list of events from key
			List<WatchEvent<?>> list = signalledKey.pollEvents();

			// VERY IMPORTANT! call reset() AFTER pollEvents() to allow the
			// key to be reported again by the watch service
			signalledKey.reset();

			// we'll simply print what has happened; real applications
			// will do something more sensible here
			for(WatchEvent e : list){
				String message = "";
				if(e.kind() == StandardWatchEventKind.ENTRY_CREATE){
					Path context = (Path)e.context();
					message = context.toString() + " created";
				} else if(e.kind() == StandardWatchEventKind.ENTRY_DELETE){
					Path context = (Path)e.context();
					message = context.toString() + " deleted";
				} else if(e.kind() == StandardWatchEventKind.OVERFLOW){
					message = "OVERFLOW: more changes happened than we could retreive";
				}
				System.out.println(message);
			}
		}


    }

}
