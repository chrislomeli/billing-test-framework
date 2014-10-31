#ifndef C__PAY_TYPE__H
#define C__PAY_TYPE__H

#include <util/EnvironmentStd.h>
#include <util/EnvironmentPortal.h>
#include <ctype/c_EnumMap.h>


class c_PayType : public c_EnumMap
{
public:
	c_PayType(void) {};
	~c_PayType(void) {};

	void initialize_map ();
	pin_fld_num_t getPayinfoArrayType(void);
};
#endif

