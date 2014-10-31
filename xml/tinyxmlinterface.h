/**
 * \file tinyxmlinterface.h
 * Header file containing classes for xml operations
 *
 * $Id: tinyxmlinterface.h,v 1.1 2007/01/15 14:14:24 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/xml/tinyxmlinterface.h,v $
 *********************************************************************************/

# include "tinyxml.h"
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

/**
 * The interface class for xml operations.
 */
class XmlInterface
{
 private:
         TiXmlDocument doc;  
         char elemdelimit;
         char attribdelimit;

 public:
         XmlInterface(char ,char);
         int XmlParse(char *buf);
         void XmltoMap(map<string,string> &xml);
         ~XmlInterface() {};
};


