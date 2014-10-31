#ifndef C__t_FunctorMap__H
#define C__t_FunctorMap__H

#include <util/EnvironmentStd.h>

template <class T>
class t_FunctorMap {
public:
	t_FunctorMap(void);
	~t_FunctorMap(void);

	virtual void initialize_map() = 0;
	void insert( string, T*& );
	T*& getInstance(string);

	map<string,T*&> enum_map;
	
};

#endif

