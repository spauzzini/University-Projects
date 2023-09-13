/**
 *
 * Descripcion: Implementation of sorting functions
 *
 * Fichero: sorting.c
 * Autor: Carlos Aguirre
 * Version: 1.0
 * Fecha: 16-09-2019
 *
 */


#include "sorting.h"

/***************************************************/
/* Function: InsertSort    Date: 14/10/2020        */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/*Function that sorts an array of elements  with   */
/* the insertion method                            */
/*                                                 */
/* Input:                                          */
/* int *table: the table to be sorted              */
/* int ip: index of the first element in the table */
/* int iu: index of the last element in the table  */
/*                                                 */
/* Output:                                         */
/* cont (number of times the OB has been executed) */
/* in case of success                              */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/

int InsertSort(int* table, int ip, int iu)
{
  int i=0, j=0, cont=0;
  int A=0;

  if(!table || ip<0 || iu<0) return ERR;

  for (i=(ip+1); i<=iu; i++){
    A = table[i];
    j = (i-1);
    while((j >= ip) && (table[j] > A)){
      table[j+1] = table[j];
      cont++;
      j--;
    }
    table[j+1] = A;
  }
  return cont;
}

/***************************************************/
/* Function: InsertSort    Date: 14/10/2020        */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/*Function that sorts an array of elements  with   */
/* the insertion method int the inverse order (from*/
/* bigger to smaller)                              */
/*                                                 */
/* Input:                                          */
/* int *table: the table to be sorted              */
/* int ip: index of the first element in the table */
/* int iu: index of the last element in the table  */
/*                                                 */
/* Output:                                         */
/* cont (number of times the OB has been executed) */
/* in case of success                              */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/

int InsertSortInv(int* table, int ip, int iu)
{
  int i=0, j=0, cont=0;
  int A=0;

  if(!table || ip<0 || iu<0) return ERR;

 for (i=(ip+1); i<=iu; i++){
    A = table[i];
    j = (i-1);
    while((j >= ip) && (table[j] < A)){
      table[j+1] = table[j];
      cont++;
      j--;
    }
    table[j+1] = A;
  }
  return cont;
}


