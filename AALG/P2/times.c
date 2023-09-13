/**
 *
 * Descripcion: Implementation of time measurement functions
 *
 * Fichero: times.c
 * Autor: Carlos Aguirre Maeso
 * Version: 1.0
 * Fecha: 16-09-2019
 *
 */

#include "times.h"
#include "sorting.h"
#include "permutations.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>


/***************************************************/
/* Function: average_sorting_time Date: 17/10/2020 */
/* Autores: Lucía Gil y Paula Beltrán              */
/*                                                 */
/*                                                 */
/* Input:                                          */
/* pfunc_ordena metodo: function that returns the  */
/* number of basic operations                      */
/* int n_perms: number of permutations to generate */
/* int N: size of each permutation                 */                              
/* PTIME_AA ptime: pointer to an structure of type */
/* time that at the end will have the time's       */
/* information                                     */                           
/*                                                 */
/* Output:                                         */
/* OK in case of success                           */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/


short average_sorting_time(pfunc_ordena metodo, 
                              int n_perms,
                              int N, 
                              PTIME_AA ptime)
{
  double t=0;
  int **array_perm = NULL;
  int *array=NULL;
  double av=0;
  int min=0;
  int max=0;
  int i=0;
  clock_t start;
  clock_t end;

  array = (int*)malloc(n_perms * sizeof(int));
  if(!array) return ERR;

  ptime->N = N;
  ptime->n_elems = n_perms;

  array_perm = generate_permutations(n_perms, N);
  if(!array_perm){
    free(array);
    return ERR;
  } 

  for (i=0; i<n_perms; i++){
    start = clock();
    array[i] = metodo(array_perm[i], 0, N-1);
    end = clock();
    t = t + difftime(end, start);
  }

  ptime->time = (t/(double)n_perms);

  min = array[0];
  max = array[0];

  for(i=0; i<n_perms; i++){

    av = av + (double)array[i];

    if(min>array[i]){
      min=array[i];
    }
    if(max<array[i]){
      max=array[i];
    }
  }
  
  ptime->average_ob = (av/((double)n_perms));
  ptime->max_ob = max;
  ptime->min_ob = min;
  
  free(array);

  for (i=0; i<n_perms; i++){
    free(array_perm[i]);
  }

  free(array_perm);

  return OK;

}


/***************************************************/
/* Function: generate_sorting_times Date:          */
/* Autores: Lucía Gil y Paula Beltrán              */
/*                                                 */
/* Input:                                          */
/* pfunc_ordena method: function that returns the  */
/* number of basic operations                      */ 
/* char* file: name of the file to be written      */
/* int num_min: minimum range for the permutations */
/* int num_max: maximum range for the permutations */
/* int incr: size of the increases in the ranges   */
/* int n_perms: number of permutations to generate */
/*                                                 */
/* Output:                                         */
/* OK in case of success                           */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/

short generate_sorting_times(pfunc_ordena method, char* file, 
                                int num_min, int num_max, 
                                int incr, int n_perms)
{

  PTIME_AA ptime2;
  int i=0;
  int j=0;
  int ic=0;

  j=((num_max-num_min)/incr);

  ptime2 = (PTIME_AA)malloc(j*sizeof(TIME_AA));
  if(!ptime2) return ERR;

  for(i=0;i<j;i++){
    ptime2[i].average_ob = -1;
    ptime2[i].N = -1;
    ptime2[i].time = -1;
    ptime2[i].n_elems = -1;
    ptime2[i].min_ob = -1;
    ptime2[i].max_ob = -1;
  }

  ic=num_min;

  for(i=0; i<j; i++){

    if((average_sorting_time(method, n_perms, ic, &(ptime2[i])))==ERR){
      free(ptime2);
      return ERR;
    }
    ic = ic+incr;

  }

  if((save_time_table(file, ptime2, j))==ERR){
    free(ptime2);
    return ERR;
  }


  free(ptime2);
  return OK;

}
/***************************************************/
/* Function: save_time_table Date: 17/10/2020      */
/* Autores: Lucía Gil y Paula Beltrán              */
/*                                                 */
/*Function that generates a table in a file with   */
/* all the information of the times                */
/*                                                 */
/* Input:                                          */
/* char *file: name of the file to be written      */
/* PTIME_AA ptime: array of structure with the     */
/* information to be written in the file           */
/*                                                 */
/* Output:                                         */
/* OK in case of success                           */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/
short save_time_table(char* file, PTIME_AA ptime, int n_times)
{
  int i;
  FILE *arch=NULL;

  arch = fopen (file, "w");
  if(!arch) return ERR;

  fprintf(arch, "|       Size N      |Average clock time time|Average average_ob|Maximum max_ob|Minimum min_ob|\n");
  
  for (i=0; i<n_times; i++){
    fprintf(arch, "|         %d          |        %.3f        |        %.3f        |        %d        |        %d        |\n", ptime[i].N, ptime[i].time, ptime[i].average_ob, ptime[i].max_ob, ptime[i].min_ob);
  }

  fclose(arch);
  return OK;
}


