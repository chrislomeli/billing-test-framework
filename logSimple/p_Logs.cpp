#include <logSimple/p_Logs.h>

p_LogSimpleStream p_Logs::errLog;
p_LogSimpleStream p_Logs::userLog;

ostream& operator<<(ostream& out, const map<string,c_NvPair>& tokens )
{
	map<string,c_NvPair>::const_iterator pos = tokens.begin();
	for (; pos != tokens.end(); pos++ )
	{
		out << "\t" << pos->first << "=" << pos->second.value << endl;
	}
	return out;
}

p_Logs::p_Logs(void)
{
}

p_Logs::~p_Logs(void)
{
}
