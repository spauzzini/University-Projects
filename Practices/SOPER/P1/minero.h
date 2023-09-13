/**
 * @file minero.h
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#ifndef _MINERO_H
#define _MINERO_H

/* Número máximo de hilos que se pueden lanzar */
#define MAX_HILOS 10

/* START [_WorkerInfo] */
typedef struct _WorkerInfo WorkerInfo;
/* END [_WorkerInfo] */

/**
 * @brief Proceso minero. Hace fork: el proceso padre busca soluciones y el hijo
 * llama al proceso monitor, que comprueba las soluciones. Ambos procesos se intercambian
 * información mediante una tubería.
 * 
 * @param goal int: objetivo.
 * @param n_rounds int: número de rondas.
 * @param n_threads int: número de hilos.
 * 
*/
int minero(int goal, int n_rounds, int n_threads);

/**
 * @brief Función que ejecuta cada hilo para buscar la solución. El rango de búsqueda
 * varía en función del id del hilo. Llama a pow hash y si se encuentra la solución, pone a 1
 * la variable global solfound y guarda en newtarget la solución.
 * 
 * @param args void*
*/
void* work(void* args);

#endif /* MINERO H */