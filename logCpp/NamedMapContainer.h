#if !defined(AFX_NamedMapContainer_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_)
#define AFX_NamedMapContainer_H__ECE17CBC_D100_435E_899B_FA6CBB65DB8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <util/EnvironmentStd.h>
#include <stdio.h>
#include <stdlib.h>

template <class T_member>
class NamedMapContainer
{
public:
	NamedMapContainer(void);
	~NamedMapContainer(void);

	void displayAll(void);
	void clear(void);
	bool addMember(string name, T_member  *t );

	map<string,T_member*> _Map;
};


template<class T_member>
NamedMapContainer<T_member>::NamedMapContainer(void)
{
}

template<class T_member>
NamedMapContainer<T_member>::~NamedMapContainer(void)
{
	this->clear();
}
template<class T_member>
bool NamedMapContainer<T_member>::addMember(string name, T_member  *t )
{
	this->_Map[name] = t;
	return true;
}

template<class T_member>
void NamedMapContainer<T_member>::clear(void)
{
	map<string,T_member*>::iterator pos = this->_Map.begin();
	
	for ( ; pos !=this->_Map.end(); pos++ )
	{
		string str = pos->first;
		T_member * memberp = pos->second;
		delete memberp;
		memberp=NULL;
	}
	this->_Map.clear();

}

template<class T_member>
void NamedMapContainer<T_member>::displayAll(void)
{
	map<string,T_member*>::iterator pos = this->_Map.begin();
	
	for ( ; pos !=this->_Map.end(); pos++ )
	{
		string str = pos->first;
		T_member * memberp = pos->second;
		cout << pos->first << "-->" << memberp->getName() << endl;
	}

}
#endif

