/**
 * @file pow.c
 * @author SOPER teaching team.
 * @brief Computation of the POW.
 * @version 1.5
 * @date 2023-02-02
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "pow.h"

long int pow_hash(long int x) {
  long int result = (x * BIG_X + BIG_Y) % PRIME;
  return result;
  
}