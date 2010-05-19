/**
* @file C_Node.h
*
* @brief A Kohonen map node
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#ifndef C_NODE_H_
#define C_NODE_H_

#include <vector>
using namespace std;

/**
 * @brief A Kohonen map node
 */
class C_Node
{
//private:
public:
	int nodeLength; ///< Length of the weights vector of the node
	vector<float> weights; ///< Weights vector of the node

	/**
	 * Constructor: Weights initialised to random values
	 * @param length Length of the weights vector of the node
	 */
	C_Node(int length);

	/**
	 * Constructor: Set the node weights
	 * @param w Set of weights that the node weights are initialised to
	 */
	C_Node(vector<float> w);

	/**
	 * Default destructor
	 */
	~C_Node();
};


#endif /* C_NODE_H_ */
