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
#include <time.h>
#include <stdio.h>
#include <stdlib.h>



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

/***************************************************/
/* Function: logtwo          Date: 15/11/2020      */
/* Autores: Lucía Gil y Paula Beltrán              */
/*                                                 */
/*Function that transforms a log in base 10 to a   */
/* log in base 2                                   */
/*                                                 */
/* Input:                                          */
/* int n: the number from we want the logarithm    */
/*                                                 */
/* Output:                                         */
/* res in case of success: result of the operation */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/
/*double logtwo(int n){
  double res=0;

  if(n<0){
    return ERR;
  }

  res=(log10(n))/(log10(2));
  return res;
}*/

/***************************************************/
/* Function: generate_file   Date: 15/11/2020      */
/* Autores: Lucía Gil y Paula Beltrán              */
/*                                                 */
/*Function that generates a table in a file with   */
/* all the information of the result of the        */
/* operations to store the average of BO           */
/*                                                 */
/* Input:                                          */
/* char *file: name of the file to be written      */
/*                                                 */
/* Output:                                         */
/* OK in case of success                           */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/
/*int generate_file(char* file){
  int i=0, n=0, ic=1000;
  double valor=0;
  FILE *arch=NULL;

  arch = fopen (file, "w");
  if(!arch) return ERR;

  n=(10000-1000)/100;

  for(i=0; i<n; i++){
    
    valor = ic * (logtwo(ic));
    fprintf(arch, "%.3f %d\n", valor, ic);
    ic=ic+100;
  }

  fclose(arch);
  return OK;
}*/

short average_search_time(pfunc_search method, pfunc_key_generator generator,int order, int N, int n_times, PTIME_AA ptime){
  PDICT dic=NULL;
  int *array_perm=NULL;
  long *array=NULL;
  int *table=NULL;
  long av=0;
  int ppos=0;
  long min=0;
  long max=0;
  int i=0;
  int t=0;
  clock_t start;
  clock_t end;
  
  if(!method || !generator || order<0 || order >1 || N<0 || n_times<0 || !ptime){
    return EXIT_FAILURE;
  }

  dic = init_dictionary (N, order);
  if(!dic){
    return EXIT_FAILURE;
  }

  ptime->N = N;
  ptime->n_elems = n_times*N;

  array_perm = generate_perm(N);
  if(array_perm==NULL){
    free_dictionary(dic);
    free(array_perm);
    return EXIT_FAILURE;
  }

  if((massive_insertion_dictionary(dic, array_perm, N))==EXIT_FAILURE){
    free_dictionary(dic);
    free(array_perm);
    return EXIT_FAILURE;
  }

  table = (int*)malloc((N*n_times) * sizeof(int));
  if(!table){
    free_dictionary(dic);
    free(array_perm);
    return EXIT_FAILURE;
  }

  array = (long*)malloc((N*n_times) * sizeof(long));
  if(!array){
    free_dictionary(dic);
    free(array_perm);
    free(table);
    return EXIT_FAILURE;
  }

  generator(table, (n_times*N), N); 

  for (i=0; i<(N*n_times); i++){
    start = clock();
    array[i] = method(dic->table, 0, N-1, table[i], &ppos);
    end = clock();
    t = t + difftime(end, start);
  }

  ptime->time = (t/(double)(N*n_times));

  min = array[0];
  max = array[0];

  for(i=0; i<(N*n_times); i++){

    av = av + (double)array[i];

    if(min>array[i]){
      min=array[i];
    }
    if(max<array[i]){
      max=array[i];
    }
  }
  
  ptime->average_ob = (av/((double)(N*n_times)));
  ptime->max_ob = max;
  ptime->min_ob = min;
  
  free(array);
  free_dictionary(dic);
  free(array_perm);

  return OK;
}

short generate_search_times(pfunc_search method, pfunc_key_generator generator, int order, char* file, int num_min, int num_max, int incr, int n_times){
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

    if((average_search_time(method, generator, order, ic, n_times, &(ptime2[i])))==ERR){
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
