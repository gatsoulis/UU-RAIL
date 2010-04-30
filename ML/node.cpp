

#include "node.h"
#include <cmath>

///////////////////////////////////node constructor/destructor///////////////////////////////////////////
Node::Node(unsigned int id, const float *weights, int weights_number,
           const float *coords, int coords_number, int class_) : m_weights_number(weights_number), m_class(class_), m_precision(0.0f), m_coords_number(coords_number)
{
	m_id = id;
        m_weights = (float *)malloc(m_weights_number * sizeof(float));
        for (int i = 0; i < weights_number; i++)
                m_weights[i] = weights[i];

        m_coords = (float *)malloc(coords_number * sizeof(float));
        for (int i = 0; i < coords_number; i++)
                m_coords[i] = coords[i];

	m_novelty = 1.0;
	fireCount=0;
	m_region=0;
}
Node::~Node()
{
        free(m_weights);
        free(m_coords);
}
///////////////////////////////////node constructor/destructor///////////////////////////////////////////



/////////////////////////////votes calculation/////////////////////////////////////////////
void Node::clear_votes(int classes_number)
{
        if (m_votes.size() && classes_number == (int)m_votes.size()) {
                for (int c = 0; c < classes_number; c++)
                        m_votes[c] = 0;
        } else {
                m_votes.clear();
                for (int c = 0; c < classes_number; c++)
                        m_votes.push_back(0);
        }
        m_class = 0;
        m_precision = 0.0f;
}

bool Node::evaluate_class(const int *classes, int classes_number) //classes 1,2,3  or  2,4,5  or 5,2,1 ... not in ascending order
{
        if (classes_number) {
                m_precision = 0.0f;

                //get max votes number and assign a class to that node
                int maxvotes = m_votes[0];
                m_class = classes[0];
                for (int c = 1; c < classes_number; c++) {
                        if (maxvotes < m_votes[c]) {
                                maxvotes = m_votes[c];
                                m_class = classes[c];
                        }
                }

                //calculate node presicion = maxvotes/(cls1votes+cls2votes+ ... )
                if (maxvotes) {
                        for (int c = 0; c < classes_number; c++)
                                m_precision += m_votes[c];
                        m_precision = ((float)maxvotes / m_precision);
                } else
                        m_class = 0;

                return true;
        } else
                return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////


/*!
    \fn Node::check_coords(float* coords) const
 */
bool Node::check_coords(float* coords) const
{
	for (unsigned int i=0; i<m_coords_number; i++) {
		if (coords[i] != m_coords[i]) {
			return false;
		}
	}
	return true;
}


/*!
    \fn Node::increase_novelty(double weight)
 */
void Node::increase_novelty(float weight)
{
	m_novelty = m_novelty + weight*(1.0 - m_novelty);
}


/*!
    \fn Node::decrease_novelty(double weight)
 */
void Node::decrease_novelty(float weight)
{
	m_novelty = m_novelty - weight*m_novelty;
}
