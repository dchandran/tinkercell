/********************************************************************************************************

Copyright (C) 2008 Deepak Chandran
contact: dchandran1@gmail.com

	This library provides the basic functions for running a Genetic Algorithm (GA), but
	the user is required to setup the fitness function and related functions.
	
	The user MUST define:
		1) a struct that represents an "individual"
		2) a function that returns the fitness of an individual
		3) a mutation function that randomly alters an individual
		4) a function to free an individual
		5) a function to clone an individual
	
	The following function definition is optional but highly recommended:
		1) a crossover function to make a new individual from two individuals
	
	The following functions definitions are entirely optional:
		1) A function that selects individuals using fitness as probabilities (library provides one by default)
		2) A callback function can be used to examine or terminate the GA at any iteration
		
	The main functions are: GAinit and GArun
	
*********************************************************************************************************/
#ifndef GA_MAIN_LOOP
#define GA_MAIN_LOOP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mtrand.h"

/*! \brief Population of individuals -- an individual is represented by a user struct (void*) */
typedef void ** Population;

/***********************************************************************
    The following functions MUST be defined somewhere
	use initGA to initialize the GA with the functions
***********************************************************************/
/*! \brief
 * Free an individual from memory
 * \param a single individual
 * \ingroup ga
*/
typedef void (*GADeleteFnc)(void *);
/*! \brief
 * Make a copy of an individual and return the memory pointer
 * \param target individual
 * \ingroup ga
*/
typedef void* (*GACloneFnc)(void *);
/*! \brief
 * Compute fitness of an individual. Fitness must be positive if default selection function is used
 * \param target individual
 * \return fitness (double) of the individual (MUST BE POSITIVE if default selection function is used)
 * \ingroup ga
*/
typedef double(*GAFitnessFnc)(void *);

/**********************************************************************************************
    At least one of the following two functions SHOULD (not must) be defined, 
	otherwise the GA will run, but nothing will evolve
***********************************************************************************************/
/*! \brief
 * combine two individuals to generate a new individual. The function must not delete the parent individuals.
 * \param parent individual 1
 * \param parent individual 2
 * \return pointer to an individual (can be the same as one of the parents)
 * \ingroup ga
*/
typedef void* (*GACrossoverFnc)(void *, void *);
/*! \brief
 * Change an individual randomly. If a new individual is created, then this function must delete (free) the old one.
 * \param parent individual
 * \return pointer to an individual (can be the same as the parent)
 * \ingroup ga
*/
typedef void* (*GAMutateFnc)(void *);

/*********************************************************************************************
  The following two functions are entirely optional. 
  The GA library provides default Selection Function, which can be overwritten
  They may or may not affect the GA performance 
*********************************************************************************************/
/*! \brief
 * Selection function. If null, then a default selection function is provided that linearly converts fitness values to probabilities
 * \param Population of individuals
 * \param array of fitness values for the individuals
 * \param total fitness (sum of all fitness values)
 * \param number of individuals in the population
 * \return index (in population vector) of the individual to select
 * \ingroup ga
*/
typedef int(*GASelectionFnc)(Population , double * , double , int );
/*! \brief
 * Callback function. If not null, then this function is called during each iteration of the GA. 
 * This function can be used to terminate the GA at any step
 * \param iteration
 * \param Population of individuals
 * \param number of individuals in the population
 * \return 0 = continue GA, 1 = stop GA. This can be used to stop the GA before it reaches max iterations
 * \ingroup ga
*/
typedef int(*GACallbackFnc)(int iter,Population,int popSz);

/************************************************************************************************************
  The central functions defined in this genetic algorithm library
****************************************************************************************************************/

/*! \brief Initialize the GA. This function MUST be called before GArun
 * \param cloning function (cannot be 0)
 * \param deletion function (cannot be 0)
 * \param fitness function pointer (cannot be 0)
 * \param crossover function pointer (can be 0, but not recommended)
 * \param mutation function pointer (can bt 0, but not recommended)
 * \param selection function pointer (can be 0)
 * \return final array of individuals (sorted by fitness)
 * \ingroup ga
*/
void GAinit(GADeleteFnc, GACloneFnc ,GAFitnessFnc, GACrossoverFnc, GAMutateFnc, GASelectionFnc);

/*! \brief The main GA loop. Must call GAinit before calling GArun. Uses GAnextGen to make new generation of individuals.
 * \param initial population (array of individuals)
 * \param number of individuals in the initial population
 * \param number of individuals to be kept in the successive populations (will affect speed of GA)
 * \param total number of generations
 * \param callback function pointer (can be 0)
 * \return final array of individuals (sorted by fitness)
 * \ingroup ga
*/
Population GArun(Population,int sz0,int sz1,int maxIter, GACallbackFnc);

/*! \brief Selects an individual at random, with probability of selection ~ fitness
 * \param array of individuals
 * \param array of corresponding fitness values
 * \param sum of all fitness values
 * \param number of individual
 * \ingroup ga
*/
int GAselect(Population , double * , double , int );
/*! \brief Generates the next population from current population
 * \param array of individuals
 * \param number of individual in population currently
 * \param number of individual in the new population (returned array)
 * \param 0 = delete old population, 1 = keep old population (warning: user must delete it later)
 * \return new array of individual (size = 3rd parameter)
 * \ingroup ga
*/
Population GAnextGen(Population,int,int,short);

/*! \brief sort (Quicksort) a population by its fitness (used at the end of GArun)
 * \param population to sort
 * \param fitness function
 * \param size of population
 * \return void
 * \ingroup ga
*/
void GAsort(Population population, GAFitnessFnc, int);

#endif

