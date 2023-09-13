/**
 * @file monitor.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#ifndef _MONITOR_H
#define _MONITOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>

#include "miner.h"
#include "pow.h"

#define SHM_NAME "/shm_monicomp" //nombre del seg memoria que utilizan Comprobador y Monitor
#define MEM_BLOCKSIZE 6

/* START [_CircularBuffer] */
typedef struct {
    Block bloques[6];
    int idx_last;
    /* Semáforos para Zona Crítica */
    sem_t semMutexMemoria;
    sem_t semFillMemoria;
    sem_t semEmptyMemoria;
} CircularBuffer;
/* END [_CircularBuffer] */

int comprobador (int lag, CircularBuffer *shm_struct_comp, int fd_shm);
int monitor (int lag, CircularBuffer * shm_struct_mon);

#endif /* MONITOR H */