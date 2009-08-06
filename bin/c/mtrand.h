#ifndef MT_RAND_NUM_GEN
#define MT_RAND_NUM_GEN

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */


/* The array for the state vector */
static unsigned long long mt[NN]; 
/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1; 

/* initializes mt[NN] with a seed */
void init_genrand64(unsigned long long seed);
/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
void init_by_array64(unsigned long long init_key[], unsigned long long key_length);

/* generates a random number on [0, 2^64-1]-interval */
unsigned long long genrand64_int64(void);

/* generates a random number on [0, 2^63-1]-interval */
long long genrand64_int63(void);

/* generates a random number on [0,1]-real-interval */
double genrand64_real1(void);

/* generates a random number on [0,1)-real-interval */
double genrand64_real2(void);

/* generates a random number on (0,1)-real-interval */
double genrand64_real3(void);

void initMTrand(void);

double mtrand(void);

#endif
