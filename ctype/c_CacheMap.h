#ifndef C__c_CacheMap__H
#define C__c_CacheMap__H

#include <util/EnvironmentStd.h>

template <class t_MapValue>
class c_CacheMap
{
public:

	c_CacheMap(void) : is_initialized(false)
	{
		this->reset();
	}

	~c_CacheMap(void)
	{
		this->reset();
	}

	virtual bool isNull(t_MapValue) =0;
	virtual void initialize_map(void) =0;

	//--add------------
	void add(string name, t_MapValue value ) 
	{
		if (name.empty())
			return;

		string str = name;
		std::transform( str.begin(), str.end(), str.begin(), (int(*)(int))toupper);
		this->data[str] = value;
	};

	//-size------------------
	int size(void)
	{
		return (int)this->data.size();
	};


	//-size------------------
	t_MapValue  get( string instr )
	{
		if (this->data.size()==0)
			this->initialize_map();
		if (this->data.size()==0)
			return -1;

		string name = instr;
		std::transform( name.begin(), name.end(), name.begin(), (int(*)(int))toupper);

		map<string,t_MapValue>::iterator p = this->data.find(name);

		//-- if not found return null----
		if ( p == this->data.end() )
			return -1;

		//-- if not found return null----
		t_MapValue test = p->second;
		return ( test );
	};


	void reset(void)
	{
		this->data.clear();
	};

	void dump_map(void)
	{
		map<string,t_MapValue>::iterator p1 = this->data.begin();
		for ( ; p1 != this->data.end(); p1++ )
			cout << p1->first << " == " << p1->second << endl;
	};

private:
	bool is_initialized;
	map<string,t_MapValue> data;

};

#endif

