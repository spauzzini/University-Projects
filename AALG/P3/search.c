/**
 *
 * Description: Implementation of functions for search
 *
 * File: search.c
 * Author: Carlos Aguirre and Javier Sanz-Cruzado
 * Version: 1.0
 * Date: 14-11-2016
 *
 */

#include "search.h"
#include "sorting.h"

#include <stdlib.h>
#include <math.h>

/*Declaration of some private functions*/
/*int intSwap (int *x, int *y);*/

/**
 *  Key generation functions
 *
 *  Description: Receives the number of keys to generate in the n_keys
 *               parameter. The generated keys go from 1 to max. The
 * 				 keys are returned in the keys parameter which must be 
 *				 allocated externally to the function.
 */
  
/**
 *  Function: uniform_key_generator
 *               This function generates all keys from 1 to max in a sequential
 *               manner. If n_keys == max, each key will just be generated once.
 */
void uniform_key_generator(int *keys, int n_keys, int max)
{
  int i;

  for(i = 0; i < n_keys; i++) keys[i] = 1 + (i % max);

  return;
}

/**
 *  Function: potential_key_generator
 *               This function generates keys following an approximately
 *               potential distribution. The smaller values are much more 
 *               likely than the bigger ones. Value 1 has a 50%
 *               probability, value 2 a 17%, value 3 the 9%, etc.
 */
void potential_key_generator(int *keys, int n_keys, int max)
{
  int i;

  for(i = 0; i < n_keys; i++) 
  {
    keys[i] = .5+max/(1 + max*((double)rand()/(RAND_MAX)));
  }

  return;
}

PDICT init_dictionary (int size, char order)
{
  PDICT dic=NULL;

  if(size<0 || order<0 || order>1 ){
    return NULL;
  }

  dic = (PDICT)malloc(sizeof(DICT));
  if (!dic) {
    return NULL;
  }

  dic->size = size;
  dic->order = order;
  dic->n_data = 0;

  (dic->table) = (int *)malloc(size * sizeof(int));
  if (!(dic->table)){
    free(dic);
    return NULL;
  }

  return dic;
}

void free_dictionary(PDICT pdict)
{
	if(!pdict){
    return;
  }

  free(pdict->table);
  free(pdict);

  return;
}

int insert_dictionary(PDICT pdict, int key)
{
  int j=0;
	if(!pdict || key<0 || pdict->size == pdict->n_data){
    return EXIT_FAILURE;
  }

  if(pdict->order == NOT_SORTED){
    pdict->table[(pdict->n_data)]=key;
    (pdict->n_data)++;
  }
  else{
    pdict->table[(pdict->n_data)]=key;
    (pdict->n_data)++;
    j=pdict->n_data-1;

    while(j>=0 && pdict->table[j]>key){
      pdict->table[j+1] = pdict->table[j];
      /*pdict->table[j]=key;*/
      j--;    
    }
    pdict->table[j]=key;
  }
  return EXIT_SUCCESS;
}

int massive_insertion_dictionary (PDICT pdict,int *keys, int n_keys)
{
  int i=0;
	if(!pdict || !keys || (pdict->n_data+n_keys)> pdict->size){
    return EXIT_FAILURE;
  }
  for(i=0; i<n_keys; i++){
    if((insert_dictionary(pdict, keys[i]))==EXIT_FAILURE){
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

int search_dictionary(PDICT pdict, int key, int *ppos, pfunc_search method)
{
  int ret=0;

	if(!pdict || key<0 || !ppos || !method){
    return EXIT_FAILURE;
  }

  ret = method(pdict->table, 0, pdict->n_data-1, key, ppos);
  if(ret == EXIT_FAILURE){
    return EXIT_FAILURE;
  }

  return ret;
}


/* Search functions of the Dictionary ADT */
int bin_search(int *table,int F,int L,int key, int *ppos)
{
	int start=F, end=L, medium=0, n=0, ob=0, res=0;

  if(!table || F<0 || L<0 || key<0 || !ppos){
    return EXIT_FAILURE;
  }

  if(start == end){
    return ob;
  }

 
  medium = (start+end)/2;
  ob ++;
  n = table[medium]-key;
  if(n==0){
    *ppos = medium;
    return ob;
  }

  else if(n<0){
    res = bin_search(table, medium+1, end, key, ppos);
    if(res==ERR){
      return ERR;
    }
    if(res== NOT_FOUND){
      return NOT_FOUND;
    }
    ob += res;
    return ob;
  }
  else{
    res = bin_search(table, start, medium, key, ppos);
    if(res==ERR){
      return ERR;
    }
    if(res== NOT_FOUND){
      return NOT_FOUND;
    }
    ob += res;
    return ob;  
  }
  
  return NOT_FOUND;
}

int lin_search(int *table,int F,int L,int key, int *ppos)
{
  int i=0, ob=0;
  
	if(!table || F<0 || L<0 || key<0 || !ppos){
    return EXIT_FAILURE;
  }
  
  for(i=F; i<=L; i++){
    ob++;
    if(table[i]==key){
      *ppos=i;
      return ob;
    }
  }
  return NOT_FOUND;
}

int lin_auto_search(int *table,int F,int L,int key, int *ppos)
{
	int i=0, ob=0;

  if(!table || F<0 || L<0 || key<0 || !ppos){
    return EXIT_FAILURE;
  }
  for(i=F; i<=L; i++){
    ob++;
    if(table[i]==key){
      if (i>0){
        if((intSwap(&table[i], &table[i-1]))==EXIT_FAILURE){
          return EXIT_FAILURE;
        }
      }
      *ppos=i;
      return ob;
    }
  }
  return NOT_FOUND;
}

/*int intSwap (int *x, int *y){
    int temp = 0;

    if(!x || !y){
      return EXIT_FAILURE;
    }

    temp = *x;
    *x = *y;
    *y = temp;

    return EXIT_SUCCESS;
}*/