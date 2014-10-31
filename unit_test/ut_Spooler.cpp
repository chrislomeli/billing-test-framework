#include <unit_test/ut_Spooler.h>

ut_Spooler::ut_Spooler(void)
{
}

ut_Spooler::~ut_Spooler(void)
{
}

void ut_Spooler::create_file(string dir, string file, string contents )
{
	ofstream pend;
	f_FileName f;
	f.setFileName(dir,file);
	pend.open(f.full_path.c_str());
	if (!pend){
		cout << "Can not open file ..." << f.full_path << endl;
		return;
	}
	pend << "Junk|junk|junk" << endl;
	pend.close();

}


void ut_Spooler::test_two(ostream& ut_out)
{  
	char * cp=NULL;
	f_Spooler spool;

	ut_out << "\n========================================================\n";
	ut_out << "\nTEST TWO\n";
	ut_out << "\n========================================================\n";
	ut_out << "\n/////////////////CONFIGS/////////////////////////////\n";
	
	string root("c:\\temp\\test2");
	string pendir("c:\\temp\\test2");
	string spooldir("c:\\temp\\test2");
	string backupdir("c:\\temp\\test2");
	string processdir("c:\\temp\\test2");
	string suspensedir("c:\\temp\\test2");
	
	spool.setSpoolConfig(Pending, pendir, "in", "", ""); 
	spool.setSpoolConfig(Spool, spooldir, "wrk", "", "");
	spool.setSpoolConfig(Backup, backupdir, "bak", "", "");
	spool.setSpoolConfig(Processed, processdir, "done", "", "");
	spool.setSpoolConfig(Suspense, suspensedir, "try", "", "");
	spool.setSpoolConfig(Error, suspensedir, "errs", "", "");
	spool.dumpConfigs(ut_out);

///----create the pending dir------
	if (0 != f_File::mkdirRecursive(pendir))
	{	
		ut_out << "Can not create recursive dir: " << pendir << endl;
		return;
	};

///----create a pending file------
	create_file(pendir,"input001.in","Junk|Junk|Junk\n");
	create_file(pendir,"input002.in","Junk|Junk|Junk\n");
	create_file(pendir,"input003.in","Junk|Junk|Junk\n");
	create_file(pendir,"input004.in","Junk|Junk|Junk\n");
	create_file(pendir,"input005.in","Junk|Junk|Junk\n");
	create_file(pendir,"input006.in","Junk|Junk|Junk\n");
	create_file(pendir,"input007.in","Junk|Junk|Junk\n");
	create_file(pendir,"input008.in","Junk|Junk|Junk\n");
	create_file(pendir,"input009.in","Junk|Junk|Junk\n");
	create_file(pendir,"input011.in","Junk|Junk|Junk\n");
	create_file(pendir,"input012.in","Junk|Junk|Junk\n");
	create_file(pendir,"input013.in","Junk|Junk|Junk\n");
	create_file(pendir,"input014.in","Junk|Junk|Junk\n");
	create_file(pendir,"input015.in","Junk|Junk|Junk\n");
	create_file(pendir,"input016.in","Junk|Junk|Junk\n");
	create_file(pendir,"input017.in","Junk|Junk|Junk\n");
	create_file(pendir,"input018.in","Junk|Junk|Junk\n");
	create_file(pendir,"input019.in","Junk|Junk|Junk\n");
	create_file(pendir,"input002.junk","Junk|Junk|Junk\n");

	ut_out << "\n/////////////////PENDING/////////////////////////////\n";
	f_Dir dir0;
	spool.dumpDirectory(&dir0,root,ut_out);

	ut_out << "\n/////////////////SPOOLED/////////////////////////////\n";
	spool.processDirectory(Pending);
	f_Dir dir1;
	spool.dumpDirectory(&dir1,root,ut_out);

	ut_out << "\n/////////////////PROCESSED/////////////////////////////\n";
	spool.processDirectory(Spool);
	f_Dir dir2;
	spool.dumpDirectory(&dir2,root,ut_out);

	return;
}

