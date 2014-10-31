/**
 * \file xmloutput.cpp
 * The source file for preparing xml output
 *
 * $Id: xmloutput.cpp,v 1.1 2007/01/15 14:14:24 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/xml/xmloutput.cpp,v $
 *********************************************************************************/

#include "xmloutput.h"
#include <cassert>
#include <cerrno>
#include <new>
using namespace std;

static char *CVS_Id = (char *)"CVS_VERSION:$Id: xmloutput.cpp,v 1.1 2007/01/15 14:14:24 sathpv Exp $";
static char *CVS_Source = (char *)"CVS_VERSION:$Source: /proj/cvs/untd_billing/server/tools/cpptools/xml/xmloutput.cpp,v $";

/// Constructor with delimiters
Output::Output(char delimit1, char delimit2)
{
  buffer=new char[1];
  strcpy(buffer,"\0");
  len=0;
  mLevel=0;
  mAttributes = false;
  elemdelimit=delimit1;
  attribdelimit=delimit2;
}

/// Writes the string passed to the local variable
void Output::Write(const char *newbuf, int newlen)
{

  if(len==0) {
    delete []buffer;
    buffer=new char[newlen+1];
    len=newlen;
    strcpy(buffer,newbuf);
  } else {
    char*oldbuf=buffer;
    int nbufsize=len+newlen;
    buffer=new char[nbufsize+1];
    len=nbufsize;
    strcpy(buffer,oldbuf);
    strcat(buffer,newbuf);
    delete [] oldbuf;
  }
}

/// Writes str to the local varible
void Output::WriteString(const char *str)
{
  assert(str);
  Write(str, strlen(str));
}

void Output::WriteLine(const char *str)
{
  assert(str);
  Write(str, strlen(str));
  Write("\n", 1);
}

Output &Output::operator<<(const string &str)
{
  Write(str.c_str(), str.size());
  return *this;
}

Output &Output::operator<<(const char *str)
{
  assert(str);
  WriteString(str);
  return *this;
}

Output &Output::operator<<(int value)
{
  char tmp[50];
  sprintf(tmp, "%d", value);
  WriteString(tmp);
  return *this;
}

Output &Output::operator<<(double value)
{
  char tmp[50];
  sprintf(tmp, "%g", value);
  WriteString(tmp);
  return *this;
}

Output &Output::operator<<(bool value)
{
  WriteString(value ? "True" : "False");
  return *this;
}

void Output::BeginDocument(const char *version, const char *encoding, bool standalone)
{
  assert(version);
  assert(encoding);

  (*this) << "<?xml version=\"" << version << "\" encoding=\"" << encoding << "\"";
  (*this) << " standalone=\"" << (standalone ? "yes" : "no") << "\"?>\n";
}

void Output::EndDocument()
{
  assert(!mAttributes);
  assert(mElements.empty());
}

void Output::Indent()
{
  for (int i = 0; i < mLevel; i++)
    (*this) << "\t";
}

void Output::BeginElement(const char *name, Mode mode)
{
  assert(name);
  assert(!mAttributes);
  Indent();
  mLevel++;
  (*this) << "<" << name << ">";
  if (mode != terse)
    (*this) << "\n";
  mElements.push_back(name);
}

void Output::BeginElementAttrs(const char *name)
{
  assert(name);
  assert(!mAttributes);
  Indent();
  mLevel++;
  (*this) << "<" << name;
  mAttributes = true;

  mElements.push_back(name);
}

void Output::EndAttrs(Mode mode)
{
  assert(mAttributes);
  mAttributes = false;
  (*this) << ">";
  if (mode != terse)
    (*this) << "\n";
}

void Output::EndElement(Mode mode)
{
  assert(mElements.size() > 0);
  assert(!mAttributes);
  assert(mLevel > 0);
  --mLevel;

  if (mode != terse)
    Indent();

  const char *name = mElements.back();
  mElements.pop_back();

  (*this) << "</" << name << ">" << "\n";
}

std::string CheckChars(const char* ptr){

  const char* indx=ptr;
  std::string buf;

  while(*indx){

    switch (*indx){

      case '<':
  buf+="&lt;";
  break;

      case '>':
  buf+="&gt;";
  break;

      case '&':
  if ('#' != *(indx+1))
    buf+="&amp;";
  else
    buf+="&";
  break;

      case '"':
  buf+="&quot;";
  break;

      case '\'':
  buf+="&apos;";
  break;

      default:
  buf+= *indx;
  break;

    }
    indx++;
  }

  return buf;
}

void Output::WriteElement(const char *name, const std::string &value)
{
  assert(name);
  BeginElement(name, terse);
  (*this) << (CheckChars(value.c_str())).c_str();
  EndElement(terse);
}

