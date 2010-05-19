#include "som.h"
#include <values.h>
#include <math.h>
#include <iostream>
#include <stdio.h>

#define min(x,y) (x<=y?x:y)

const char SOM::g_distance[5][5] = {"Eucl", "Sosd", "Txcb", "Angl", "Mhln"};
const char SOM::g_normalization[5][5] = {"None", "Mnmx", "Zscr", "Sigm", "Enrg"};
const char SOM::g_trainmode[2][5] = {"Slow", "Fast"};


///////////////////////////////////SOM  constructor/destructor///////////////////////////////////////////
SOM::SOM(int dimensionality, int *dimensions, int weights_per_node,  //example: 3 [10,30,20] 49
         enum Node::DistanceMetric distance_metric,
         float *add, float *mul) : m_status(1),
                                   m_dimensionality(dimensionality), m_dimensions(0), m_weights_per_node(weights_per_node),
				   m_normalization(NONE), m_distance_metric(distance_metric), m_train_mode(SLOW), m_add(0), m_mul(0), m_data(0)
{
        m_dimensions = (int *)malloc(m_dimensionality * sizeof(int));
        for (int d = 0; d < m_dimensionality; d++)
                m_dimensions[d] = dimensions[d];

        m_data = (float *)malloc(weights_per_node * sizeof(float));

        create_nodes(add, mul);
	m_novelty_increase = 0;
	m_novelty_decrease = 0;

	distancemap=new float[m_dimensions[1]*m_dimensions[0]];

	m_number_regions=0;

	classified = new float[m_dimensions[1]*m_dimensions[0]];
}

/*
   status = 0 if complete file format was loaded
   status = 1 if reduced file format was loaded, random weights
   status = -1 if failed to load any file format
*/
SOM::SOM(const char *fname): m_status(-1), m_dimensionality(0), m_dimensions(0), m_weights_per_node(0),
                                m_normalization(NONE), m_distance_metric(Node::EUCL), m_train_mode(SLOW),
                                m_add(0), m_mul(0), m_data(0)

{
        char str[256] = "";

        FILE *fp = fopen(fname, "rt");
        if (fp != 0) {
                if (fscanf(fp, "%d", &m_dimensionality) != 1)
                        return;

                int nodes_number = 0;
                m_dimensions = (int *)malloc(m_dimensionality * sizeof(int));
                for (int i = 0; i < m_dimensionality; i++) {
                        if (fscanf(fp, "%d", &m_dimensions[i]) != 1) {
                                fclose(fp);
                                return;
                        }
                        if (i != 0)
                                nodes_number *= m_dimensions[i];
                        else
                                nodes_number = m_dimensions[i];
                }

                if (fscanf(fp, "%d", &m_weights_per_node) != 1) {
                        fclose(fp);
                        return;
                }
                //m_dimensionality,m_dimensions,m_weights_per_node arranged////////////////////////////

                m_data = (float *)malloc(get_weights_per_node() * sizeof(float));

                //check for distance
                if (fscanf(fp, "%s", str) != 1) {
                        create_nodes();
                        m_status = 1;
                        fclose(fp);
                        return;
                }

                //default eucl=0
                if (strcmp(str, g_distance[Node::EUCL]) == 0)
                        m_distance_metric = Node::EUCL;
                else if (strcmp(str, g_distance[Node::SOSD]) == 0)
                        m_distance_metric = Node::SOSD;
                else if (strcmp(str, g_distance[Node::TXCB]) == 0)
                        m_distance_metric = Node::TXCB;
                else if (strcmp(str, g_distance[Node::ANGL]) == 0)
                        m_distance_metric = Node::ANGL;

                //check for classes info
                std::vector<int> classes;
                for (int i = 0; i < nodes_number; i++) {
                        int cls;
                        if (fscanf(fp, "%d", &cls) != 1)
                                break;
                        classes.push_back(cls);
                }

                if ((int)classes.size() == nodes_number) {  //load positions/weights
                        //if classes marks present but norm is absent m_status=-1
                        if (fscanf(fp, "%s", str) != 1) {
                                fclose(fp);
                                return;
                        }
                        if (strcmp(str, g_normalization[NONE]) == 0)
                                m_normalization = NONE;
                        else if (strcmp(str, g_normalization[MNMX]) == 0)
                                m_normalization = MNMX;
                        else if (strcmp(str, g_normalization[ZSCR]) == 0)
                                m_normalization = ZSCR;
                        else if (strcmp(str, g_normalization[SIGM]) == 0)
                                m_normalization = SIGM;
                        else if (strcmp(str, g_normalization[ENRG]) == 0)
                                m_normalization = ENRG;

                        m_add = (float *)malloc(m_weights_per_node * sizeof(float));
                        m_mul = (float *)malloc(m_weights_per_node * sizeof(float));
                        for (int i = 0; i < m_weights_per_node; i++) {
                                if (fscanf(fp, "%g %g", &m_add[i], &m_mul[i]) != 2) {
                                        fclose(fp);
                                        return;
                                }
                        }

                        //if classes marks present but coords/weights corrupted m_status=-1
                        float *weights = (float *)malloc(m_weights_per_node * sizeof(float));
                        float *coords = (float *)malloc(m_dimensionality * sizeof(float));

			unsigned int id = 0;
                        for (int i = 0; i < nodes_number; i++) {
                                for (int p = 0; p < m_dimensionality; p++)
                                        if (fscanf(fp, "%g", &coords[p]) != 1) {
                                                fclose(fp);
                                                return;
                                        }
                                for (int w = 0; w < m_weights_per_node; w++)
                                        if (fscanf(fp, "%g", &weights[w]) != 1) {
                                                fclose(fp);
                                                return;
                                        }

                                Node *pnode = new Node(id++,weights, m_weights_per_node, coords, m_dimensionality, classes[i]);
                                m_nodes.push_back(pnode);
                        }

                        free(weights);
                        free(coords);

                        fclose(fp);
                        m_status = 0;
                } else {        //corrupted classes
                        fclose(fp);
                        return;
                }
        }

    	distancemap=new float[m_dimensions[1]*m_dimensions[0]];
    	classified = new float[m_dimensions[1]*m_dimensions[0]];
    	m_number_regions=0;
}

