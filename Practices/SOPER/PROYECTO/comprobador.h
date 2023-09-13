/**
 * @file comprobador.h
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#ifndef _COMPROBADOR_H
#define _COMPROBADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <semaphore.h>

#include "minero.h"
#include "pow.h"

#define SHM_NAME_CM "/shm_commin"

/* Estructura Memoria Compartida Comprobador-Monitor */
typedef struct { 
    Block bloques[5];
    int idx_last;
    /*Semforos*/
    sem_t semMutex;
    sem_t semFill;
    sem_t semEmpty;
} ComminMem;

#endif /* _COMPROBADOR_H */ 