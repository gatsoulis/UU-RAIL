/**
* @file support.cpp
*
* @brief General support functions
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#include <cmath>
#include "support.h"

//float sse(vector<float> weights, vector<float> tPattern)
//{
//	int length = weights.size();
//	float sum = 0;
//	for (int i = 0; i < length; i++)
//		sum += pow(weights.at(i) - tPattern.at(i), 2);
//	return sum;
//}

void tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
	// Skip delimiters at beginning.
	string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	string::size_type pos     = str.find_first_of(delimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		// Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));
		// Skip delimiters.  Note the "not_of"
		lastPos = str.find_first_not_of(delimiters, pos);
		// Find next "non-delimiter"
		pos = str.find_first_of(delimiters, lastPos);
	}
}
