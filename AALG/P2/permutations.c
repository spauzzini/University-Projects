/**
 *
 * Descripcion: Implementation of function that generate permutations
 *
 * File: permutations.c
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 21-09-2019
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "permutations.h"



/***************************************************/
/* Function: random_num Date:                      */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Rutine that generates a random number           */
/* between two given numbers                       */
/*                                                 */
/* Input:                                          */
/* int inf: lower limit                            */
/* int sup: upper limit                            */
/* Output:                                         */
/* int: random number                              */
/***************************************************/
int random_num(int inf, int sup)
{
  int num;

  if(inf < 0 || sup < 0 || sup < inf ) {
    return ERR;
  }

  num = inf + (rand() % (sup - inf+1));
  
  return num;
}

/***************************************************/
/* Function: generate_perm Date:                   */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Rutine that generates a random permutation      */
/*                                                 */
/* Input:                                          */
/* int n: number of elements in the permutation    */
/* Output:                                         */
/* int *: pointer to integer array                 */
/* that contains the permitation                   */
/* or NULL in case of error                        */
/***************************************************/

int* generate_perm(int N)
{
  int i;
  int rand=0;
  int tmp=0;
  int *perm=NULL;

  perm = (int*)malloc((N) * sizeof(int));
  if(!perm) return NULL;

  for (i=0; i < N; i++){
    perm[i]=i;
  }

  for (i=0; i< N; i++){
    tmp = perm[i];
    rand = random_num(i, (N-1));
    if(rand==ERR){
      free(perm);
      return NULL;
    } 
    perm[i] = perm[rand];
    perm[rand] = tmp;
  }

  return perm;
}

/***************************************************/
/* Function: generate_permutations Date:           */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Function that generates n_perms random          */
/* permutations with N elements                    */
/*                                                 */
/* Input:                                          */
/* int n_perms: Number of permutations             */
/* int N: Number of elements in each permutation   */
/* Output:                                         */
/* int**: Array of pointers to integer that point  */
/* to each of the permutations                     */
/* NULL en case of error                           */
/***************************************************/
int** generate_permutations(int n_perms, int N)
{
  int **perm=NULL;
  int i;

  perm = (int**)malloc((n_perms) * sizeof(int*));
  if(!perm) return NULL;

  for (i=0; i < n_perms; i++){
    perm[i]=generate_perm(N);
    if(perm[i]==NULL){
      for(i=0; i<n_perms; i++){
        free(perm[i]);
      }
      free(perm);
      return NULL;
    }
  }

  return perm;

}
