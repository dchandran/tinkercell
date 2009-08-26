/****************************************************************************
**
** Copyright (C) 2008 Deepak Chandran
** see ga.h
**
****************************************************************************/

#include "ga.h"
#include <stdio.h>

/*
* functions for the GA
*/
static GADeleteFnc deleteGAindividual = 0;
static GACloneFnc clone = 0;
static GAFitnessFnc fitness = 0;
static GACrossoverFnc crossover = 0;
static GAMutateFnc mutate = 0;
static GASelectionFnc selection = 0;

/*************************************************
get and set the above function pointers
*************************************************/

void GAsetupNewStruct(GADeleteFnc f, GACloneFnc g)
{
	deleteGAindividual = f;
	clone = g;
}

void GAsetFitnessFunction(GAFitnessFnc f)
{
	fitness = f;
}

GAFitnessFnc GAgetFitnessFunction()
{
	return fitness;
}

void GAsetCrossoverFunction(GACrossoverFnc f)
{
	crossover = f;
}

GACrossoverFnc GAgetCrossoverFunction()
{
	return crossover;
}

void GAsetMutationFunction(GAMutateFnc f)
{
	mutate = f;
}

GAMutateFnc GAgetMutationFunction()
{
	return mutate;
}

void GAsetSelectionFunction(GASelectionFnc f)
{
	selection = f;
}

GASelectionFnc GAgetSelectionFunction()
{
	return selection;
}

/*************************************************/

/*! \brief Selects an individual at random, with probability of selection ~ fitness
* \param array of individuals
* \param array of corresponding fitness values
* \param sum of all fitness values
* \param number of individual
* \ingroup ga
*/
int GArouletteWheelSelection(GApopulation population, double * fitnessValues, double sumOfFitness, int popSz)
{
	int i;
	double randNum = mtrand() * sumOfFitness, 
		total = 0;
	for (i=0; i < popSz-1; ++i)
		if (total < randNum && randNum < (total+fitnessValues[i]))
			return (i);
		else
			total += fitnessValues[i];
	return (i);
}
/*! \brief Selects the best of two random individuals
* \param array of individuals
* \param array of corresponding fitness values
* \param sum of all fitness values
* \param number of individual
* \ingroup ga
*/
int GAtournamentSelection(GApopulation population, double * fitnessValues, double sumOfFitness, int popSz)
{
	int i = (int)(mtrand() * popSz);
	int j = (int)(mtrand() * popSz);
	int k;

	while (popSz != 1 && i==j)  //prevent tournament with self
		j = (int)(mtrand() * popSz);

	if (fitnessValues[i] > fitnessValues[j])
		k = i;
	else
		k = j;
	//fitnessValues[k] = 0;   //do not pick this individual again?
	return (k);
}
/*! \brief Selects the individual with highest fitness
* \param array of individuals
* \param array of corresponding fitness values
* \param sum of all fitness values
* \param number of individual
* \ingroup ga
*/
int GAeliteSelection(GApopulation population, double * fitnessValues, double sumOfFitness, int popSz)
{
	int i, best = 0;
	for (i=0; i < popSz; ++i)
		if (fitnessValues[i] > fitnessValues[best])
			best = i;
	fitnessValues[best] = 0;
	return (best);
}
/*
* Get next population from current population
* @param: array of individuals
* @param: number of individual in population currently
* @param: number of individual in the new population (returned array)
* @param: 0 = delete old population, 1 = keep old population (warning: user must delete it later)
* @ret: new array of individual (size = 3rd parameter)
*/
GApopulation GAnextGen(GApopulation currentGApopulation, int oldPopSz, int newPopSz,
					   short keepOldGApopulation)
{
	int i,k,best,k2;
	void * x1 = NULL, * x2 = NULL;
	double * fitnessArray, totalFitness;
	//allocate memory for next generation
	GApopulation nextGApopulation = malloc( newPopSz * sizeof(void*) );
	if (nextGApopulation == NULL) 
	{
		return (0);
	}

	//make array of fitness values
	fitnessArray = malloc ( oldPopSz * sizeof(double) );
	totalFitness = 0;
	best = 0;  //save best's index

	for (i = 0; i < oldPopSz; ++i)
	{
		fitnessArray[i] = fitness(currentGApopulation[i]);
		if (fitnessArray[i] < 0) fitnessArray[i] = 0;   //negative fitness not allowed

		totalFitness += fitnessArray[i];
		if (fitnessArray[i] > fitnessArray[best]) 
			best = i;
	}

	//keep the best
	nextGApopulation[0] = clone(currentGApopulation[best]);

	//select the fit individuals
	x1 = NULL;
	x2 = NULL;
	for (i = 1; i < newPopSz; ++i)
	{
		k = selection(currentGApopulation,fitnessArray,totalFitness,oldPopSz);

		x1 = currentGApopulation[k];
		if (crossover != NULL) 
		{
			double temp = fitnessArray[k];
			fitnessArray[k] = 0;   //this is to prevent self-self crossover
			k2 = selection(currentGApopulation,fitnessArray,totalFitness,oldPopSz);
			fitnessArray[k] = temp;
			x2 = currentGApopulation[k2];
			x1 = crossover(x1,x2);

			if (x1 == currentGApopulation[k] || x1 == currentGApopulation[k2])
			{
				x1 = clone(x1); //cannot allow the same x1
			}
		}
		else
		{
			x1 = clone(x1);
		}

		if (mutate != NULL) 
		{
			x1 = mutate(x1);
		}

		nextGApopulation[i] = x1; //add to the new population
	}
	/*free the memory from the old population*/
	if (keepOldGApopulation == 0)
	{
		for (i = 0; i < oldPopSz; ++i)
			if (currentGApopulation[i] != NULL)
				deleteGAindividual(currentGApopulation[i]);
		free(currentGApopulation);
	}
	free(fitnessArray);
	return (nextGApopulation);
}

