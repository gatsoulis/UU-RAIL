#ifndef Node_h
#define Node_h

#include <vector>
#include <xmmintrin.h>
#include <cmath>

class Node
{
        friend class SOM;
public:
        Node(unsigned int id, const float *weights, int weights_number,
             const float *coords, int coords_number, int class_ = 0);
        ~Node();

        enum DistanceMetric {EUCL, SOSD, TXCB, ANGL, MHLN};

// Operators
        //const Node& operator=(const Node& node);

// Operations
        bool evaluate_class(const int *classes, int classes_number); // const vector<int> &classes);

// Access
// Inquiry
        inline int get_class(void) const;
        inline const float* get_coords() const;
        inline const float* get_weights() const;
        inline float get_precision() const;
	inline float get_distance(const float *vec, enum DistanceMetric distance_metric, const float **cov = 0) const;
	bool check_coords(float* coords) const;
	int fireCount;

	int weights_number() const
	{
		return m_weights_number;
	}

	float novelty() const
	{
		return m_novelty;
	}
	void increase_novelty(float weight);
	void decrease_novelty(float weight);

	unsigned int id() const
	{
		return m_id;
	}

	inline unsigned int region() const {
		return m_region;
	}

	inline void set_region(int region) {
		m_region = region;
		m_tagged = true;
	}
private:
        Node(const Node& node);
        const Node& operator=(const Node& node);


        int m_weights_number;           //number of weights
        float *m_weights;               //weights vector
        float *m_coords;                //x,y,z, ... M_DIM position
        int m_class;                    //class mark 0-undetermined, 1,2,3,....
        float m_precision;              //(max class votes)/(class1 votes + class2 votes + ... + classN votes)
        std::vector<int> m_votes;            //votes for class1, class2, ... classN  //actual classes numbers stored in REC->clsnum[] array
		int m_region;
		bool m_tagged;	// is the node region tagged already?

        inline void train(const float *vec, float learning_rule);

        inline float mse(const float *vec1, const float *vec2, int size) const;

        inline void add_vote(int class_);
        void clear_votes(int classes_number = 0);
        inline void set_class(int class_);

protected:
	unsigned int m_coords_number;
	float m_novelty;
	unsigned int m_id;
};

inline const float* Node::get_coords() const
{
        return m_coords;
}

inline const float* Node::get_weights() const
{
        return m_weights;
}

inline float Node::get_precision() const
{
        return m_precision;
}

inline void Node::train(const float *vec, float learning_rule)
{
        for (int w = 0; w < m_weights_number; w++)
                m_weights[w] += learning_rule * (vec[w] - m_weights[w]);
}

inline float Node::get_distance(const float *vec, enum DistanceMetric distance_metric, const float**) const
{
        float distance = 0.0f;
        float n1 = 0.0f, n2 = 0.0f;

        switch (distance_metric) {
        default:
        case EUCL:   //euclidian
                if (m_weights_number >= 4) {
                        distance = mse(vec, m_weights, m_weights_number);
                } else {
                        for (int w = 0; w < m_weights_number; w++)
                                distance += (vec[w] - m_weights[w]) * (vec[w] - m_weights[w]);
                }
                return sqrt(distance);

        case SOSD:   //sum of squared distances
                if (m_weights_number >= 4) {
                        distance = mse(vec, m_weights, m_weights_number);
                } else {
                        for (int w = 0; w < m_weights_number; w++)
                                distance += (vec[w] - m_weights[w]) * (vec[w] - m_weights[w]);
                }
                return distance;

        case TXCB:   //taxicab
                for (int w = 0; w < m_weights_number; w++)
                        distance += fabs(vec[w] - m_weights[w]);
                return distance;

        case ANGL:   //angle between vectors
                for (int w = 0; w < m_weights_number; w++) {
                        distance += vec[w] * m_weights[w];
                        n1 += vec[w] * vec[w];
                        n2 += m_weights[w] * m_weights[w];
                }
                return acos(distance / (sqrt(n1)*sqrt(n2)));

        //case MHLN:   //mahalanobis
                //distance = sqrt(m_weights * cov * vec)
                //return distance
        }
}

inline float Node::mse(const float *vec1, const float *vec2, int size) const
{
        float z = 0.0f, fres = 0.0f;
        float ftmp[4];
        __m128 mv1, mv2, mres;
        mres = _mm_load_ss(&z);

        for (int i = 0; i < size / 4; i++) {
                mv1 = _mm_loadu_ps(&vec1[4*i]);
                mv2 = _mm_loadu_ps(&vec2[4*i]);
                mv1 = _mm_sub_ps(mv1, mv2);
                mv1 = _mm_mul_ps(mv1, mv1);
                mres = _mm_add_ps(mres, mv1);
        }
        if (size % 4) {
                for (int i = size - size % 4; i < size; i++)
                        fres += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
        }

        //mres = a,b,c,d
        mv1 = _mm_movelh_ps(mres, mres);   //a,b,a,b
        mv2 = _mm_movehl_ps(mres, mres);   //c,d,c,d
        mres = _mm_add_ps(mv1, mv2);       //res[0],res[1]

        _mm_storeu_ps(ftmp, mres);

        return fres + ftmp[0] + ftmp[1];
}

inline void Node::add_vote(int class_)
{
        m_votes[class_]++;
}

inline int Node::get_class(void) const
{
        return m_class;
}

inline void Node::set_class(int class_)
{
        m_class = class_;
}

#endif