SOM::~SOM()
{
        for (int i = 0; i < get_nodes_number(); i++)
                delete m_nodes[i];

        if (m_dimensions != 0)
                free(m_dimensions);
        if (m_add != 0)
                free(m_add);
        if (m_mul != 0)
                free(m_mul);
        if (m_data != 0)
                free(m_data);

        delete classified;
}
///////////////////////////////////SOM  constructor/destructor///////////////////////////////////////////




/////////////////////////////init nodes to random weights///////////////////////////////////
void SOM::create_nodes(const float *add, const float *mul)
{
        //normalization parameters////////////////////////////////////
        m_add = (float *)malloc(m_weights_per_node * sizeof(float));
        if (add != 0)
                for (int w = 0; w < m_weights_per_node; w++)
                        m_add[w] = add[w];
        else
                for (int w = 0; w < m_weights_per_node; w++)
                        m_add[w] = 0.0f;

        m_mul = (float *)malloc(m_weights_per_node * sizeof(float));
        if (mul != 0)
                for (int w = 0; w < m_weights_per_node; w++)
                        m_mul[w] = mul[w];
        else
                for (int w = 0; w < m_weights_per_node; w++)
                        m_mul[w] = 1.0f;


        //weights and positions/////////////////////////////////////////
        float *weights = (float *)malloc(m_weights_per_node * sizeof(float));
        float *coords = (float *)malloc(m_dimensionality * sizeof(float));
        for (int i = 0; i < m_dimensionality; i++)
                coords[i] = 0.0f;

        //fill coords with all posible combinations
        srand((unsigned int)time(0));
	unsigned int id = 0;
        while (true) {
                //randomize weights
                for (int i = 0; i < m_weights_per_node; i++) {
                        int w = 0xFFF & rand();
                        w -= 0x800;
                        weights[i] = (float)w / 4096.0f;
                }

                Node *pnode = new Node(id, weights, m_weights_per_node, coords, m_dimensionality);
		id++;
                m_nodes.push_back(pnode);

                //increment counter
                for (int i = 0; i < m_dimensionality; i++) {
                        coords[i]++;
                        if (coords[i] >= m_dimensions[i])
                                coords[i] = 0.0f;
                        else
                                break;
                }

                float sum = 0.0f;
                for (int i = 0; i < m_dimensionality; i++)
                        sum += coords[i];
                if (sum == 0.0f)
                        break;
        }

        free(weights);
        free(coords);
}


////////////////////////////////////////////////////////////////////////////////////////////
/*
      [SOM file format]

       dimensionality
       dim1 dim2 dim3 ...
       weight_per_node
       distance_metric

       classes

       normalization params

       weights coords/coeffs
       node coord 0 0 0 ... dim
        weights
       node coord 1 0 0 ... dim
        weights
       node coord 2 0 0 ... dim
        weights
       ...
                                        */
int SOM::save(const char *fname) const
{
        FILE *fp = fopen(fname, "wt");
        if (fp != 0) {
                fprintf(fp, "%d\n", m_dimensionality);
                for (int i = 0; i < m_dimensionality; i++)
                        fprintf(fp, "%d ", m_dimensions[i]);
                fprintf(fp, "\n%d\n%s\n\n", m_weights_per_node, g_distance[m_distance_metric]);

                //save classes numbers of nodes
                for (int n = 0; n < get_nodes_number(); n++) {
                        Node *pnode = m_nodes[n];
                        fprintf(fp, " %d", pnode->get_class());

                        if (!((n + 1) % m_dimensions[0]))
                                fprintf(fp, "\n");
                }
                fprintf(fp, "\n");

                //save norm parameters
                fprintf(fp, "%s\n", g_normalization[m_normalization]);
                for (int i = 0; i < m_weights_per_node; i++)
                        fprintf(fp, "%g\t\t%g\n", m_add[i], m_mul[i]);
                fprintf(fp, "\n");

                //save nodes positions and weights
                for (int n = 0; n < get_nodes_number(); n++) {
                        Node *pnode = m_nodes[n];
                        for (int c = 0; c < m_dimensionality; c++)
                                fprintf(fp, "%g ", pnode->m_coords[c]);
                        fprintf(fp, "\n");

                        for (int w = 0; w < m_weights_per_node; w++)
                                fprintf(fp, "%g\n", pnode->m_weights[w]);
                        fprintf(fp, "\n");
                }

                fclose(fp);
                return 0;
        } else
                return -1;
}

