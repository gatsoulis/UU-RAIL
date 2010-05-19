/**
* @file strConvert.h
*
* @brief Convert a string to a double. A runtime error exception is thrown if conversion fails.
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#ifndef STRCONVERT_H_
#define STRCONVERT_H_

#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>

/**
 * @brief Runtime error exception if convertToDouble string to double conversion (strConvert.h/convertToDouble()) fails
 */
class BadConversion : public std::runtime_error {
public:
	BadConversion(const std::string& s)
	: std::runtime_error(s)
	{ }
};


/**
 * Converts a string to a double
 * @param s String to be converted
 * @param failIfLeftoverChars Boolean to throw the BadConversion exception
 * @return x The string as a double
 */
inline double convertToDouble(const std::string& s,
		bool failIfLeftoverChars = true)
{
	std::istringstream i(s);
	double x;
	char c;
	if (!(i >> x) || (failIfLeftoverChars && i.get(c)))
		throw BadConversion("convertToDouble(\"" + s + "\")");
	return x;
}

#endif /* STRCONVERT_H_ */
