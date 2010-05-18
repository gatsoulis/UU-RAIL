/**
* @file C_InputPattern.cpp
*
* @brief Class that holds an input pattern
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#include "C_InputPattern.h"

C_InputPattern::C_InputPattern(std::vector<float> pattern)
{
	length = pattern.size();
	this->pattern.assign(pattern.begin(), pattern.end());
}

C_InputPattern::C_InputPattern(int length, float pattern[])
{
	this->length = length;
	for (int i = 0; i < length; i++)
		this->pattern.push_back(pattern[i]);
}

C_InputPattern::~C_InputPattern()
{

}
