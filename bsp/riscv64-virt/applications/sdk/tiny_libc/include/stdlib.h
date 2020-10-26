#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

#define RAND_MAX (INT32_MAX)

void srand(unsigned seed);
int rand();

long int atol ( const char * str );
int atoi(char* pstr);

#endif /* STDLIB_H */
