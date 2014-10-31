#ifndef CCC_FIELD__HH
#define CCC_FIELD__HH

#include <util/EnvironmentStd.h>


template <class T> 
class CField  
{
public:
	CField() : b_empty(true) {};
	CField(T in) : b_empty(false) { set(in); };
	CField(string _name, T _value) : b_empty(false) { set(_name, _value); };

	void set( string _name, T _value ) { 
		this->value = _value; 
		this->name = _name;
		b_empty=false; 
	};
	
	void set( T in ) { 
		this->value = in; 
		b_empty=false; 
	};
	
	T& get() { 
		if (this->empty()) 
			throw range_error("value not set"); 
		return value; 
	};
	
	string& Name() { 
		if (this->empty()) 
			throw range_error("value not set"); 
		return name; 
	};

	T& Value() { 
		if (this->empty()) 
			throw range_error("value not set"); 
		return value; 
	};

	bool empty() { 
		return b_empty; 
	}

	void clear() {
		b_empty = true;
	}
	
private:
	string name;
	T value;
	bool b_empty;

};

#endif


