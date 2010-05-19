/**
* @file C_ClosestNode.cpp
*
* @brief Class that holds the best-matching node of a Kohonen map to a given input
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/


#include "C_ClosestNode.h"


C_ClosestNode::C_ClosestNode()
{
	row = -1;
	col = -1;
	value = -1;
}


C_ClosestNode::C_ClosestNode(int r, int c, float value)
{
	row = r;
	col = c;
	this->value = value;
}


C_ClosestNode::~C_ClosestNode()
{

}


void C_ClosestNode::find_closest(float **data, int rlength, int clength)
{
	row = 0;
	col = 0;
	value = data[row][col];

	for (int r = 0; r < rlength; r++)
		for (int c = 0; c < clength; c++)
			// this finds the first instance of the smallest value, if I want to find the last instance then this can be modifed to <=
			if (data[r][c] < value) {
				row = r;
				col = c;
				value = data[r][c];
			}
}


void C_ClosestNode::find_closest(std::vector <std::vector <float> > out, unsigned int nRows, unsigned int nCols)
{
	row = 0;
	col = 0;
	value = out[row][col];
	for (unsigned int r = 0; r < nRows; r++)
		for (unsigned int c = 0; c < nCols; c++)
			// this finds the first instance of the smallest value, if I want to find the last instance then this can be modifed to <=
			if (out[r][c] < value) {
				row = r;
				col = c;
				value = out[r][c];
			}
}
