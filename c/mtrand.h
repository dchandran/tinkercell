#include <stdio.h>
#include <time.h>

#ifndef MT_RAND_NUM_GEN
#define MT_RAND_NUM_GEN

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

/*! \brief Initialize the random number generator based on time*/
void initMTrand(void);

/*! \brief Initialize the random number generator based on time and get the seeds
 *  \return the seed based on time*/
unsigned long long * getMTseeds(void);

/*! \brief set the starting seed for the random number generator.
 * \param the input array MUST be of length 4*/
void setMTseeds(unsigned long long *);

/*! \brief indicates whether mtrand has been initialized yet*/
int MTrandHasBeenInitialized();

double mtrand(void);

#endif
