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
static GADeleteFnc deleteIndividual = 0;
static GACloneFnc clone = 0;
static GAFitnessFnc fitness = 0;
static GACrossoverFnc crossover = 0;
static GAMutateFnc mutate = 0;
static GASelectionFnc selection = 0;

/*
 * Selects an individual at random, with probability of selection ~ fitness
 * @param: array of individuals
 * @param: array of corresponding fitness values
 * @param: sum of all fitness values
 * @param: number of individual
 * @ret: index of selected individual in the population
*/
int GAselect(Population population, double * fitnessValues, double sumOfFitness, int popSz)
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

/*
 * Get next population from current population
 * @param: array of individuals
 * @param: number of individual in population currently
 * @param: number of individual in the new population (returned array)
 * @param: 0 = delete old population, 1 = keep old population (warning: user must delete it later)
 * @ret: new array of individual (size = 3rd parameter)
*/
Population GAnextGen(Population currentPopulation, int oldPopSz, int newPopSz,
                     short keepOldPopulation)
{
   //allocate memory for next generation
   Population nextPopulation = malloc( newPopSz * sizeof(void*) );
   if (nextPopulation == NULL) 
   {
      return (0);
   }
   int i,k;
   //make array of fitness values
   double * fitnessArray = malloc ( oldPopSz * sizeof(double) );
   double totalFitness = 0;
   int best = 0;  //save best's index

   for (i = 0; i < oldPopSz; ++i)
   {
      fitnessArray[i] = fitness(currentPopulation[i]);
      if (fitnessArray[i] < 0) fitnessArray[i] = 0;   //negative fitness not allowed

      totalFitness += fitnessArray[i];
      if (fitnessArray[i] > fitnessArray[best]) 
         best = i;
   }

   //keep the best
   nextPopulation[0] = clone(currentPopulation[best]);

   //select the fit individuals
   void * x1 = NULL, * x2 = NULL;
   for (i = 1; i < newPopSz; ++i)
   {
      k = selection(currentPopulation,fitnessArray,totalFitness,oldPopSz);

      x1 = currentPopulation[k];
      if (crossover != NULL) 
      {
         double temp = fitnessArray[k];
         fitnessArray[k] = 0;   //this is to prevent self-self crossover
         int k2 = selection(currentPopulation,fitnessArray,totalFitness,oldPopSz);
         fitnessArray[k] = temp;
         x2 = currentPopulation[k2];
         x1 = crossover(x1,x2);
      }
      else
      {
         x1 = clone(x1);
      }

      if (mutate != NULL) 
      {
         x1 = mutate(x1);
      }
      nextPopulation[i] = x1; //add to the new population
   }
   /*free the memory from the old population*/
   if (keepOldPopulation == 0)
   {
 
     for (i = 0; i < oldPopSz; ++i)
        if (currentPopulation[i] != NULL)
           deleteIndividual(currentPopulation[i]);
     free(currentPopulation);
   }
   free(fitnessArray);
   return (nextPopulation);
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
void GAinit(GADeleteFnc deleteIndividualPtr, GACloneFnc clonePtr,GAFitnessFnc fitnessPtr, GACrossoverFnc crossoverPtr, GAMutateFnc mutatePtr, GASelectionFnc selectionPtr)
{
	deleteIndividual = deleteIndividualPtr;
	clone = clonePtr;
	fitness = fitnessPtr;
	crossover = crossoverPtr;
	mutate = mutatePtr;
	if (selectionPtr == 0)
		selection = &(GAselect);
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
Population GArun(Population initialPopulation, int initPopSz, int popSz, int numGenerations,
                 GACallbackFnc callback)
{
   FILE * errfile = freopen("GArun_errors.log", "w", stderr);
   
   /*function pointers*/
   if (!deleteIndividual || !clone || !fitness || (!crossover && !mutate) || !selection) return 0;

   initMTrand(); /*initialize seeds for MT random number generator*/
   int i = 0, stop = 0;
   Population population = initialPopulation;

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
    int less(double x, double y) {
        return (x > y);
    }

    // exchange a[i] and a[j]
    void exch(Population population, double* a, int i, int j) {
        double swap = a[i];
        a[i] = a[j];
        a[j] = swap;

        void * temp = population[i];
        population[i] = population[j];
        population[j] = temp;
    }

    // partition a[left] to a[right], assumes left < right
    int partition(Population population, double* a, int left, int right) {
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
    void quicksort(Population population, double* a, int left, int right) {
        if (right <= left) return;
        int i = partition(population, a, left, right);
        quicksort(population, a, left, i-1);
        quicksort(population, a, i+1, right);
    }

    //quicksort
    void GAsort(Population population, GAFitnessFnc fitness, int populationSz) 
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

