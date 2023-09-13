/**
 * @file votante.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include "votante.h"

int n_procs;
sem_t *semCandidato;
sem_t *semVotacion;
sem_t *semSignal;
sem_t *semReady;
sem_t *semStart;
sem_t *semRecuento;
sigset_t mask, oldmask;

int generateRandomVote();

int votante () {
    char voto;
    FILE *votacion=NULL;
    srand(time(NULL));

    /* Los votantes indican a principal que están listos para empezar */
    sem_wait(semStart);
    /* Espera no activa de la señal SIGUSR1*/
    sigsuspend(&oldmask);

    /* Comienzo de rondas */
    while (1) {
        if (sem_trywait(semCandidato) == 0) {
            /* Primer hijo que entra se convierte en Candidato y hace su ejecución */
            candidato();
        } else {
            /* Resto hijos: votantes */
            /* Los votantes indican a Candidato que están listos para votar */
            sem_wait(semReady);
            /* Espera no activa de la señal SIGUSR1 */
            sigsuspend(&oldmask);
            
            /* Comienzo Zona-Crítica de Votación */
            sem_wait(semVotacion);

            /* Abren el fichero y escriben su voto */
            votacion = fopen("votacion.txt", "a");
            if (!votacion) {
                printf("Error en la creación del archivo\n");
            }
            
            /* Calculamos voto */
            if (generateRandomVote() == 0) {
            voto = 'N';
            } else {
            voto = 'Y';
            }
            
            /* Se escribe el voto en el fichero */
            fprintf(votacion, "%c", voto);
            
            /* Indica al candidato que ha votado */
            sem_wait(semRecuento);
            fclose(votacion);
            /* Fin Zona-Crítica de Votación */
            sem_post(semVotacion); 
            
            
            /* Espera no activa de la señal SIGUSR1 para nueva ronda */
            sigsuspend(&oldmask);
        }
    }
    return 0;
}

int candidato() {
    int i, k=0, j=0;
    char v[100];
    int negs=0, poss=0, sval,recval=0;
    FILE *votacion=NULL;
    FILE *pidVotantes=NULL;
    pid_t pids[MAX_VOT];
    pid_t pid;

    /* Comprueba si todos los votantes están esperando su señal */
    sem_getvalue(semReady, &sval);
    while(sval!=0){
        sem_getvalue(semReady, &sval);
    }

    /* Abrimos el fichero y obtenemos los ids para trabajar con ellos */
    pidVotantes = fopen("pidVotantes.txt", "r");
    if (!pidVotantes) {
        fprintf(stderr, "Error en la creación del archivo\n");
        exit(EXIT_FAILURE);
    }
    /* Guarda los ids en un array */
    while(fscanf(pidVotantes,"%d,",&pid)==1){
        pids[j]=pid; fprintf(pidVotantes, "%d,", pid);
        j++;
    }
    fclose(pidVotantes);

    /* Mandamos SIGUSR2 a todos los votantes para que comiencen a votar */
    for (i=0; i<n_procs; i++) {
        if (getpid() != pids[i]) {
            kill(pids[i], SIGUSR2);    
        }
    }

    /* Comprueba si todos los votantes han votado cada 1ms */
    sem_getvalue(semRecuento, &recval);
    while(recval!=0){
        usleep(1000);
        sem_getvalue(semRecuento, &recval);
    }

    /* ---ABRIMOS ZONA CRITICA LECTURA--- */
    sem_wait(semVotacion);  
    votacion = fopen("votacion.txt", "r");
    while (!feof(votacion)) { 
        v[k] = fgetc(votacion);
        k++;
    }
    fclose(votacion);
    sem_post(semVotacion);
    /* --- CERRAMOS ZONA CRITICA LECTURA--- */

    /* Recuento votacion y mostramos veredicto */
    printf("Candidate %d => [ ", getpid());
    for(int j=0; j<k-1; j++){
        if (v[j] == 'Y') { poss++; } else { negs++; }
        printf("%c ", v[j]);
    }
    /* Mostramos resultado de la votación */
    if (poss>negs) { printf("] => Accepted\n"); } else { printf("] => Rejected\n"); }
    
    /* Espera no activa de 250ms */
    usleep(250000);

    /* Reestablecemos los valores de los semáforos para comenzar la nueva ronda */
    for (i=0; i<n_procs-1; i++) {
        sem_post(semReady);
        sem_post(semRecuento);
    }

    /* Indicamos comienzo de nueva ronda mandando SIGUSR1*/
    for (i=0; i<n_procs; i++) {
        if (getpid() != pids[i]) {
            kill(pids[i], SIGUSR1);    
        }
    }

    /* Subimos el valor del semáforo para permitir ser Candidato a otro proceso en la nueva ronda*/
    sem_post(semCandidato);

    remove("votacion.txt");
    return 0;
}


int generateRandomVote(){
    int num;
    num = rand()%2;
    usleep(num);
    return num;
}