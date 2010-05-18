/**
* @file support.h
*
* @brief General support functions
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#ifndef SUPPORT_H_
#define SUPPORT_H_

#include <vector>
#include <string>
//#include "C_TrainingPattern.h" // for sse

using namespace std;

//float sse(vector<float> weights, vector<float> tPattern);

/**
 * String tokenizer
 * @param str String to be tokenized
 * @param tokens The list that will hold the tokens
 * @param delimiters The delimiters for the tokenizing
 */
void tokenize(const string& str, vector<string>& tokens, const string& delimiters);

#endif /* SUPPORT_H_ */