int SOM::save_2D_distance_map(const char *fname) const
{
        int D = 2;
        float min_dist = 1.5f;

        if (get_dimensionality() != D)
                return -1;

        FILE *fp = fopen(fname, "wt");
        if (fp != 0) {
                int n = 0;
                for (int i = 0; i < m_dimensions[0]; i++) {
                        for (int j = 0; j < m_dimensions[1]; j++) {
                                float dist = 0.0f;
                                int nodes_number = 0;
                                const Node *pnode = get_node(n++);
                                for (int m = 0; m < get_nodes_number(); m++) {
                                        const Node *node = get_node(m);
                                        if (node == pnode)
                                                continue;
                                        float tmp = 0.0;
                                        for (int x = 0; x < D; x++)
                                                tmp += pow(*(pnode->get_coords() + x) - *(node->get_coords() + x), 2.0f);
                                        tmp = sqrt(tmp);
                                        if (tmp <= min_dist) {
                                                nodes_number++;
                                                dist += pnode->get_distance(node->m_weights, m_distance_metric);
                                        }
                                }
                                dist /= (float)nodes_number;
                                fprintf(fp, " %f", dist);
                        }
                        fprintf(fp, "\n");
                }
                fclose(fp);
                return 0;
        }
        else
                return -2;
}
// Changed from normal distance map to distance map for current weights
int SOM::compute_distance_map(const float *vec)
{
    const float *pdata = normalize(vec);
	int D = 2;
//	float min_dist = 1.5f;

	if (get_dimensionality() != D)
		return -1;

	int n = 0;
	for (int i = 0; i < m_dimensions[0]; i++) {
		for (int j = 0; j < m_dimensions[1]; j++) {
			float dist = 0.0f;
//			int nodes_number = 0;
			const Node *pnode = get_node(n++);
//			for (int m = 0; m < get_nodes_number(); m++) {
//				const Node *node = get_node(m);
//				if (node == pnode)
//				continue;
//				float tmp = 0.0;
//				for (int x = 0; x < D; x++)
//					tmp += pow(*(pnode->get_coords() + x) - *(node->get_coords() + x), 2.0f);
//					tmp = sqrt(tmp);
//					if (tmp <= min_dist) {
//					nodes_number++;
					dist = pnode->get_distance(pdata, m_distance_metric);
//				}
//			}
//			dist /= (float)nodes_number;
			distancemap[i*m_dimensions[0] + j]=dist;
			//                                fprintf(fp, " %f", dist);
		}
		//                        fprintf(fp, "\n");
	}
    // Normalise the map
	float max=1.0;
	for (int i=0; i<m_dimensions[1]*m_dimensions[0];i++)
		if (max<distancemap[i])
			max=distancemap[i];
	for (int i=0; i<m_dimensions[1]*m_dimensions[0];i++)
		distancemap[i]/=max;
}

////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////normalization//////////////////////////////////////////////////////////
void SOM::compute_normalization(PREC rec, enum Normalization norm)
{
        m_normalization = norm;

        //calculate disp,mean,min,max
        if (m_normalization == MNMX) {
                for (int x = 0; x < get_weights_per_node(); x++) {
                        m_add[x] = FLT_MAX;     //min
                        m_mul[x] = -FLT_MAX;    //max
                }
                for (int y = 0; y < (int)rec->entries.size(); y++) {
                        if (rec->entries[y] == 0)
                                continue;
                        for (int x = 0; x < get_weights_per_node(); x++) {
                                if (m_add[x] > rec->entries[y]->vec[x]) m_add[x] = rec->entries[y]->vec[x];   //min
                                if (m_mul[x] < rec->entries[y]->vec[x]) m_mul[x] = rec->entries[y]->vec[x];   //max
                        }
                }
                for (int x = 0; x < get_weights_per_node(); x++) {
                        float mul, add;
                        add = -m_add[x];
                        if ((m_mul[x] - m_add[x]) != 0.0f)
                                mul = 1.0f / (m_mul[x] - m_add[x]);
                        else
                                mul = 1.0f;
                        m_add[x] = add;
                        m_mul[x] = mul;
                }
        } else if (m_normalization == ZSCR || m_normalization == SIGM) {
                for (int x = 0; x < get_weights_per_node(); x++) {
                        m_add[x] = 0.0f;
                        m_mul[x] = 0.0f;
                }

                //mean values
                int size = 0;
                for (int y = 0; y < (int)rec->entries.size(); y++) {
                        if (rec->entries[y] == 0)
                                continue;
                        for (int x = 0; x < get_weights_per_node(); x++)
                                m_add[x] += rec->entries[y]->vec[x];
                        size++;
                }
                for (int x = 0; x < get_weights_per_node(); x++)   //m_add = mean values
                        m_add[x] /= (float)size;

                //dispersions
                for (int y = 0; y < (int)rec->entries.size(); y++) {
                        if (rec->entries[y] == 0)
                                continue;
                        for (int x = 0; x < rec->entries[y]->size; x++)
                                m_mul[x] += (rec->entries[y]->vec[x] - m_add[x]) * (rec->entries[y]->vec[x] - m_add[x]);
                }

                //m_add; m_mul
                for (int x = 0; x < get_weights_per_node(); x++) { //m_add = -mean values;  m_mul = 1/disp
                        float disp = sqrt(m_mul[x] / float(size - 1));
                        if (disp != 0.0f)
                                m_mul[x] = 1.0f / disp;
                        else
                                m_mul[x] = 1.0f;
                        m_add[x] = -m_add[x];
                }
        }
}

