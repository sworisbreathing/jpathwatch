/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package watchlog;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Vector;
import javax.swing.table.AbstractTableModel;

/**
 *
 * @author count
 */
class LogTableModel extends AbstractTableModel {
	public static final int CREATED = 1;
	public static final int DELETED = 2;
	public static final int MODIFIED = 3;
	
	private SimpleDateFormat formatter = new SimpleDateFormat();
	private static class Record{
		long time;
		int change;
		String path;

		private Record(long time, int change, String path) {
			this.time = time;
			this.change = change;
			this.path = path;
		}
	}
	private Vector<Record> store = new Vector<Record>();

	public int getRowCount() {
		return store.size();
	}

	public int getColumnCount() {
		return 3;
	}

	public Object getValueAt(int rowIndex, int columnIndex) {
		Record r = store.elementAt(rowIndex);
		switch(columnIndex){
			case 0:
				return formatter.format(new Date(r.time));
			case 1:
				return changeToString(r.change);
			case 2:
				return r.path;
		}
		return null;
			
	}

	void addRecord(long time, int change, File file) {
		store.add(new Record(time, change, file.toString()));
		this.fireTableRowsInserted(store.size()-1, store.size()-1);
	}

	void clear() {
		int numRows = store.size();
		store.clear();
		if(numRows > 0)
			this.fireTableRowsDeleted(0, numRows-1);
	}

	private String changeToString(int change) {
		switch(change){
			case DELETED:
				return "delete";
			case MODIFIED:
				return "modified";
			case CREATED:
				return "created";
		}
		return null;
	}
	
}