void Output::WriteElement(const char *name, const char *value)
{
  assert(name);
  assert(value);
  BeginElement(name, terse);
  (*this) << (CheckChars(value)).c_str();
  EndElement(terse);
}

void Output::WriteElement(const char *name, int value)
{
  assert(name);
  BeginElement(name, terse);
  (*this) << value;
  EndElement(terse);
}

void Output::WriteElement(const char *name, double value)
{
  assert(name);
  BeginElement(name, terse);
  (*this) << value;
  EndElement(terse);
}

void Output::WriteElement(const char *name, bool value)
{
  assert(name);
  BeginElement(name, terse);
  (*this) << value;
  EndElement(terse);
}

void Output::WriteAttr(const char *name, const std::string &value)
{
  assert(mAttributes);
  assert(name);
  (*this) << " " << name << "=\"" << value << "\"";
}

void Output::WriteAttr(const char *name, const char *value)
{
  assert(mAttributes);
  assert(name);
  assert(value);

  (*this) << " " << name << "=\"" << value << "\"";
}

void Output::WriteAttr(const char *name, int value)
{
  assert(mAttributes);
  assert(name);

  (*this) << " " << name << "=\"" << value << "\"";
}

void Output::WriteAttr(const char *name, double value)
{
  assert(mAttributes);
  assert(name);

  (*this) << " " << name << "=\"" << value << "\"";
}

void Output::WriteAttr(const char *name, bool value)
{
  assert(mAttributes);
  assert(name);

  (*this) << " " << name << "=\"" << value << "\"";
}

char* Output::Buffer()
{
   return buffer;
}

char* Output::XmlResponse(map<string,string> buf)
{
  Output temp(this->elemdelimit,this->attribdelimit);
  int index;
  map<string,string>::iterator p;
  vector<string> write;
  p=buf.find("trancd");
  temp.BeginElement(p->second.c_str());
  buf.erase(p);
  p=buf.begin();
  while( p != buf.end()) {
    index=p->first.find_first_of(elemdelimit,0);
    if(index == string::npos) {
      temp.WriteElement(p->first.c_str(),p->second.c_str());
      write.push_back(p->first);
      p++;
    } else {
      p++;
    }
  }
  for(int i=0;i<write.size();i++)
  {
    p=buf.find(write[i]);
    buf.erase(p);
  }
  //code for writing the unordered nested elements into the buf
  int size=buf.size();
  int count=0;
  while(count<size) {
    const char *attrib="index";
    p=buf.begin();
    int start_index=p->first.find_first_of(this->elemdelimit,0);
    string elem = p->first.substr(0,start_index);
    int end_index=p->first.find_first_of(this->attribdelimit,0);
    string attribvalue;
    if(end_index != string::npos) {
      attribvalue=p->first.substr(end_index+1,
          (p->first.size()-end_index-1));
      temp.BeginElementAttrs(elem.c_str());
      temp.WriteAttr(attrib,attribvalue.c_str());
      temp.EndAttrs(indent);
      write.clear();
      while(p!=buf.end()) {
        int istart_index=p->first.find_first_of(this->elemdelimit,0);
        string ielem;
        if(istart_index !=string::npos) {
          ielem=p->first.substr(0,istart_index);
        }
        int iend_index=p->first.find_first_of(this->attribdelimit,0);
        string iattribvalue;
        if(iend_index != string::npos) {
          iattribvalue=p->first.substr(iend_index+1,
              (p->first.size()-iend_index-1));
        }
        string subelem=p->first.substr(istart_index+1,
            iend_index-istart_index-1);
        if( ielem==elem && iattribvalue==attribvalue) {
          temp.WriteElement(subelem.c_str(),p->second.c_str());
          write.push_back(p->first);
          p++;
        } else {
          p++;
        }
      }
      for(int i=0;i<write.size();i++) {
        p=buf.find(write[i]);
        buf.erase(p);
      }
      temp.EndElement();
      size=buf.size();
      count=0;
    } else {
      temp.BeginElement(elem.c_str());
      write.clear();
      while(p!=buf.end()) {
        int istart_index=p->first.find_first_of(this->elemdelimit,0);
        string ielem=p->first.substr(0,istart_index);
        string subelem=p->first.substr(istart_index+1,
            p->first.size()-istart_index-1);
        if( ielem==elem) {
          temp.WriteElement(subelem.c_str(),p->second.c_str());
          write.push_back(p->first);
          p++;
        } else {
          p++;
        }
      }
      int count=0;
      for(int i=0;i<write.size();i++){
        p=buf.find(write[i]);
        buf.erase(p);
      }
      temp.EndElement();
      size=buf.size();
      count=0;
    }
  }
  temp.EndElement();
  int len1=strlen(temp.buffer);
  if(buffer)
    delete [] buffer;
  buffer= new char[strlen(temp.buffer)+1];
  strcpy(buffer,temp.buffer);
  return buffer;
}
