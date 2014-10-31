/**
 * \file tinyxmlinterface.cpp
 * Source file containing classes for xml operations
 *
 * $Id: tinyxmlinterface.cpp,v 1.1 2007/01/15 14:14:24 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/xml/tinyxmlinterface.cpp,v $
 *********************************************************************************/

# include "tinyxml.h"
# include "tinyxmlinterface.h"
# define TIXML_USE_STL
# ifdef TIXML_USE_STL
      # include  <iostream>
      # include  <sstream>
      # include <strstream>
      # include <fstream>
      # include <vector>
      # include <map>
      using namespace std;
#endif

static char *CVS_Id = (char *)"CVS_VERSION:$Id: tinyxmlinterface.cpp,v 1.1 2007/01/15 14:14:24 sathpv Exp $";
static char *CVS_Source = (char *)"CVS_VERSION:$Source: /proj/cvs/untd_billing/server/tools/cpptools/xml/tinyxmlinterface.cpp,v $";

/// Constructor -- with delimiters.
XmlInterface::XmlInterface(char delimit1, char delimit2)
{
  elemdelimit=delimit1;
  attribdelimit=delimit2;
}

/// Parses the given buffer
int XmlInterface::XmlParse(char *buf)
{
// Parse the given buffer
  doc.Parse(buf);
  if(doc.Error())
  {
    printf("Error in %s:%s\n",doc.Value(),doc.ErrorDesc());
    return -1;
    //exit(1);
  }
  doc.SaveFile();
  return 0;
}

/// Puts the xml contents into a map
void XmlInterface::XmltoMap(map<string,string> &xml)
{
 map<string,string>::iterator count=xml.begin();
 TiXmlElement *parentelem=doc.RootElement();
 for(TiXmlNode *parentnode=parentelem;parentnode;parentnode=parentnode->NextSibling())
 {
   TiXmlElement *iparentelem=parentnode->ToElement();
   string sparentnode=parentnode->Value();
   string sparentattrib;
   TiXmlAttribute *parentattrib=iparentelem->FirstAttribute();
   if(parentattrib)
   {
       sparentattrib=parentattrib->Value();
   }
   TiXmlElement *parentcontent=parentnode->FirstChildElement();
   if(!parentcontent && !parentattrib)
   {
     TiXmlNode *parentdata=parentnode->FirstChild();
     if(parentdata)
     {
       TiXmlText *parenttext=parentdata->ToText();
       string sparenttext=parenttext->Value();
       xml.insert(pair<string,string>(sparentnode,sparenttext));
      // count=xml.find(sparentnode);
      // cout<<"Name:"<<sparentnode;
      // cout<<"\t\tValue:"<<count->second<<endl;
     }
     else
     {
        string sparenttext;
        xml.insert(pair<string,string>(sparentnode,sparenttext));
      }
   }
   else
   {
     count=xml.find("trancd");
     if(count==xml.end())
     {
       xml.insert(pair<string,string>("trancd",sparentnode));
     }
    // count=xml.find("transaction");
    // cout<<"Name:"<<"transaction";
    // cout<<"\t\tValue:"<<count->second<<endl;
     TiXmlElement *childelem=parentcontent;
     for(TiXmlNode *childnode=childelem;childnode;childnode=childnode->NextSibling())
     {
       TiXmlElement *ichildelem=childnode->ToElement();
       string schildnode=childnode->Value();
       string schildattrib;
       TiXmlAttribute *childattrib=ichildelem->FirstAttribute();
       if(childattrib)
       {
            schildattrib=childattrib->Value();
       }
       TiXmlElement *childcontent=childnode->FirstChildElement();
       if(!childcontent && !childattrib)
       {
         TiXmlNode *childdata=childnode->FirstChild();
         if(childdata)
         {
           TiXmlText *childtext=childdata->ToText();
           string schildtext=childtext->Value();
           xml.insert(pair<string,string>(schildnode,schildtext));
          // count=xml.find(schildnode);
          // cout<<"Name:"<<schildnode;
          // cout<<"\t\tValue:"<<count->second<<endl;
         }
         else
         {
             string schildtext;
             xml.insert(pair<string,string>(schildnode,schildtext));
         }
       }
       else
       {
         TiXmlElement *grandchildelem=childcontent;
         for(TiXmlNode *grandchildnode=grandchildelem;grandchildnode;grandchildnode=grandchildnode->NextSibling())

         {
           TiXmlElement *igrandchildelem=grandchildnode->ToElement();
           string sgrandchildnode=grandchildnode->Value();
           string sgrandchildattrib;
           string pathname;
           TiXmlAttribute *grandchildattrib=igrandchildelem->FirstAttribute();
           if(grandchildattrib)
           {
              sgrandchildattrib=grandchildattrib->Value();
           }
           TiXmlElement *grandchildcontent=grandchildnode->FirstChildElement();
           if(!grandchildcontent && !grandchildattrib)
           {
             TiXmlNode *grandchilddata=grandchildnode->FirstChild();
             if(grandchilddata)
             {
               TiXmlText *grandchildtext=grandchilddata->ToText();
               string sgrandchildtext=grandchildtext->Value();
               if(childattrib)
               {
                  pathname=schildnode+elemdelimit+sgrandchildnode+attribdelimit+schildattrib;
               }
               else
               {
                  pathname=schildnode+elemdelimit+sgrandchildnode;
               }
               xml.insert(pair<string,string>(pathname,sgrandchildtext));
              // count=xml.find(pathname);
              // cout<<"Name:"<<pathname;
              // cout<<"\t\tValue:"<<sgrandchildtext<<endl;
             }
             else
             {
               string sgrandchildtext;
               if(childattrib)
               {
                  pathname=schildnode+elemdelimit+sgrandchildnode+attribdelimit+schildattrib;
               }
               else
               {
                  pathname=schildnode+elemdelimit+sgrandchildnode;
               }
               xml.insert(pair<string,string>(pathname,sgrandchildtext));
             }
            }
           }
         }
       }
     }
   }
 }




