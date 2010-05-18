/**
* @file C_KohonenMap.h
* The Kohonen map uses a 1x1 neighbourhood and there is no decay on the learning rate.
* This should change at some point.
*
* @brief The main class of the Kohonen map
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#ifndef C_KOHONENMAP_H_
#define C_KOHONENMAP_H_

#include <vector>
#include "C_Node.h"
#include "C_ClosestNode.h"
#include "C_InputPattern.h"

/**
 * @brief The main class of the Kohonen map
 */
class C_KohonenMap
{
public:
	float eta; ///< The learning rate
	std::vector<float> delta; ///< The delta for the update of the weights
	unsigned int nRows; ///< Number of rows of the Kohonen map
	unsigned int nCols; ///< Number of columns of the Kohonen map
	unsigned int kmapLength; ///< Hmmm. What is this again?
	std::vector<C_Node*> nodes; ///< Vector of length nRows*nCols holding the nodes, basically the Kohonen map
	//float *map[];
	std::vector <std::vector<float> > out; ///< Hmmm. What is this again?
	C_ClosestNode closest; ///< The closest node of the SOM to a given input

	/**
	 * Constructor with default learning parameters: eta = 0.3
	 * @param nRows Number of rows of the Kohonen map
	 * @param nCols Number of columns of the Kohonen map
	 * @param nodeLength The length of the node, i.e. the length of the weights vector
	 */
	C_KohonenMap(unsigned int nRows, unsigned int nCols, int nodeLength);

	/**
	 * Constructor
	 * @param nRows Number of rows of the Kohonen map
	 * @param nCols Number of columns of the Kohonen map
	 * @param nodeLength The length of the node, i.e. the length of the weights vector
	 * @param eta The learning rate
	 */
	C_KohonenMap(unsigned int nRows, unsigned int nCols, int nodeLength, float eta);

	/**
	 * Default destructor
	 */
	~C_KohonenMap();

	/**
	 * delta = eta * (in(t,:) - w(addr,:)); delta = learning_rate * input_pattern - w_of_winner
	 * @param addr The index of the vector
	 * @param tPattern The training pattern
	 */
	void calc_delta(unsigned int addr, vector<float> tPattern);

	/**
	 * Update the weights of the winning node
	 * @param tPattern The training pattern
	 */
	void update_weights(vector<float> tPattern);

	/**
	 * Sum of squared errors
	 * @param addr The index of the vector
	 * @param tPattern The training pattern
	 */
	float sse(unsigned int addr, vector<float> *tPattern);

	/**
	 * A typical training step is to find the best-matching node and update its weights for each one of the training patterns
	  @param tps A vector that holds the training patterns
	 */
	void train_step(vector<C_InputPattern*> *tps);

	/**
	 * Save the training settings of a Kohonen map. Filename is "<prefix>.set"
	 * @param prefix Prefix of the filename
	 * @param postfix Not used
	 */
	void save_settings_to_file(char *prefix, const char *postfix);

	/**
	 * Save the Kohonen map to a file. Filename is "<prefix><postfix>.dat"
	 * @param prefix Prefix of the filename
	 * @param postfix Postfix of the filename. This is usually the number of the training step
	 */
	void save_weights_to_file(char *prefix, const char *postfix);

	/**
	 * Pending
	 * @param filename The filename from which to load a stored Kohonen map
	 */
	void load_weights_from_file(char *filename);

	/**
	 * Finds the best matching-node of a Kohonen for an input pattern
	 * @param pattern The input pattern
	 */
	void  matching_step(vector<float> *pattern);
};

#endif /* C_KOHONENMAP_H_ */
