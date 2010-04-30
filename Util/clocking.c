#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>

main()
{
	time_t  t0, t1; /* time_t is defined on <time.h> and <sys/types.h> as long */
	clock_t c0, c1; /* clock_t is defined on <time.h> and <sys/types.h> as int */

	long count;
	double a, b, c;

	printf ("using UNIX function time to measure wallclock time ... \n");
	printf ("using UNIX function clock to measure CPU time ... \n");

	t0 = time(NULL);
	c0 = clock();

	printf ("\tbegin (wall):            %ld\n", (long) t0);
	printf ("\tbegin (CPU):             %d\n", (int) c0);

	printf ("\t\tsleep for 5 seconds ... \n");
	sleep(5);

	printf ("\t\tperform some computation ... \n");
	for (count = 1l; count < 10000000l; count++) {
		a = sqrt(count);
		b = 1.0/a;
		c = b - a;
	}

	t1 = time(NULL);
	c1 = clock();

	printf ("\tend (wall):              %ld\n", (long) t1);
	printf ("\tend (CPU);               %d\n", (int) c1);
	printf ("\telapsed wall clock time: %ld\n", (long) (t1 - t0));
	printf ("\telapsed CPU time:        %f\n", (float) (c1 - c0)/CLOCKS_PER_SEC);
}