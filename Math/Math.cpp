/*
 * Math.cpp
 *
 *  Created on: 08-May-2009
 *      Author: chris
 */

#include "Math.h"
#include <math.h>


void largest_factors(int x, int *a, int *b) {
	float s=sqrt(x);
	if ((x % (int)s) == 0) {
		*a=*b=s;
		return;
	}
	for (int n=s;n>0;n--) {
		if ((x % n) == 0) {
			*a=n;
			*b=x/n;
			return;
		}
	}
}


float angle_fix_rad(float &angle) {
	if (angle>M_PI)
		angle=-(2.0*M_PI-angle);
	if (angle<-M_PI)
		angle=2.0*M_PI+angle;
	return angle;
}



float absolute_angle_difference(float a, float b) {
	if (a<0) a+=M_2PI;
	if (b<0) b+=M_2PI;
	// Now angles range from 0 to 2PI
	float diff=fabs(a-b);
	if (diff>M_PI)
		diff=M_2PI-diff;
	return diff;
}

float angle_difference(float a, float b){
	if (a<0) a+=M_2PI;
	if (b<0) b+=M_2PI;
	// Now angles range from 0 to 2PI
	float diff=(a-b);
	if (diff>M_PI)
		diff=M_2PI-diff;
	else if (diff<-M_PI)
		diff=-M_2PI-diff;
	return diff;
}

int pascal_number(int n, int k) {
	int *numbers = new int[n+1];
	memset(numbers,0,sizeof(int)*n+1);
	int tmp,tmp2;
	numbers[0]=numbers[1]=1;
	for (int row=2;row<=n;row++) {
		tmp=numbers[0];
//		printf("1\t");
		for (int entry = 1; entry < row+1; entry++) {
			tmp2 = numbers[entry];
			numbers[entry] = tmp + numbers[entry];
//			printf("%3d\t",numbers[entry]);
			tmp = tmp2;
		}
//		printf("\n");
	}
	tmp = numbers[k];
	delete numbers;
	return tmp;
}

