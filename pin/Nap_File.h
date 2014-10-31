#ifndef INT_CNap_File__HH__
#define INT_CNap_File__HH__

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>

enum file_status_t {
	End_of_file,
	Success,
	Bad_parse,
	End_recurse
};


class CNap_File 
{
public:
	CNap_File();
	~CNap_File(void);

	//--file2 flist routines---
	pin_flist_t * filetoFlist(string filename);

private:
	file_status_t recurse_textfile(
		FILE * fp,
		pin_flist_t * c_flistp, 
		int in_level );

	string filename;
	int list_ptr;
	long pos;
	int lineno ;
};

#endif
