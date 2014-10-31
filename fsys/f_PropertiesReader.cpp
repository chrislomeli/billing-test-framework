#include <fsys/f_PropertiesReader.h>

static char *CVS_Id = (char*)"CVS_VERSION:$Id: f_PropertiesReader.cpp,v 1.5 2008/03/05 03:24:13 kwillens Exp $";


f_PropertiesReader::f_PropertiesReader(void)
{
}

f_PropertiesReader::~f_PropertiesReader(void)
{
}

void 
f_PropertiesReader::readPropertiesFile(
	const string target,
	const string ifilename )
{
	bool in_target = false;

	if (ifilename.empty())
	{
		return;
	}

	string filename(ifilename);
	string group;
	string elem;
	vector<c_NvRecord> records;

	c_NvRecord column_attribs;
	column_attribs.ignoreCase(true);

	string target_start(target);
	string target_end("/"+target);

	bool in_elem = false;

	vector<string> tokens;
	vector<string>::iterator vpos;

	string last_string;

	//---
	FILE * fp = NULL;

	const char * fname = filename.c_str();
	if ((fp=fopen(fname,"r")) == NULL )
	{
		return;
	}

	//---
	char buffer[1024];
	string line;
	while ( fgets(buffer,255,fp) != NULL )
	{
		line.assign(buffer);

		in_elem = false;
		tokens.clear();
		c_Strings::tokenize(line,tokens,"<>\n");

		for (vpos=tokens.begin(); vpos!=tokens.end(); vpos++)
		{
			string value(*vpos);
			c_Strings::trimAll(value,"\t\n ");
			if (value.empty())
				continue;


			if ( value.compare(0,target_start.size(),target_start) == 0 )
			{
				column_attribs.reset();
				column_attribs.parse_nv_argument_string(value," \t\n", "=");
				group.assign(value.substr(0,value.find_first_of(" \t\n")));

				this->onGroup(group,column_attribs);
				//WIP:: out of this the parser can be null - if nothing found 

				in_target=true;
				column_attribs.reset();
			}
			else if ( value.compare(0,target_end.size(),target_end) == 0 )
			{
				in_target=false;
				this->onGroupFinal(group);
			}

			else 
			{	
				int h = -1;
				int g = value.compare(0,1,"/");
				size_t l1 = value.size()-1;
				if (l1 > 0)
					h =value.compare(1,l1,elem.substr(0, l1));
				if (g==0 && h==0)
				{

					value.erase(0,1);
					if ( in_target && !target.empty() )
					{
						c_Strings::trimAll(last_string);


						column_attribs.reset();
						column_attribs.parse_nv_argument_string(elem," \t\n", "=");


						if (in_elem)
						{


						}


						if (!target.empty() && group==target)
							this->onElem(group,value,last_string,column_attribs);
						//WIP: the above code assumes a non NULL parser and blows up
						
					}
					in_elem=false;
				}
				else 
				{
					if (in_elem)
					{
						in_elem=false;
					}
					else
					{
						elem.assign(value);
						in_elem=true;
					}
				}
			}


			last_string.assign(value);
		} 
	}
	fclose(fp);

	
	return;

}

void f_PropertiesReader::onGroupFinal(string group)
{
		cout << "Group Ends: " << group << endl;
}

void f_PropertiesReader::onGroup(string group, c_NvRecord attribs)
{
		cout << "\n\nGroup : " << group << endl;
		map<string,string>::iterator p1 = attribs.nv_map.begin();
		for ( ; p1 != attribs.nv_map.end(); p1++ )
				cout << "		Attrib: " << p1->first << " == " << p1->second << endl;

}

void f_PropertiesReader::onElem(
			string group, string elem, string value, c_NvRecord attribs)
{
		cout << "   Elem" << "." << elem;
		if (!value.empty()) 
			cout << " == {" << value << "}" << endl;
		map<string,string>::iterator p1 = attribs.nv_map.begin();
		for ( ; p1 != attribs.nv_map.end(); p1++ )
				cout << "		Attrib: " << p1->first << " == " << p1->second << endl;

}