void ut_Spooler::test_one(ostream& ut_out)
{  
	char * cp=NULL;
	f_Spooler spool;


	ut_out << "\n========================================================\n";
	ut_out << "\nTEST ONE\n";
	ut_out << "\n========================================================\n";
	ut_out << "\n/////////////////CONFIGS/////////////////////////////\n";
	
	string root("c:\\temp\\test1");
	string pendir("c:\\temp\\test1\\pending");
	string spooldir("c:\\temp\\test1\\spool");
	string backupdir("c:\\temp\\test1\\backup");
	string processdir("c:\\temp\\test1\\processed");
	string suspensedir("c:\\temp\\test1\\suspense");
	
	spool.setSpoolConfig(Pending, pendir, "in", "", ""); 
	spool.setSpoolConfig(Spool, spooldir, "wrk", "YYYYMMDD", "");
	spool.setSpoolConfig(Backup, backupdir, "bak", "", "YYYY/MM/DD");
	spool.setSpoolConfig(Processed, processdir, "done", "", "YYYY/MM/DD");
	spool.setSpoolConfig(Suspense, suspensedir, "try", "", "");
	spool.setSpoolConfig(Error, suspensedir, "err", "", "");
	spool.dumpConfigs(ut_out);

///----create the pending dir------
	if (0 != f_File::mkdirRecursive(pendir))
	{	
		ut_out << "Can not create recursive dir: " << pendir << endl;
		return;
	};

///----create a pending file------
	create_file(pendir,"input001.in","Junk|Junk|Junk\n");
	create_file(pendir,"input002.in","Junk|Junk|Junk\n");
	create_file(pendir,"input003.in","Junk|Junk|Junk\n");
	create_file(pendir,"input004.in","Junk|Junk|Junk\n");
	create_file(pendir,"input005.in","Junk|Junk|Junk\n");
	create_file(pendir,"input006.in","Junk|Junk|Junk\n");
	create_file(pendir,"input007.in","Junk|Junk|Junk\n");
	create_file(pendir,"input008.in","Junk|Junk|Junk\n");
	create_file(pendir,"input009.in","Junk|Junk|Junk\n");
	create_file(pendir,"input011.in","Junk|Junk|Junk\n");
	create_file(pendir,"input012.in","Junk|Junk|Junk\n");
	create_file(pendir,"input013.in","Junk|Junk|Junk\n");
	create_file(pendir,"input014.in","Junk|Junk|Junk\n");
	create_file(pendir,"input015.in","Junk|Junk|Junk\n");
	create_file(pendir,"input016.in","Junk|Junk|Junk\n");
	create_file(pendir,"input017.in","Junk|Junk|Junk\n");
	create_file(pendir,"input018.in","Junk|Junk|Junk\n");
	create_file(pendir,"input019.in","Junk|Junk|Junk\n");
	create_file(pendir,"input002.junk","Junk|Junk|Junk\n");

	ut_out << "\n/////////////////PENDING/////////////////////////////\n";
	f_Dir dir0;
	spool.dumpDirectory(&dir0,root,ut_out);

	ut_out << "\n/////////////////SPOOLED/////////////////////////////\n";
	spool.processDirectory(Pending);
	f_Dir dir1;
	spool.dumpDirectory(&dir1,root,ut_out);

	ut_out << "\n/////////////////PROCESSED/////////////////////////////\n";
	spool.processDirectory(Spool);
	f_Dir dir2;
	spool.dumpDirectory(&dir2,root,ut_out);

	return;
}

void ut_Spooler::test_three(ostream& ut_out)
{  
	char * cp=NULL;
	f_Spooler spool;

	ut_out << "\n========================================================\n";
	ut_out << "\nTEST THREE\n";
	ut_out << "\n========================================================\n";
///----------------
	ut_out << "\n/////////////////CONFIGS/////////////////////////////\n";
	
	string root("/temp/test3");
	string pendir("/temp/test3/pending");
	string spooldir("/temp/test3/spool");
	string backupdir("/temp/test3/backup");
	string processdir("/temp/test3/processed");
	string suspensedir("/temp/test3/suspense");
	
	spool.setSpoolConfig(Pending, pendir, "in", "", ""); 
	spool.setSpoolConfig(Spool, spooldir, "wrk", "YYYYMMDD", "");
	spool.setSpoolConfig(Backup, backupdir, "bak", "", "YYYY/MM/DD");
	spool.setSpoolConfig(Processed, processdir, "done", "", "YYYY/MM/DD");
	spool.setSpoolConfig(Suspense, suspensedir, "try", "", "");
	spool.setSpoolConfig(Error, suspensedir, "err", "", "");
	spool.dumpConfigs(ut_out);

///----create the pending dir------
	if (0 != f_File::mkdirRecursive(pendir))
	{	
		ut_out << "Can not create recursive dir: " << pendir << endl;
		return;
	};

///----create a pending file------
	create_file(pendir,"input001.in","Junk|Junk|Junk\n");
	create_file(pendir,"input002.in","Junk|Junk|Junk\n");
	create_file(pendir,"input003.in","Junk|Junk|Junk\n");
	create_file(pendir,"input004.in","Junk|Junk|Junk\n");
	create_file(pendir,"input005.in","Junk|Junk|Junk\n");
	create_file(pendir,"input006.in","Junk|Junk|Junk\n");
	create_file(pendir,"input007.in","Junk|Junk|Junk\n");
	create_file(pendir,"input008.in","Junk|Junk|Junk\n");
	create_file(pendir,"input009.in","Junk|Junk|Junk\n");
	create_file(pendir,"input011.in","Junk|Junk|Junk\n");
	create_file(pendir,"input012.in","Junk|Junk|Junk\n");
	create_file(pendir,"input013.in","Junk|Junk|Junk\n");
	create_file(pendir,"input014.in","Junk|Junk|Junk\n");
	create_file(pendir,"input015.in","Junk|Junk|Junk\n");
	create_file(pendir,"input016.in","Junk|Junk|Junk\n");
	create_file(pendir,"input017.in","Junk|Junk|Junk\n");
	create_file(pendir,"input018.in","Junk|Junk|Junk\n");
	create_file(pendir,"input019.in","Junk|Junk|Junk\n");
	create_file(pendir,"input002.junk","Junk|Junk|Junk\n");

	ut_out << "\n/////////////////PENDING/////////////////////////////\n";
	f_Dir dir0;
	spool.dumpDirectory(&dir0,root,ut_out);

	ut_out << "\n/////////////////SPOOLED/////////////////////////////\n";
	spool.processDirectory(Pending);
	f_Dir dir1;
	spool.dumpDirectory(&dir1,root,ut_out);

	ut_out << "\n/////////////////PROCESSED/////////////////////////////\n";
	spool.processDirectory(Spool);
	f_Dir dir2;
	spool.dumpDirectory(&dir2,root,ut_out);

	return;
}


void ut_Spooler::test_Driver()
{
	string filename = ut_Directory + "ut_Spooler.out";
	ofstream ut_out;

	ut_out.open(filename.c_str());
	if (!ut_out){
		cout << "Can not open file ..." << filename << endl;
		return;
	}
	test_one(ut_out);
	test_two(ut_out);
	test_three(ut_out);
	ut_out.close();
}