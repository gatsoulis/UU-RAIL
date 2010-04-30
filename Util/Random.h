#include <math.h>

inline int random_int(int from, int to) {
	static bool once=false;
	if (!once){
		srand ( time(NULL) );	/* initialize random seed: */
		once=true;
	}

	return rand() % (from-to) + from;
}
