/**
 * @file minero.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-02-06
 * 
 * @version 1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <wait.h>

#include "minero.h"
#include "principal.h"
#include "monitor.h"
#include "pow.h"

/* Variable global: indica si se ha encontrado una solución (1) o no (0) */
static volatile int solfound = 0;
/* Variable global: indica la solucion del minero, sin revisar por el monitor */
static volatile int candidateTarget = 0;
/* Variable global: indica el objetivo inicial y se actualiza cuando el minero aprueba la candidateTarget */
static volatile int newtarget = 0;

/* Estructura del Minero */
struct _WorkerInfo {
    int totalThreads; // Numero total de hilos.
    int threadNum; // ID del hilo.
    int target; // Objetivo del hilo.
    int rangoRecorrido; // Flag que indica si el hilo recorrió su rango entero.
};

void* work(void * args){
    /* Declaración variables */
    int i;
    int myRange = 0;
    WorkerInfo* winfo = (WorkerInfo*) args;
    int inicio = 0;

    /* Cálculo del inicio del rango de trabajo */
    inicio = (PRIME/(winfo->totalThreads))*(winfo->threadNum);
    
    /* Cálculo del final del rango de trabajo */
    myRange = (PRIME/(winfo->totalThreads))*(winfo->threadNum + 1);

    /* Si eres el último hilo, modificamos el final para recorrer todo el rango */
    if (winfo->threadNum == (winfo->totalThreads-1)) {
        myRange = PRIME;
    }

    for(i=inicio; i<myRange; i++){
        if (solfound == 1) {
            pthread_exit(NULL);
        }
        // Solución encontrada
        if(winfo->target == pow_hash(i)){
            candidateTarget = i;
            solfound = 1;
            break;
        }
    }
    
    winfo->rangoRecorrido = 1;
    pthread_exit(NULL);
}

int minero (int goal, int n_rounds, int n_threads) {
    /* Declaración variables */
    pthread_t h[MAX_HILOS];
    int i, error, res;
    void * result;
    WorkerInfo info[MAX_HILOS];
    pid_t pid;
    int tuberia1[2], tuberia2[2];
    int pipe_status;
    char msg1[80], msg2[80];
    ssize_t nbytes1, nbytes2;
    char readbuffer1[80], readbuffer2[80];
          
    /*Creamos tubería1 y comprobamos*/
    pipe_status = pipe(tuberia1);
    if (pipe_status == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    /*Creamos tubería2 y comprobamos*/
    pipe_status = pipe(tuberia2);
    if (pipe_status == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    
    /* Objetivo inicial */
    newtarget = goal;
    
    
    pid = fork();
    if (pid<0) {
        perror("fork");
        return EXIT_FAILURE;

    } else if (pid == 0) {

        /* Cerramos la escritura de la tuberia 1 */
        close(tuberia1[1]);
        /* Cerramos la lectura de la tubería 2 */
        close(tuberia2[0]);

        /* Leemos hasta que monitor recibe un -1 */
       do {
            nbytes1 = 0;
            /* Monitor lee la solución que envía el Minero */
            nbytes1 = read(tuberia1[0], readbuffer1, sizeof(readbuffer1));
            if (nbytes1 == -1) {
                perror("read");
                printf("Monitor exited with status 1\n");
                return EXIT_FAILURE;
            }

            /* Comprobación del Monitor */
            res = monitor(atoi(readbuffer1), newtarget);
            if(res == 0){
                /* Solución aceptada */
                newtarget = atoi(readbuffer1);
            }
            
            sprintf(msg2, "%d", res);
            /* Monitor envía su veredicto a Minero */
            nbytes2 = write(tuberia2[1], msg2, strlen(msg2) + 1);
            if (nbytes2 == -1) {
                perror("write");
                printf("Monitor exited with status 1\n");
                return EXIT_FAILURE;
            }

       } while (strcmp(readbuffer1, "-1") != 0);
        
       /* Cerramos la lectura de la tuberia 1 */
       close(tuberia1[0]);
       /* Cerramos la escritura de la tuberia 2 */
       close(tuberia2[1]);

    } else {
       
        /* Cerramos la lectura de la tuberia 1 */
        close(tuberia1[0]);
        /* Cerramos la escritura de la tuberia 2 */
        close(tuberia2[1]);

        while (n_rounds !=0) {

            /* Inicializamos cada ronda la variable global a 0 */
            solfound = 0;
            /*Creación de hilos*/
            for (i=0; i<n_threads; i++) {

                /* Inicializamos estructura de cada hilo */
                info[i].target = newtarget;
                info[i].threadNum = i;
                info[i].totalThreads = n_threads;
                info[i].rangoRecorrido = 0;

                error = pthread_create(&h[i], NULL, work, &info[i]);
                /* Comprobamos que el pthread create se ha hecho bien */
                if (error != 0) {
                    printf("Miner exited with status 1\n");
                    perror("pthread_create");
                    return EXIT_FAILURE;
                }
            }

             /*Limpieza de hilos*/
            for (i=0; i<n_threads; i++) {
                pthread_join(h[i], &result);
            }
            
            if(solfound == 1) {
                sprintf(msg1, "%d", candidateTarget);
                nbytes1=0;
                /* Minero envia la solución a Monitor */
                nbytes1 = write(tuberia1[1], msg1, strlen(msg1) + 1);
                if (nbytes1 == -1) {
                    printf("Miner exited with status 1\n");
                    perror("write");
                    return EXIT_FAILURE;
                }
            
                nbytes2 = 0;
                /* Minero lee veredicto del Monitor */
                nbytes2 = read(tuberia2[0], readbuffer2, sizeof(readbuffer2));
                if (nbytes2 == -1) {
                    perror("read");
                    return EXIT_FAILURE;
                }

                if(strcmp(readbuffer2,"0") == 0){
                    /* Solución aceptada por el monitor, actualizamos el valor del objetivo */
                    newtarget = candidateTarget;
                }else if (strcmp(readbuffer2,"1") == 0 ) {
                    /* Solución rechazada por el monitor */
                    printf("The solution has been invalidated\n");
                }
            } else if (solfound == 0) {
                        /* Si el hilo recorrió su rango y no encontró la solución, continua el siguiente */
                        printf("No se ha encontrado la solución\n");
                    }
            n_rounds--;
        }
        /* Rondas acabadas, mandamos un -1 al Monitor para que acabe */
        sprintf(msg1, "-1");
        nbytes1 = 0;
        nbytes1 = write(tuberia1[1], msg1, strlen(msg1) + 1);
        if (nbytes1 == -1) {
            perror("write");
            return EXIT_FAILURE;
        }

        /* Cerramos la escritura de la tuberia 1 */
        close(tuberia1[1]);
        /* Cerramos la lectura de la tuberia 2 */
        close(tuberia2[0]);

        /* Proceso Minero espera a que acabe Monitor */
        wait(NULL);
        printf("Monitor exited with status 0\n");
    }
    return EXIT_SUCCESS;
}
