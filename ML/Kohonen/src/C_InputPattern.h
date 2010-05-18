/**
* @file C_InputPattern.h
*
* @brief Class that holds an input pattern
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#ifndef C_INPUTPATTERN_H_
#define C_INPUTPATTERN_H_

#include <vector>

/**
 * @brief Class that holds an input pattern
 */
class C_InputPattern
{
public:
	int length; ///< Length of the input pattern
	std::vector<float> pattern; ///< The input pattern

	/**
	 * Constructor
	 * @param pattern The training pattern
	 */
	C_InputPattern(std::vector<float> pattern);

	/**
	 * Constructor
	 * @param length Length of the input pattern
	 * @param pattern The input pattern
	 */
	C_InputPattern(int length, float pattern[]);

	/**
	 * Default destructor
	 */
	~C_InputPattern();
};

#endif /* C_INPUTPATTERN_H_ */