/*
* Initialize the GA. This function MUST be called before GArun
* @param: cloning function (cannot be 0)
* @param: deletion function (cannot be 0)
* @param: fitness function pointer (cannot be 0)
* @param: crossover function pointer (can be 0, but not recommended)
* @param: mutation function pointer (can bt 0, but not recommended)
* @param: selection function pointer (can be 0)
* @ret: final array of individuals (sorted by fitness)
*/
void GAinit(GADeleteFnc deleteGAindividualPtr, GACloneFnc clonePtr,GAFitnessFnc fitnessPtr, GACrossoverFnc crossoverPtr, GAMutateFnc mutatePtr, GASelectionFnc selectionPtr)
{
	deleteGAindividual = deleteGAindividualPtr;
	clone = clonePtr;
	fitness = fitnessPtr;
	crossover = crossoverPtr;
	mutate = mutatePtr;
	if (selectionPtr == 0)
		selection = &(GArouletteWheelSelection);
	else
		selection = selectionPtr;
}

/*
* The main GA loop
* @param: array of individuals
* @param: number of individual initially
* @param: number of individual in successive populations
* @param: total number of generations
* @param: callback function pointer
* @ret: final array of individuals (sorted)
*/
GApopulation GArun(GApopulation initialGApopulation, int initPopSz, int popSz, int numGenerations,
				   GACallbackFnc callback)
{
	int i = 0, stop = 0;
	GApopulation population = initialGApopulation;

	FILE * errfile = freopen("GArun_errors.log", "w", stderr);

	/*function pointers*/
	if (!deleteGAindividual || !clone || !fitness || (!crossover && !mutate) || !selection) return 0;

	initMTrand(); /*initialize seeds for MT random number generator*/

	while (stop == 0) //keep going until max iterations or until callback function signals a stop
	{ 
		if (i == 0)  //initial population
			population = GAnextGen(population, initPopSz, popSz, 0);
		else        //successive populations
			population = GAnextGen(population, popSz, popSz, 0);

		if (callback != NULL)
			stop = callback(i,population,popSz);   //callback function can stop the GA

		++i;
		if (i >= numGenerations) stop = 1;  //max number of iterations
	}
	GAsort(population,fitness,popSz);  //sort by fitness (Quicksort)

	fclose(errfile);
	return (population);
}

/***********************************************************************
*  Quicksort code from Sedgewick 7.1, 7.2.
***********************************************************************/

// is x < y ?
static int less(double x, double y) {
	return (x > y);
}

// exchange a[i] and a[j]
static void exch(GApopulation population, double* a, int i, int j) 
{
	void * temp;
	double swap;
	
	swap = a[i];
	a[i] = a[j];
	a[j] = swap;

	temp = population[i];
	population[i] = population[j];
	population[j] = temp;
}

// partition a[left] to a[right], assumes left < right
static int partition(GApopulation population, double* a, int left, int right) 
{
	int i = left - 1;
	int j = right;
	while (1) {
		while (less(a[++i], a[right]))      // find item on left to swap
			;                               // a[right] acts as sentinel
		while (less(a[right], a[--j]))      // find item on right to swap
			if (j == left) break;           // don't go out-of-bounds
		if (i >= j) break;                  // check if pointers cross
		exch(population, a, i, j);         // swap two elements into place
	}
	exch(population, a, i, right);                      // swap with partition element
	return i;
}

// quicksort helper a[left] to a[right]
static void quicksort(GApopulation population, double* a, int left, int right) 
{
	int i = partition(population, a, left, right);

	if (right <= left) return;
	quicksort(population, a, left, i-1);
	quicksort(population, a, i+1, right);
}

//quicksort
void GAsort(GApopulation population, GAFitnessFnc fitness, int populationSz) 
{
	double * a = malloc ( populationSz * sizeof(double) );
	int i;
	for (i=0; i < populationSz; ++i)
	{
		a[i] = fitness(population[i]);
	}
	if (a != NULL)
	{
		quicksort(population, a, 0, populationSz - 1);
		free(a);
	}
}

