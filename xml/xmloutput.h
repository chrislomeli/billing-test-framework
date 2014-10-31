/**
 * \file xmloutput.h
 * The header file for preparing xml output
 *
 * $Id: xmloutput.h,v 1.1 2007/01/15 14:14:24 sathpv Exp $
 * $Source: /proj/cvs/untd_billing/server/tools/cpptools/xml/xmloutput.h,v $
 *********************************************************************************/

# include <iostream>
# include <string>
# include <vector>
# include <map>
using namespace std;

typedef vector<const char*> ElementStack;

/**
 * This class it used to generated an xml string from a map.
 */
class Output
{

  public:
    /// constructor
    Output(char, char);
    /// write the beginnig
    
    void BeginDocument (const char *version="1.0",
                            const char *encoding="US-ASCII",
                            bool standalone=true);
    /// Does nothing but performs checking on nesting
    void EndDocument();

    /**
     * indent-normal nested indenting terse-do not add new lines for 
     * compact elements 
     */
    enum Mode { indent, terse}; 

    ///Begin a new element leaving the tag for attributes
    void BeginElement(const char *name,Mode mode = indent);
       
    /// End an element tag after writing attributes  
    void BeginElementAttrs(const char *name);
 
    void EndAttrs(Mode mode = indent);
        
    /// write a string attribute 
    void WriteAttr (const char *name,const string &value);
    /// write a c string atribute
    void WriteAttr (const char *name,const char *value);
    /// write an integer atribute
    void WriteAttr (const char *name,int value);
    /// write a double-value attribute
    void WriteAttr(const char *name,double value);
    /// write a boolean attribute
    void WriteAttr (const char *name,bool value);

    /**
     * finish writing the current element by writing 
     * the end element tag
     */
    void EndElement(Mode mode=indent);
    /// explicitly indent to the current nest level
    void Indent();

    /// write out a terse element with the specified data
    void WriteElement( const char *name,const string &value);
    void WriteElement(const char *name,const char *value);
    void WriteElement(const char *name,int value);
    void WriteElement(const char *name,double value);
    void WriteElement(const char *name,bool value);
    ///return the buffer to char *  
    char * Buffer();  
    ///output the xml response to a buffer
    char * XmlResponse (map<string,string> buf); 

    // output functions.

    Output &operator<<(const string &str);
    Output &operator<<(const char *str);
    Output &operator<<(int value);
    Output &operator<<(double value);
    Output &operator<<(bool value);

    void Write(const char *str, int len);
    void WriteString(const char *str);
    void WriteLine(const char *str);

    /// Destructor
    ~Output()  { delete []buffer;}

  private:
    char *buffer;
    int len; 
    int mLevel;            /**< nesting level */
    ElementStack mElements ;      
    bool mAttributes;
    char elemdelimit;
    char attribdelimit;
};
