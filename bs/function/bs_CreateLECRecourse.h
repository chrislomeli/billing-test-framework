#ifndef C__bs_CreateLECRecourse__H
#define C__bs_CreateLECRecourse__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <pin/PinSession.h>
#include <ctype/c_NvRecord.h>
#include <ops/extension.h>


class bs_CreateLECRecourse
{
public:

  static bool createRecourse(
    pin_Session * pinp,
    c_NvRecord& nv,
    pin_flist_t   **return_flistpp,
    pin_errbuf_t  *ebufp  );


	static void createRecourse(
		pin_Session * pinp,
		const char * sRecordType,
		const char * sBtn,
		const char * sAuthDate,
		const char * sReasonCode,
		const char * sAmount,
		const char * sVersion,
		const char * file_name,
		pin_flist_t   **return_flistpp,
		pin_errbuf_t  *ebufp  );

};

#endif
