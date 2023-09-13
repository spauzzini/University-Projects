/**
 * @file monitor.h
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#ifndef _MONITOR_H
#define _MONITOR_H

/**
 * @brief Proceso monitor. Comprueba si la solución del minero es correcta o no.
 * 
 * @param mineroSol int: solucion dada por el minero.
 * @param target int: objetivo.
 * 
 * @return 0 y 1 dependiendo de si la solución es correcta o no.
*/
int monitor(int mineroSol, int target);

#endif /* MONITOR H */