const float* SOM::normalize(const float *vec)
{
        switch (m_normalization) {
        default:
        case NONE:
                for (int x = 0; x < get_weights_per_node(); x++)
                        m_data[x] = vec[x];
                break;

        case MNMX:
                for (int x = 0; x < get_weights_per_node(); x++)
                        m_data[x] = (0.9f - 0.1f) * (vec[x] + m_add[x]) * m_mul[x] + 0.1f;
                break;

        case ZSCR:
                for (int x = 0; x < get_weights_per_node(); x++)
                        m_data[x] = (vec[x] + m_add[x]) * m_mul[x];
                break;

        case SIGM:
                for (int x = 0; x < get_weights_per_node(); x++)
                        m_data[x] = 1.0f / (1.0f + exp(-((vec[x] + m_add[x]) * m_mul[x])));
                break;

        case ENRG:
                float energy = 0.0f;
                for (int x = 0; x < get_weights_per_node(); x++)
                        energy += vec[x] * vec[x];
                energy = sqrt(energy);

                for (int x = 0; x < get_weights_per_node(); x++)
                        m_data[x] = vec[x] / energy;
                break;
        }
        return m_data;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////





/////////////////////calculate initial R0 for 1st epoch/////////////////////////////////////////
float SOM::R0() const
{
        float R = 0.0f;

        for (int i = 0; i < m_dimensionality; i++)
                if ((float)m_dimensions[i] > R)
                        R = (float)m_dimensions[i];

        return R / 2.0f;
}
////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////training///////////////////////////////////////////////////////////////
/*void SOM::train(const std::vector<float *> &vectors, float R, float learning_rule)
{
        for (int n = 0; n < (int)vectors.size(); n++) {

                const float *pdata = normalize(vectors.at(n));

                //get best node
                Node *bmu_node = get_bmu_node(pdata);
                const float *p1 = bmu_node->get_coords();

                if (R <= 1.0f)  //adjust BMU node only
                        bmu_node->train(pdata, learning_rule);
                else {
                        for (int i = 0; i < get_nodes_number(); i++) { //adjust weights within R
                                const float *p2 = m_nodes[i]->get_coords();
                                float dist = 0.0f;

                                for (int p = 0; p < m_dimensionality; p++)     //dist = sqrt((x1-y1)^2 + (x2-y2)^2 + ...)  distance to node
                                        dist += (p1[p] - p2[p]) * (p1[p] - p2[p]);
                                dist = sqrt(dist);

                                if (m_train_mode == FAST && dist > R)
                                        continue;

                                float y = exp(-(1.0f * dist * dist) / (R * R));
                                m_nodes[i]->train(pdata, learning_rule * y);
                        }
                }
        }
}*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//////VOTING scheme best node to a vector
void SOM::vote_nodes_from(PREC rec)
{
        //rec->clsnum = [cls 1][cls 2] ... [cls N]   N entries   examle: 0,1,2  3,1,2   1,4,10 missed classes

        //clear votes for classes of all nodes
        for (int n = 0; n < get_nodes_number(); n++)
                m_nodes[n]->clear_votes((int)rec->clsnum.size());

        while (true) { //untill unclassified nodes m_class=0
                //add vote to a best node for a given class
                for (int y = 0; y < (int)rec->entries.size(); y++) {
                        if (rec->entries[y] == 0)
                                continue;

                        const float *pdata = normalize(rec->entries[y]->vec);
                        Node *pbmu_0node = get_bmu_0node(pdata);

                        //no more m_class=0 nodes
                        if (pbmu_0node == 0)
                                return;

                        //check class location in REC->clsnum[] array
                        int c = rec->entries[y]->cls;
                        for (int i = 0; i < (int)rec->clsnum.size(); i++) {
                                if (rec->clsnum[i] == c) {
                                        c = i;
                                        break;
                                }
                        }

                        pbmu_0node->add_vote(c);
                }

                //assign class from the max number of votes for a class
                for (int n = 0; n < get_nodes_number(); n++) {
                        if (m_nodes[n]->get_class() == 0)
                                m_nodes[n]->evaluate_class(&rec->clsnum[0], (int)rec->clsnum.size());
                }
        }
}

//////DIRECT scheme best vector to a node
void SOM::assign_nodes_to(PREC rec)
{
        //run thru nodes and get best vector matching
        for (int n = 0; n < get_nodes_number(); n++) {
                m_nodes[n]->clear_votes();

                int index = 0;
                float mindist = FLT_MAX, dist;
                for (int i = 0; i < (int)rec->entries.size(); i++) {
                        if (rec->entries[i] == 0)
                                continue;
                        const float *pdata = normalize(rec->entries[i]->vec);
                        if ((dist = m_nodes[n]->get_distance(pdata, m_distance_metric)) < mindist) {
                                mindist = dist;
                                index = i;
                        }
                }

                m_nodes[n]->set_class(rec->entries[index]->cls);
        }
}
/////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////testing///////////////////////////////////////////////////////
const Node *SOM::classify(const float *vec)
{
        Node *pbmu_node = m_nodes[0];
        const float *pdata = normalize(vec);
        float mindist = pbmu_node->get_distance(pdata, m_distance_metric), dist;
        classified[0]=mindist;
        for (int n = 1; n < get_nodes_number(); n++) {
                if ((dist = m_nodes[n]->get_distance(pdata, m_distance_metric)) < mindist) {
                        mindist = dist;
                        pbmu_node = m_nodes[n];
                }
                classified[n]=dist;
        }
        pbmu_node->fireCount+=1;
        return pbmu_node;
}
//////////////////////////////////////////////////////////////////////////////////////////////////


/*!
    \fn SOM::find_node(float* coords) const
 */
const Node* SOM::find_node(float* coords) const
{
	std::vector<Node*>::const_iterator n;
	for (n = m_nodes.begin(); n != m_nodes.end(); n++) {
		if ((*n)->check_coords(coords))
			return (*n);
	}
	return NULL;
}

Node* SOM::find_node(float* coords)
{
	std::vector<Node*>::const_iterator n;
	for (n = m_nodes.begin(); n != m_nodes.end(); n++) {
		if ((*n)->check_coords(coords))
			return (*n);
	}
	return NULL;
}

/*!
    \fn SOM::train(const std::vector<float *> &vectors, float R, float learning_rule, bool* fold)
 */
void SOM::train(const std::vector<float *> &vectors, float R, float learning_rule, bool* fold)
{
	for (int n = 0; n < (int)vectors.size(); n++) {

		const float *pdata = normalize(vectors.at(n));

                //get best node
		Node *bmu_node = get_bmu_node(pdata);
		//novelty
		bmu_node->decrease_novelty(m_novelty_decrease);
		bmu_node->train(pdata, learning_rule);
		const float *p1 = bmu_node->get_coords();

		if (R > 1.0)  {
			for (int i = 0; i < get_nodes_number(); i++) { //adjust weights within R
				if (m_nodes[i] == bmu_node) {
					continue;
				}
				const float *p2 = m_nodes[i]->get_coords();
				float dist = 0.0f;
				float dist1 = INFINITY;
				float dist2 = INFINITY;
				float dist3 = INFINITY;
				for (int p = 0; p < m_dimensionality; p++) {
					dist1 = (p1[p] - p2[p]) * (p1[p] - p2[p]); //ordinary distance
					if ((fold == NULL) || (fold[p])) { //folded distance
						dist2 = (p1[p] - (p2[p] - m_dimensions[p])) * (p1[p] - (p2[p] - m_dimensions[p])); //shift backward
						dist3 = (p1[p] - (p2[p] + m_dimensions[p])) * (p1[p] - (p2[p] + m_dimensions[p])); //shift forward
					}
					else {
						dist2 = INFINITY;
						dist3 = INFINITY;
					}
					dist += min(dist1,min(dist2,dist3));
				}
				dist = sqrt(dist);

// 				if (m_train_mode == FAST && dist > R)
// 					continue;


				float y = exp(-(1.0 * dist * dist) / (R * R));
				m_nodes[i]->train(pdata, learning_rule * y);
// 				if (dist > R)
					m_nodes[i]->increase_novelty(m_novelty_increase);
// 				else
// 					m_nodes[i]->decrease_novelty(m_novelty_decrease*exp(-dist));
			}
		}
	}
}

int SOM::getEquivelent(int region) {
	if (regionEquivelents[region]==-1)
		return region;
	else
		return regionEquivelents[region];
}

// Sets region to be the ame as regionSameAs
void SOM::setEquivelent(int region, int regionSameAs) {
	int temp;
	if (region<regionSameAs) {
		temp=region;
		region=regionSameAs;
		regionSameAs=temp;
	}
	regionEquivelents[region]=regionSameAs;
	for (unsigned int i=0;i<regionEquivelents.size(); i++) {
		if (regionEquivelents[i]==region)
			regionEquivelents[i]=regionSameAs;
	}
//	printf("Merged regions\n");
}

void SOM::splitToRegions_blob(float thresh) {
#define DBG(X) printf(X);
	regionEquivelents.clear();
	// set all the nodes to non tagged state
	for (unsigned int i=0; i<m_nodes.size(); i++) {
		m_nodes.at(i)->m_tagged=false;
	}

	float coords[2];
	float left_neighbour_coords[2];
	float up_neighbour_coords[2];
	float distance, distanceL, distanceU;
	int latestID=0;
	int noTaggedNeighbours=0;
	for (coords[1]=0;coords[1]<m_dimensions[1]; coords[1]++) {
		for (coords[0]=0; coords[0]<m_dimensions[0];coords[0]++) {
			left_neighbour_coords[0]=coords[0]-1;
			left_neighbour_coords[1]=coords[1];
			up_neighbour_coords[0]=coords[0];
			up_neighbour_coords[1]=coords[1]-1;
			if (left_neighbour_coords[0]<0)
				left_neighbour_coords[0]=m_dimensions[0]-1;
			if (up_neighbour_coords[1]<0)
				up_neighbour_coords[1]=m_dimensions[1]-1;

			// See which type of point it is:
			//	(a) a point that has no tagged left and top neighbour	= 1
			//	(b) a point that has only a left tagged neighbour		= 2
			//	(c) a point that has only a top tagged neighbour		= 3
			//	(d) a point that has both tagged neighbours				= 4
			if (!(find_node(up_neighbour_coords)->m_tagged) && !(find_node(left_neighbour_coords)->m_tagged)) {
				// situation (a)
				// must be a new region
				find_node(coords)->set_region(latestID);
				regionEquivelents.push_back(-1);
				latestID++;
			} else if ((find_node(up_neighbour_coords)->m_tagged) && (find_node(left_neighbour_coords)->m_tagged)) {
				// situation (d)
//				DBG("situation (d)");
				// the usual situation. check for similarity to neighbours, merge regions if needed.
				// situations:
				//  (a) the point is similar to both up and left, and bridges two different regions
				//  (b) the point is similar to either/both up and left and both up and left are the same region
				//  (c) the point is similar to either left or up, but not both, and each is a different region
				//  (d) the point is not similar to anything and is a new region
				if (getEquivelent(find_node(up_neighbour_coords)->m_region)==getEquivelent(find_node(left_neighbour_coords)->m_region)) {
					// both neighbours are the same region
					// if the point is similar to either then set it to match, otherwise a new region
					distance=min((find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric)),
							(find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric)));
//					printf("Distance = %f\n",distance);
					if (distance < thresh){
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
					} else {
						find_node(coords)->set_region(latestID);
						regionEquivelents.push_back(-1);
						latestID++;
					}
				} else {
					// both neighbours are not the same regions, possible bridgeing situation
					// the possible situations:
					//  (a) similar to both, bridge the regions
					//	(b) similar to one of, make same region
					//  (c) not same, new region
					distanceU=find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
					distanceL=find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
					if ((distanceU<thresh)&&(distanceL<thresh)) {
						// situation (a): bridge left & upper
						// use a region equivelence table to remember that actually the two regions are the same
						setEquivelent(getEquivelent(find_node(up_neighbour_coords)->m_region),getEquivelent(find_node(left_neighbour_coords)->m_region));
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));

					} else if (distanceU<thresh) {
						// situation (b): part of upper
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
					} else if (distanceL<thresh) {
						// situation (b): part of left
						find_node(coords)->set_region(getEquivelent(find_node(left_neighbour_coords)->m_region));
					} else {
						// situation (c): new region
						find_node(coords)->set_region(latestID);
						regionEquivelents.push_back(-1);
						latestID++;
					}
//					find_node(coords)->m_tagged=true;
				}

			} else if ((find_node(up_neighbour_coords)->m_tagged)) {
				// situation (c)
				distance=find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
				if (distance < thresh){
					find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
				} else {
					find_node(coords)->set_region(latestID);
					regionEquivelents.push_back(-1);
					latestID++;
				}
				find_node(coords)->m_tagged=true;
			} else {
				// situation (b)
				distance=find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
//				printf( "Distance = %f\n",distance);
				if (distance < thresh){
					find_node(coords)->set_region(getEquivelent(find_node(left_neighbour_coords)->m_region));
				} else {
					find_node(coords)->set_region(latestID);
					regionEquivelents.push_back(-1);
					latestID++;
				}
				find_node(coords)->m_tagged=true;
			}

		}
	}

	// Do another loop through the top row and left column so the wrap around works
	for (coords[1]=0;coords[1]<1; coords[1]++) {
		for (coords[0]=0; coords[0]<m_dimensions[0];coords[0]++) {
			left_neighbour_coords[0]=coords[0]-1;
			left_neighbour_coords[1]=coords[1];
			up_neighbour_coords[0]=coords[0];
			up_neighbour_coords[1]=coords[1]-1;
			if (left_neighbour_coords[0]<0)
				left_neighbour_coords[0]=m_dimensions[0]-1;
			if (up_neighbour_coords[1]<0)
				up_neighbour_coords[1]=m_dimensions[1]-1;

			// See which type of point it is:
			//	(a) a point that has no tagged left and top neighbour	= 1
			//	(b) a point that has only a left tagged neighbour		= 2
			//	(c) a point that has only a top tagged neighbour		= 3
			//	(d) a point that has both tagged neighbours				= 4
			if (!(find_node(up_neighbour_coords)->m_tagged) && !(find_node(left_neighbour_coords)->m_tagged)) {
				// situation (a)
				// must be a new region
				find_node(coords)->set_region(latestID);
				regionEquivelents.push_back(-1);
				latestID++;
			} else if ((find_node(up_neighbour_coords)->m_tagged) && (find_node(left_neighbour_coords)->m_tagged)) {
				// situation (d)
//				DBG("situation (d)");
				// the usual situation. check for similarity to neighbours, merge regions if needed.
				// situations:
				//  (a) the point is similar to both up and left, and bridges two different regions
				//  (b) the point is similar to either/both up and left and both up and left are the same region
				//  (c) the point is similar to either left or up, but not both, and each is a different region
				//  (d) the point is not similar to anything and is a new region
				if (getEquivelent(find_node(up_neighbour_coords)->m_region)==getEquivelent(find_node(left_neighbour_coords)->m_region)) {
					// both neighbours are the same region
					// if the point is similar to either then set it to match, otherwise a new region
					distance=min((find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric)),
							(find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric)));
//					printf("Distance = %f\n",distance);
					if (distance < thresh){
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
					} else {
						find_node(coords)->set_region(latestID);
						regionEquivelents.push_back(-1);
						latestID++;
					}
				} else {
					// both neighbours are not the same regions, possible bridgeing situation
					// the possible situations:
					//  (a) similar to both, bridge the regions
					//	(b) similar to one of, make same region
					//  (c) not same, new region
					distanceU=find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
					distanceL=find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
					if ((distanceU<thresh)&&(distanceL<thresh)) {
						// situation (a): bridge left & upper
						// use a region equivelence table to remember that actually the two regions are the same
						setEquivelent(getEquivelent(find_node(up_neighbour_coords)->m_region),getEquivelent(find_node(left_neighbour_coords)->m_region));
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));

					} else if (distanceU<thresh) {
						// situation (b): part of upper
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
					} else if (distanceL<thresh) {
						// situation (b): part of left
						find_node(coords)->set_region(getEquivelent(find_node(left_neighbour_coords)->m_region));
					} else {
						// situation (c): new region
						find_node(coords)->set_region(latestID);
						regionEquivelents.push_back(-1);
						latestID++;
					}
