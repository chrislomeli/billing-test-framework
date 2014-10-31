#ifndef C__BILL_TYPE__H
#define C__BILL_TYPE__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <ctype/c_EnumMap.h>


class c_BillType : public c_EnumMap
{
public:
	c_BillType(void) {};
	~c_BillType(void) {};

	void initialize_map ();
	pin_fld_num_t getPayinfoArrayType(void);
};
#endif

