/**
 * @file principal.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <wait.h>

#include "minero.h"
#include "principal.h"

int main (int argc, char *argv[]){
    int n_rounds, n_threads, err;
    long int goal;
    pid_t pid;

    /* Comprobamos si el número de argumentos es correcto */
    if (argc != 4){
        fprintf(stderr, "Usage: %s <TARGET_INI> <ROUNDS> <N_THREADS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Comprobamos y guardamos los argumentos en las variables correspondientes */
    goal = atol(argv[1]);
    if (goal<0) {
        fprintf(stderr, "Goal negativa\n");
        exit(EXIT_FAILURE);
    }
    
    n_rounds = atol(argv[2]);
    
    n_threads = atol(argv[3]);
    if (n_threads>MAX_HILOS) {
        fprintf(stderr, "Límite de hilos superado\n");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if(pid<0){
        return 1;
    } else if (pid == 0){
        /* Lanzamos proceso minero */
        err = minero(goal, n_rounds, n_threads);
        printf("Miner exited with status %d\n", err);
    }else{
        /* Proceso principal espera a minero */
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
    exit(EXIT_SUCCESS);
}
