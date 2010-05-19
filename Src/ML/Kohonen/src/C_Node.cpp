/**
* @file C_Node.cpp
*
* @brief A Kohonen map node
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#include <cstdlib>
#include "C_Node.h"

C_Node::C_Node(int length)
{
	nodeLength = length;
	int decimalDigit = 10000;
	for (int i = 0; i < nodeLength; i++)
		weights.push_back((float)(rand() % decimalDigit) / decimalDigit);
}

C_Node::C_Node(vector<float> w)
{
	for (unsigned int i = 0; i < w.size(); i++)
		weights.push_back(w.at(i));
}

C_Node::~C_Node()
{

}