//					find_node(coords)->m_tagged=true;
				}

			} else if ((find_node(up_neighbour_coords)->m_tagged)) {
				// situation (c)
				distance=find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
				if (distance < thresh){
					find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
				} else {
					find_node(coords)->set_region(latestID);
					regionEquivelents.push_back(-1);
					latestID++;
				}
				find_node(coords)->m_tagged=true;
			} else {
				// situation (b)
				distance=find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
//				printf( "Distance = %f\n",distance);
				if (distance < thresh){
					find_node(coords)->set_region(getEquivelent(find_node(left_neighbour_coords)->m_region));
				} else {
					find_node(coords)->set_region(latestID);
					regionEquivelents.push_back(-1);
					latestID++;
				}
				find_node(coords)->m_tagged=true;
			}

		}
	}
	for (coords[1]=0;coords[1]<m_dimensions[1]; coords[1]++) {
		for (coords[0]=0; coords[0]<1;coords[0]++) {
			left_neighbour_coords[0]=coords[0]-1;
			left_neighbour_coords[1]=coords[1];
			up_neighbour_coords[0]=coords[0];
			up_neighbour_coords[1]=coords[1]-1;
			if (left_neighbour_coords[0]<0)
				left_neighbour_coords[0]=m_dimensions[0]-1;
			if (up_neighbour_coords[1]<0)
				up_neighbour_coords[1]=m_dimensions[1]-1;

			// See which type of point it is:
			//	(a) a point that has no tagged left and top neighbour	= 1
			//	(b) a point that has only a left tagged neighbour		= 2
			//	(c) a point that has only a top tagged neighbour		= 3
			//	(d) a point that has both tagged neighbours				= 4
			if (!(find_node(up_neighbour_coords)->m_tagged) && !(find_node(left_neighbour_coords)->m_tagged)) {
				// situation (a)
				// must be a new region
				find_node(coords)->set_region(latestID);
				regionEquivelents.push_back(-1);
				latestID++;
			} else if ((find_node(up_neighbour_coords)->m_tagged) && (find_node(left_neighbour_coords)->m_tagged)) {
				// situation (d)
//				DBG("situation (d)");
				// the usual situation. check for similarity to neighbours, merge regions if needed.
				// situations:
				//  (a) the point is similar to both up and left, and bridges two different regions
				//  (b) the point is similar to either/both up and left and both up and left are the same region
				//  (c) the point is similar to either left or up, but not both, and each is a different region
				//  (d) the point is not similar to anything and is a new region
				if (getEquivelent(find_node(up_neighbour_coords)->m_region)==getEquivelent(find_node(left_neighbour_coords)->m_region)) {
					// both neighbours are the same region
					// if the point is similar to either then set it to match, otherwise a new region
					distance=min((find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric)),
							(find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric)));
//					printf("Distance = %f\n",distance);
					if (distance < thresh){
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
					} else {
						find_node(coords)->set_region(latestID);
						regionEquivelents.push_back(-1);
						latestID++;
					}
				} else {
					// both neighbours are not the same regions, possible bridgeing situation
					// the possible situations:
					//  (a) similar to both, bridge the regions
					//	(b) similar to one of, make same region
					//  (c) not same, new region
					distanceU=find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
					distanceL=find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
					if ((distanceU<thresh)&&(distanceL<thresh)) {
						// situation (a): bridge left & upper
						// use a region equivelence table to remember that actually the two regions are the same
						setEquivelent(getEquivelent(find_node(up_neighbour_coords)->m_region),getEquivelent(find_node(left_neighbour_coords)->m_region));
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));

					} else if (distanceU<thresh) {
						// situation (b): part of upper
						find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
					} else if (distanceL<thresh) {
						// situation (b): part of left
						find_node(coords)->set_region(getEquivelent(find_node(left_neighbour_coords)->m_region));
					} else {
						// situation (c): new region
						find_node(coords)->set_region(latestID);
						regionEquivelents.push_back(-1);
						latestID++;
					}
