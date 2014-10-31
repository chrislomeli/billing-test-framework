# include <iostream>
# include <string>
# include <map>
# include <vector>
using namespace std;
# include "xmloutput.h"
# include "tinyxmlinterface.h"


static char *CVS_Id = (char *)"CVS_VERSION:$Id: main.cpp,v 1.1 2007/01/15 14:14:24 sathpv Exp $";
static char *CVS_Source = (char *)"CVS_VERSION:$Source: /proj/cvs/untd_billing/server/tools/cpptools/xml/main.cpp,v $";


int main()
{


 map<string,string> buf;
 map<string,string>::iterator p;

 XmlInterface Object('@','&');
 ifstream in ("test1.xml");
 ostrstream tmp;
 tmp<<in.rdbuf();
 char *x=tmp.str();

 Object.XmlParse(x);
 Object.XmltoMap(buf);
 for(p=buf.begin();p!=buf.end();p++)
 {
   cout<<"Name:"<<p->first<<"\t\t"<<"Value:"<<p->second<<endl;
 }

map<string,string>xml;
xml.insert(pair<string,string>("trancd","ASUM"));
xml.insert(pair<string,string>("BILL#START_TIME@0","1:00"));
xml.insert(pair<string,string>("BILL#START_TIME@1","2:00"));
xml.insert(pair<string,string>("BILL#START_TIME@2","6:00"));
xml.insert(pair<string,string>("BILL#END_TIME@0","8:0"));
xml.insert(pair<string,string>("BILL#END_TIME@2","8:0"));
xml.insert(pair<string,string>("BILL#END_TIME@1","8:0"));
xml.insert(pair<string,string>("BILL#TOTALTIME@1","10:00"));
xml.insert(pair<string,string>("BILL#TOTALTIME@0","10:00"));
xml.insert(pair<string,string>("FIRSTNAME","SUBBURAIDU"));
xml.insert(pair<string,string>("LASTNAME","YARLAGADDA"));
xml.insert(pair<string,string>("CITY",""));
char *temp;
Output object('#','@');
temp=object.XmlResponse(xml);
cout<<"********"<<endl;
cout<<temp<<endl;
cout<<"*********"<<endl;
}


