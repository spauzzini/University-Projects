/**
 * @file minero.h
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#ifndef _MINERO_H
#define _MINERO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <mqueue.h>

#include "pow.h"

#define MAX_MINEROS 100
#define MAX_HILOS 10
#define MAX_CHAR 15
#define SHM_NAME "/shm_system" 

#define MQ_NAME "/mq_queue_minercomp"
#define MQ_MAXMSG 7
#define MQ_FLAGS 0
#define MQ_CURMSG 0

extern struct mq_attr attr;
extern sem_t *semActivo;

/* Estructura del Minero */
typedef struct {
    pid_t pid;
    int monedas;
} MinerWallet;

/* Bloque para desafíos resueltos */
typedef struct {
    int id;
    pid_t winner;
    int target;
    int solution;
    int totalVotes;
    int posVotes;
    int correct; // flag validated/rejected
    MinerWallet carteras[MAX_MINEROS];
} Block;

/* Estructura Memoria Compartida Sistema */
typedef struct { 
    pid_t pids[MAX_MINEROS];
    int currentSysMiners;
    int playingMiners;
    int votos[MAX_MINEROS];   
    int monedas[MAX_MINEROS]; 
    Block lastBlock;
    Block currentBlock;
    /*Semaforos*/
    sem_t semMutex;
    sem_t semFill;
    sem_t semEmpty;
} SysMemory;

/* Estructura del Hilo */
typedef struct {
    int totalThreads; // Numero total de hilos.
    int threadNum; // ID del hilo.
    int target; // Objetivo del hilo.
} WorkerInfo;

#endif /* _MINERO_H */