//					find_node(coords)->m_tagged=true;
				}

			} else if ((find_node(up_neighbour_coords)->m_tagged)) {
				// situation (c)
				distance=find_node(up_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
				if (distance < thresh){
					find_node(coords)->set_region(getEquivelent(find_node(up_neighbour_coords)->m_region));
				} else {
					find_node(coords)->set_region(latestID);
					regionEquivelents.push_back(-1);
					latestID++;
				}
				find_node(coords)->m_tagged=true;
			} else {
				// situation (b)
				distance=find_node(left_neighbour_coords)->get_distance(find_node(coords)->get_weights(),m_distance_metric);
//				printf( "Distance = %f\n",distance);
				if (distance < thresh){
					find_node(coords)->set_region(getEquivelent(find_node(left_neighbour_coords)->m_region));
				} else {
					find_node(coords)->set_region(latestID);
					regionEquivelents.push_back(-1);
					latestID++;
				}
				find_node(coords)->m_tagged=true;
			}

		}
	}

	// Now go through an organise the region identifiers
	// first pass: make the identity the equivelent one
	for (unsigned int i=0; i<m_nodes.size(); i++) {
		m_nodes[i]->m_region=getEquivelent(m_nodes[i]->m_region);
	}
	// Go through the equivelence table and store a list of the number of regions
