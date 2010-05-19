

#ifndef Som_h
#define Som_h


//class Node;  before inlined
#include "node.h"
#include <vector>
#include <cstdlib>
#include <string.h>

// Include the k_means code
extern int *k_means(const float**, int, int, int, float, float**);

typedef struct _entry {
	float *vec;                //vector
	int size;                  //vector size
	char fname[256];  //vector file name
	int cls;                   //vector class
} ENTRY, *PENTRY;

typedef struct _rec {
	std::vector<PENTRY> entries;         //rec entries
	std::vector< std::vector<int> > indices;  //2D array of classes indices
	std::vector<int> clsnum;             //classes numbers of indeces columns
} REC, *PREC;

/*
   2D type array

    [entry ... vec ...]   (+ size,fname,class type)
    [entry ... vec ...]
    [entry ... vec ...]
    [entry ... vec ...]
     ...
    N = rec.size()

    individual point - rec.entries[y].vec[x]


    indices to y axis of entries vector
     vector<int> x0;  vector<int> x1; vector<int> x2; ... vector<int> xClassesNum

    clsnum[0],clsnum[1], ... clsnum[N]
      [x0][x1][x2][x3] ... [xN]    N different classes
      [x0][x1][x2][x3] ... [xN]
      [x0][x1][x2][x3] ... [xN]
          [x1][x2]
          [x1]
          ...

    example
      rec->clsnum[] =   3, 1, 2    <--  1D vector

                        0  1  8    <--  rec->indices[x].at(y)  2D array
                       10  5  9
                        3  2
                        4  6

                                     */

class SOM
{
	public:
		SOM(int dimensionality, int *dimensions, int weights_per_node,          //create/randomize nodes with weights
		    enum Node::DistanceMetric distance_metric, float *add = 0, float *mul = 0);
		SOM(const char *fname);                                              //load SOM net, if empty, default norm=0
		~SOM();

		enum Normalization {NONE, MNMX, ZSCR, SIGM, ENRG};
		enum TrainMode {SLOW, FAST};

		static const char g_distance[5][5];
		static const char g_normalization[5][5];
		static const char g_trainmode[2][5];

		float *distancemap;

		float R0() const;                               //calculate initial R0 for 1st epoch
		void vote_nodes_from(PREC rec);                 //voting scheme; get_bmu_node to a vector
		void assign_nodes_to(PREC rec);                 //direct scheme; best vector to a node

		const Node* classify(const float *vec);         //return BMU node
		int save(const char *fname) const;
		int save_2D_distance_map(const char *fname) const;
		int compute_distance_map(const float *vec);
		void compute_normalization(PREC rec, enum Normalization norm);

		inline int status(void) const;
		inline int get_dimensionality(void) const;
		inline int get_weights_per_node(void) const;
		inline int get_nodes_number() const;
		inline const Node* get_node(int n) const;
		inline void set_distance_metric(enum Node::DistanceMetric distance_metric);
		inline void set_train_mode(enum TrainMode train_mode);

		void setNormalization ( const enum Normalization& theValue )
		{
			m_normalization = theValue;
		}
		const Node* find_node(float* coords) const;
		Node* find_node(float* coords);
		void train(const std::vector<float *> &vectors, float R, float learning_rule, bool* fold);

		inline int* dimensions() const
		{
			return m_dimensions;
		}

		void setNovelty_decrease ( float theValue )
		{
			m_novelty_decrease = theValue;
		}

		void setNovelty_increase ( float theValue )
		{
			m_novelty_increase = theValue;
		}

		inline int getNodeCount() {	// NOTE: Only any good for 2D maps!
			return m_dimensions[0]*m_dimensions[1];
		}

		void splitToRegions_blob(float thresh);
		void splitToRegions_kmeans(int numberRegions);

		float *classified;

	protected:
		SOM(const SOM& som);
		const SOM& operator=(const SOM& som);

		int m_status;                   //SOM status after loading

		int m_dimensionality;           //dimension of SOM
		int *m_dimensions;              //dimensions sizes
		int m_weights_per_node;         //number of weights per node

		std::vector<Node *> m_nodes;         //array of nodes

		enum Normalization m_normalization;                     //normalization type
		enum Node::DistanceMetric m_distance_metric;            //distance metric used in training
		enum TrainMode m_train_mode;                            //slow/fast mode training

		float *m_add;                   //normalization params for input data   (x+add)*mul
		float *m_mul;                   //normalization params for input data
		float *m_data;                  //data vector to classify

		void create_nodes(const float *add = 0, const float *mul = 0);    //random weights for nodes init to m_nodes array
		const float* normalize(const float *vec);

		Node *get_bmu_node(const float *vec);                  //get best node (neuron that fire)
		Node *get_bmu_0node(const float *vec);                 //get best node if its m_class=0 (unclassified neuron)

		float m_novelty_increase;
		float m_novelty_decrease;

		int m_number_regions;

		std::vector<int> regionEquivelents;
		int getEquivelent(int region);
		void setEquivelent(int region, int regionSameAs);

};

inline int SOM::status(void) const
{
	return m_status;
}

inline int SOM::get_dimensionality(void) const
{
	return m_dimensionality;
}

inline int SOM::get_weights_per_node(void) const
{
	return m_weights_per_node;
}

inline int SOM::get_nodes_number() const
{
	return (int)m_nodes.size();
}

inline const Node* SOM::get_node(int n) const
{
	return m_nodes[n];
}

inline void SOM::set_distance_metric(enum Node::DistanceMetric distance_metric)
{
	m_distance_metric = distance_metric;
}

inline void SOM::set_train_mode(enum TrainMode train_mode)
{
	m_train_mode = train_mode;
}

inline Node *SOM::get_bmu_node(const float *vec)
{
	Node *pbmu_node = m_nodes[0];
	float mindist = pbmu_node->get_distance(vec, m_distance_metric);
	float dist;

	for (int i = 1; i < get_nodes_number(); i++) {
		if ((dist = m_nodes[i]->get_distance(vec, m_distance_metric)) < mindist) {
			mindist = dist;
			pbmu_node = m_nodes[i];
		}
	}

	return pbmu_node;
}

inline Node *SOM::get_bmu_0node(const float *vec)
{
	Node *pbmu_0node = 0;
	int n;
	for (n = 0; n < get_nodes_number(); n++) {
		if (m_nodes[n]->get_class() == 0) {
			pbmu_0node = m_nodes[n];
			break;
		}
	}

	if (pbmu_0node != 0) { //there is 0class node m_nodes[n]
		float mindist = pbmu_0node->get_distance(vec, m_distance_metric), dist;
		for (int i = n + 1; i < get_nodes_number(); i++) {
			if (m_nodes[i]->get_class() == 0 &&
						 (dist = m_nodes[i]->get_distance(vec, m_distance_metric)) < mindist) {
				mindist = dist;
				pbmu_0node = m_nodes[i];
						 }
		}
		return pbmu_0node;
	} else
		return 0;
}

#endif
