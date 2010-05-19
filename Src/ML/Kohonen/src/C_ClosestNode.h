/**
* @file C_ClosestNode.h
*
* @brief Class that holds the best-matching node of a Kohonen map to a given input
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#ifndef C_CLOSESTNODE_H_
#define C_CLOSESTNODE_H_

#include <vector>

/**
 * @brief Class that holds the best-matching node of a Kohonen map to a given input
 */
class C_ClosestNode
{
public:
	unsigned int row; ///< The row index of the closest node
	unsigned int col; ///< The column index of the closest node
	float value; ///< The (error?) value of the closest node

	/**
	 * Default constructor
	 */
	C_ClosestNode();

	/**
	 * Instantiate and "manually" set the closest node
	 * @param r The row index of the closest node
	 * @param c The column index of the closest node
	 * @param value The value of the closest node
	 * @return
	 */
	C_ClosestNode(int r, int c, float value);

	/**
	 * Default destructor
	 */
	~C_ClosestNode();

	/**
	 * Finds the first best-matching node of the SOM to input "data".
	 *
	 * @param data The input vector
	 * @param rlength Row length of data
	 * @param clength Column length of data
	 */
	void find_closest(float **data, int clength, int rlength);

	/**
	 * Finds the first best-matching node of the SOM to input "data".
	 *
	 * @param data The input vector
	 * @param nRows Row length of data
	 * @param nCols Column length of data
	 */
	void find_closest(std::vector <std::vector <float> > data, unsigned int nRows, unsigned int nCols);
};


#endif /* C_CLOSESTNODE_H_ */