//	std::vector<int> regions(100);
	latestID=0;
	for (unsigned int i=0; i<regionEquivelents.size();i++) {
//		printf("Region %d=>%d\n",i,regionEquivelents[i]);
		if (regionEquivelents[i] == -1) {
			regionEquivelents[i]=latestID;
			latestID++;
		}
	}
	m_number_regions=latestID;
	// now go through nodes and update region id
	for (unsigned int i=0; i<m_nodes.size(); i++) {
		m_nodes[i]->m_region=regionEquivelents[m_nodes[i]->m_region];
	}
	printf("Blobed and found region count=%d\n",m_number_regions);
//	printf("Found %d nodes to have no tagged left neighbours\n",noTaggedNeighbours);
//	printf("\n\n\n\n\n\n\n\n\n");
//	float coords3[2];
	for (coords[1]=0;coords[1]<m_dimensions[1];coords[1]++) {
		for (coords[0]=0;coords[0]<m_dimensions[0];coords[0]++) {
//			printf("%2d ",find_node(coords)->region());
		}
//		printf("\n");
	}
}

void SOM::splitToRegions_kmeans(int numberRegions) {
	// form an array of the arrays of weights
	int number_nodes=m_nodes.size();
	const float **datapoints = new const float*[number_nodes];
	for (int i=0;i<number_nodes;i++) {
		datapoints[i]=m_nodes[i]->get_weights();
	}

	int *c = k_means(datapoints, number_nodes, m_weights_per_node, numberRegions, 1e-4, 0);
	for (int i=0;i<number_nodes;i++) {
		m_nodes[i]->set_region(c[i]);
	}
	m_number_regions=numberRegions;
//	float coords[2];
//	for (coords[1]=0;coords[1]<m_dimensions[1];coords[1]++) {
//		for (coords[0]=0;coords[0]<m_dimensions[0];coords[0]++) {
//			printf("% 2d ",find_node(coords)->region());
//		}
//		printf("\n");
//	}

}
