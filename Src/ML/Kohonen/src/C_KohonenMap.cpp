/**
* @file C_KohonenMap.cpp
* The Kohonen map uses a 1x1 neighbourhood and there is no decay on the learning rate.
* This should change at some point.
*
* @brief The main class of the Kohonen map
* @date Created on: 8 Feb 2010
*
* @author Yianni
* [Note: deprecated practice as git will maintain author info and blame logs.]
*/

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <malloc.h>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include "C_KohonenMap.h"
#include "C_Node.h"
#include "strConvert.h"
#include "support.h"

using namespace std;

C_KohonenMap::C_KohonenMap(unsigned int nRows, unsigned int nCols, int nodeLength)
{
	eta = 0.3;
	delta.resize(nodeLength, 0.0);
	this->nRows = nRows;
	this->nCols = nCols;
	kmapLength = this->nRows * this->nCols;
	srand((unsigned)time(0));
	for (unsigned int i = 0; i < kmapLength; i++)
		nodes.push_back(new C_Node(nodeLength));
	out.resize(nRows, std::vector<float> (nCols, -1));
}

C_KohonenMap::C_KohonenMap(unsigned int nRows, unsigned int nCols, int nodeLength, float eta)
{
	this->eta = eta;
	delta.resize(nodeLength, 0.0);
	this->nRows = nRows;
	this->nCols = nCols;
	kmapLength = this->nRows * this->nCols;
	for (unsigned int i = 0; i < kmapLength; i++)
		nodes.push_back(new C_Node(nodeLength));
	out.resize(nRows, std::vector<float> (nCols, 0));
}

C_KohonenMap::~C_KohonenMap() {
	for (std::vector<C_Node*>::iterator node = nodes.begin(); node != nodes.end(); node++)
		delete *node;
}


// delta = eta * (in(t,:) - w(addr,:)); // delta = learning_rate * input_pattern - w_of_winner
void C_KohonenMap::calc_delta(unsigned int addr, vector<float> tPattern)
{
	for (unsigned int i = 0; i < tPattern.size(); i++) {
		delta.at(i) = eta * (tPattern.at(i) - nodes.at(addr)->weights.at(i));
	}
}


void C_KohonenMap::update_weights(vector<float> tPattern)
{
	//cout << "(" << closest.row << ", " << closest.col << "): " << closest.value << endl;
	unsigned int addr;
	int ar, ac;
	int dbgCounter = 0;
	for (int r = (int) closest.row - 1; r <= (int) closest.row + 1; r++)
		for (int c = (int) closest.col - 1; c <= (int) closest.col + 1; c++) {
			ar = r;
			ac = c;
			// Define torus shape, i.e. the wrapping of the edges
			if (ar > (int) nRows - 1)
				ar -= nRows;
			else if (ar < 0)
				ar += nRows;
			if (ac > (int) nCols - 1)
				ac -= nCols;
			else if (ac < 0)
				ac += nCols;
			addr = ar * nCols + ac;
			//cout << "Updating node: (" << ar << ", " << ac << ") = " << addr << endl;
			calc_delta(addr, tPattern);
			for (uint i = 0; i < nodes.at(addr)->weights.size(); i++)
				nodes.at(addr)->weights.at(i) += delta.at(i);
			dbgCounter++;
		}
	if (dbgCounter != 9)
		cout << ">> ERROR (" << dbgCounter << "): Loop did not execute for the whole neighbourhood " << "(" << closest.row << ", " << closest.col << "): " << closest.value << endl;
	//cout << "----\n";
}


float C_KohonenMap::sse(unsigned int addr, vector<float> *tPattern)
{
//	ofstream fsout;
//	fsout.open("log.txt", fstream::app);
	float sum = 0;
	for (unsigned int i = 0; i < tPattern->size(); i++) {
//		fsout << "nodes[" << addr << "].weights[" << i << "] - tPattern[" << i << "]: " << nodes.at(addr)->weights.at(i) << " - " << tPattern->at(i) << endl;
		sum += pow(nodes.at(addr)->weights.at(i) - tPattern->at(i), 2);
	}
//	fsout << sum << endl << "---" << endl;
//	fsout.close();
	return sum;
}


void C_KohonenMap::train_step(vector<C_InputPattern*> *tps)
{
	ofstream fsout;
	fsout.open("log.txt");


	for (unsigned int t = 0; t < tps->size(); t++) {
		for (unsigned int r = 0; r < nRows; r++)
			for (unsigned int c = 0; c < nCols; c++)
				out[r][c] = sse(r * nCols + c, &tps->at(t)->pattern);
		closest.find_closest(out, nRows, nCols);
		fsout << "(" << closest.row << ", " << closest.col << "): " << closest.value << endl;
		update_weights(tps->at(t)->pattern);
	}
	fsout.close();
}


void C_KohonenMap::save_settings_to_file(char *prefix, const char *postfix)
{
	ofstream fsout;
	string filename = prefix;
	filename.append(".set");
	fsout.open (filename.c_str());
	fsout << nRows << endl << nCols << endl;
	fsout.close();
}

void C_KohonenMap::save_weights_to_file(char *prefix, const char *postfix)
{
	ofstream fsout;
	unsigned int addr;
	const unsigned int wSize = nodes.at(0)->weights.size();
	string filename = prefix;
	filename.append(postfix);
	filename.append(".dat");
	fsout.open (filename.c_str());
	for (unsigned int r = 0; r < nRows; r++)
		for (unsigned int c = 0; c < nCols; c++) {
			addr = r * nCols + c;
			for (unsigned int i = 0; i < wSize - 1; i++)
				fsout << nodes.at(addr)->weights.at(i) << " ";
			fsout << nodes.at(addr)->weights.at(wSize - 1);
			fsout << endl;
		}
	fsout.close();
}


void C_KohonenMap::load_weights_from_file(char *filename)
{
	ifstream fsin(filename);
	string line;
    vector<string> tokens;
    unsigned int k = 0;
//	char *pch;
	while (fsin.good()) {
		getline(fsin, line);
//		pch = strtok((char *) line.c_str(), " ");
//		while (pch != NULL) {
//			printf ("%s ",pch);
//			pch = strtok(NULL, " ");
//		}
//		printf("\n");
		tokenize(line, tokens, " ");
		for (unsigned int i = 0; i < tokens.size(); i++)
			nodes.at(k)->weights.at(i) = (float) convertToDouble(tokens.at(i));
		tokens.clear();
		k++;
	}
	fsin.close();

//	for (unsigned int k = 0; k < nodes.size(); k++) {
//		for (unsigned int i = 0; i < nodes.at(k)->weights.size(); i++)
//			cout << "<" << nodes.at(k)->weights.at(i) << "> ";
//		cout << endl;
//	}
}


void C_KohonenMap::matching_step(vector<float> *input)
{
	for (unsigned int r = 0; r < nRows; r++)
		for (unsigned int c = 0; c < nCols; c++)
			out[r][c] = sse(r * nCols + c, input);
	closest.find_closest(out, nRows, nCols);
}
