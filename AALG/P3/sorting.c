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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

/***************************************************/
/* Function: mergesort    Date: 21/10/2020         */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Function that sorts an array of elements  with  */
/* the merge method                                */
/*                                                 */
/* Input:                                          */
/* int *table: the table to be sorted              */
/* int ip: index of the first element in the table */
/* int iu: index of the last element in the table  */
/*                                                 */
/* Output:                                         */
/* OB (number of times the OB has been executed)   */
/* in case of success                              */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/

int mergeSort(int* table, int ip, int iu){
  int m=0;
  int cont=0;
  int OB=0;
  
  if(!table || ip>iu){
    return ERR;
  }

  if(ip == iu){
    return 0;
  }

  if(ip<iu){
    m = ((iu-ip)/2)+ip;
  
  cont = mergeSort(table, ip, m);
    if(cont == ERR){
      return ERR;
    }

  OB = OB+cont;

  cont = mergeSort(table, (m+1), iu);
    if(cont == ERR){
      return ERR;
    }
  OB = OB+cont;
  
  cont = merge(table, ip, iu, m);
    if(cont == -1){
      return ERR;
    }
  OB  = OB+cont;
  }
  return OB;
}

/***************************************************/
/* Function: merg         Date: 21/10/2020         */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Function that switch the two elements of the     */
/* array to order them                             */
/*                                                 */
/* Input:                                          */
/* int *table: the table to be sorted              */
/* int ip: index of the first element in the table */
/* int iu: index of the last element in the table  */
/* int imiddle: index in the middle of the array   */
/*                                                 */
/* Output:                                         */
/* ob (number of times the OB has been executed)   */
/* in case of success                              */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/

int merge(int* table, int ip, int iu, int imiddle) {
  int i=0, j=0, k=0, size1=0, size2=0;
  int *taux1=NULL;
  int *taux2=NULL;
  int ob=0;

  if(!table || ip>iu){
    return ERR;
  }

  size1 = (imiddle - ip + 1);
  size2 = (iu-imiddle);

  taux1 = (int *)malloc(size1 * sizeof(int));
  if(!taux1){
    return ERR;
  } 

  taux2 = (int *)malloc(size2 * sizeof(int));
  if(!taux2){
    free(taux1);
    return ERR;
  }

  for(i=0; i<size1; i++){
    taux1[i] = table[ip+i];
  }

  for(j=0; j<size2; j++){
    taux2[j] = table[imiddle + 1 + j];
  }

  i = 0;
  j = 0;
  k = ip;
  
  while (i < size1 && j < size2){
    if (taux1[i] <= taux2[j]){
      table[k] = taux1[i];
      i++;
      ob++;
    }
    else{
      table[k] = taux2[j];
      j++;
      ob++;
    }
    k++;
  }

  while (i < size1){
    table[k] = taux1[i];
    i++;
    k++;
  }

  while (j < size2){
    table[k] = taux2[j];
    j++;
    k++;
  }

  free(taux1);
  free(taux2);
  return ob;
}

/***************************************************/
/* Function: quicksort          Date: 28/10/2020   */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Function that sorts an array of elements  with  */
/* the quick method                                */
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

int quicksort(int* table, int ip, int iu){
  int cont=0;
  int pivot=0;
  int x=0;

  if (!table || ip>iu){
    return ERR;
  }

  if(ip==iu){
    return OK;
  }

  x = split(table, ip, iu, &pivot);
  if(x==ERR){
    return ERR;
  }
  cont+=x;
  

  if(ip<pivot-1){
    x = quicksort(table, ip, (pivot-1));
    if(x == ERR){
      return ERR;
    }
    cont+=x;
  }

  if(pivot+1<iu){
    x = quicksort(table, (pivot+1), iu);
    if(x == ERR){
      return ERR;
    }
    cont+=x;
  }
  
  return cont;
}

/***************************************************/
/* Function: split              Date: 28/10/2020   */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Function that sorts the array by switching      */
/* elements and compare them with the pivot        */
/*                                                 */
/* Input:                                          */
/* int *table: the table to be sorted              */
/* int ip: index of the first element in the table */
/* int iu: index of the last element in the table  */
/* int *pos: pointer to save the pivot's position  */
/*                                                 */
/* Output:                                         */
/* cont (number of times the OB has been executed) */
/* in case of success                              */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/

int split(int* table, int ip, int iu, int *pos){
  int i=0, cont=0, x=0, k=0;
  
  if(!table || ip>iu || !pos){
    return ERR;
  }

  x = median(table, ip, iu, pos);
  if(x == ERR){
    return ERR;
  }
  k = table[*pos];
  x = intSwap(&table[ip], &table[*pos]);
  if(x == ERR){
    return ERR;
  }
  *pos=ip;
  for(i=ip+1; i<=iu; i++){
    if(table[i]<k){
       (*pos)++;
      x = intSwap(&table[i], &table[*pos]);
      if(x == ERR){
        return ERR;
      }
    }
    cont++;
  }

  x = intSwap(&table[ip], &table[*pos]);
  if(x == ERR){
    return ERR;
  }
  return cont;
} 


/***************************************************/
/* Function: median          Date: 28/10/2020      */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Function to get the pivot. In our case the one  */
/* in the first position                           */
/*                                                 */
/* Input:                                          */
/* int *table: the table to be sorted              */
/* int ip: index of the first element in the table */
/* int iu: index of the last element in the table  */
/* int *pos: pointer to save the pivot's position  */
/*                                                 */
/* Output:                                         */
/* OK in case of success                           */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/

int median(int *table, int ip, int iu, int *pos){
  
  if(!table || ip>iu){
    return ERR;
  }
  *pos=ip;
  /*Because we are using the first element as pivot*/
  return OK;
}


/***************************************************/
/* Function: intSwap          Date: 28/10/2020     */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Function to swap two elements                   */
/*                                                 */
/* Input:                                          */
/* int *x: first element to be swap                */
/* int *x: second element to be swap               */
/*                                                 */
/* Output:                                         */
/* OK in case of success                           */
/* ERR en case of error                            */
/*                                                 */
/***************************************************/

int intSwap (int *x, int *y){
    int temp = 0;

    if(!x || !y){
      return ERR;
    }
    temp = *x;
    *x = *y;
    *y = temp;

    return OK;
}

/***************************************************/
/* Function: quicksort_ntr      Date: 04/11/2020   */
/* Authors: Lucía Gil and Paula Beltrán            */
/*                                                 */
/* Function that sorts an array of elements  with  */
/* the quick method but without tail recursion     */
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

int quicksort_ntr(int* table, int ip, int iu){
  
  int cont=0;
  int pivot=0;
  int x=0;

  START:

  if (!table || ip>iu){
    return ERR;
  }

  if(ip==iu){
    return OK;
  }


  x = split(table, ip, iu, &pivot);
  if(cont==ERR){
    return ERR;
  }
  cont+=x;

  if(ip<pivot-1){
    x = quicksort_ntr(table, ip, (pivot-1));
    if(x == ERR){
      return ERR;
    }
    cont+=x;
  }

  if(pivot+1<iu){
    ip = pivot+1;
    iu=iu;
    goto START;
  }
  return cont;
}