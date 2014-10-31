#include <util/EnvironmentStd.h>
#include <util/EnvironmentAnsiC.h>


const char * root_name(const char * filename)
{
	const char * cp = filename;
	while( *cp != '\0'){
		const char * p=strchr(cp,'\\');
		if (p!=0 && *(p+1)!='\0')
			cp = p+1;
		else 
			break;
	}
	return cp;
}


