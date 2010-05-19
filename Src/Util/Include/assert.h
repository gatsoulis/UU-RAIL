#ifndef ASSERT_H_
#define ASSERT_H_

#include <stdio.h>

#ifndef NDEBUG
# define ASSERT( isOK, message ) \
 if ( !(isOK) ) { \
  (void)printf("ERROR!! Assert ‘%s’ failed on line %d " \
   "in file ‘%s’\n%s\n", \
      #isOK, __LINE__, __FILE__, #message); \
  }
#else
# define ASSERT( unused, message ) do {} while ( false )
#endif

#endif /*ASSERT_H_*/
