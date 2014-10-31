#pragma once

#include <util/EnvironmentStd.h>

class c_Tokenizer
{
public:
	c_Tokenizer(void);
	~c_Tokenizer(void);
	virtual void tokenize(string data, vector<string>& tokens) = 0;
};
