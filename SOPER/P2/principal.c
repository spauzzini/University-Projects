/**
 * @file principal.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include "principal.h"

/* Declaración Variables Globales*/
extern int n_procs;

/* Mascaras, Señales y Manejadores */
extern sigset_t mask, oldmask;
struct sigaction man_Sig1;
struct sigaction man_Sig2;
struct sigaction man_Int;
struct sigaction man_Alarm;

/* Semáforos */
extern sem_t *semCandidato;
extern sem_t *semVotacion;
extern sem_t *semReady;
extern sem_t *semStart;
extern sem_t *semRecuento;

/* Declaracion de funciones */
void mascaras ();
void manejaTodo ();
void semaforos();

int main (int argc, char *argv[]) {
    int i,res=0, sval;
    FILE *pidVotantes = NULL;
    pid_t pid;
    double n_secs;  

    if (argc != 3){
        fprintf(stderr, "Usage: %s <N_PROCS> <N_SECS> \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    n_procs = atoi(argv[1]);
    if (n_procs <2 || n_procs > MAX_VOT) {
        fprintf(stderr, "Número de procesos no válido\n");
        exit(EXIT_FAILURE);
    }
    
    n_secs = atof(argv[2]);
    if (n_secs <= 0) {
        fprintf(stderr, "Número de segundos no válido\n");
        exit(EXIT_FAILURE);
    }

    /* Comienzo temporizador */
    if(alarm(n_secs)) {
        fprintf(stderr, "Existe ya la alarma\n");
        exit(EXIT_FAILURE);
    }

    /* Creación de máscaras, manejadores y semáforos */
    mascaras();
    manejaTodo();
    semaforos();
    
    /* Creamos n_procs procesos votantes */
    for (i=0; i<n_procs; i++) {
        pid = fork();
        if (pid<0) {
            return 1;
        } else if (pid == 0) {
            res = votante();      
            if (res == 1) {
                /* Liberación de recursos */
                sem_close(semCandidato);
                sem_close(semVotacion);
                sem_close(semStart);
                sem_close(semReady);
                sem_close(semRecuento);
                sem_unlink("EligeCandidato");
                sem_unlink("FicheroVotacion");
                sem_unlink("PodemosEmpezarVotacion");
                sem_unlink("PodemosEmpezarRonda");
                sem_unlink("RecuentoVotos");
                exit(EXIT_FAILURE);
            }
            break;
        } else {
            /* Creamos fichero y guardamos los ids de los votantes */
            pidVotantes = fopen("pidVotantes.txt", "a");
            if (!pidVotantes) {
                fprintf(stderr, "Error en la creación del archivo\n");
                exit(EXIT_FAILURE);
            }
            fprintf(pidVotantes, "%d,", pid);
            fclose(pidVotantes);
        }
    }
    
    /* Esperamos a que todos los procesos estén esperando la señal SIGUSR1 */
    while(sval!=0){
        sem_getvalue(semStart, &sval);
    }

    /* Abrimos el fichero y obtenemos los ids para mandarles señal*/
    pidVotantes = fopen("pidVotantes.txt", "r");
    if (!pidVotantes) {
        fprintf(stderr, "Error en la lectura del archivo\n");
        exit(EXIT_FAILURE);
    }
    
    /* Mandamos señal a los votantes */
    while(fscanf(pidVotantes, "%d,",&pid)==1){
        kill(pid, SIGUSR1);
        sem_post(semStart);
    }

    fclose(pidVotantes);

    /* Suspendemos el proceso Principal hasta que recibamos SIGINT o SIGALRM*/
    sigsuspend(&oldmask);

    
    /* Abrimos el fichero y obtenemos los ids para hacer un wait por cada proceso */
    pidVotantes = fopen("pidVotantes.txt", "r");
    if (!pidVotantes) {
        fprintf(stderr, "Error en la lectura del archivo\n");
        exit(EXIT_FAILURE);
    }

    /* Realizamos wait */
    while(fscanf(pidVotantes, "%d,",&pid)==1){
        waitpid(pid, NULL, 0);
    }
    fclose(pidVotantes);

    /* Liberación de recursos */
    sem_close(semCandidato);
    sem_close(semVotacion);
    sem_close(semStart);
    sem_close(semReady);
    sem_close(semRecuento);
    sem_unlink("EligeCandidato");
    sem_unlink("FicheroVotacion");
    sem_unlink("PodemosEmpezarVotacion");
    sem_unlink("PodemosEmpezarRonda");
    sem_unlink("RecuentoVotos");

    exit(EXIT_SUCCESS);
}

/* --- MANEJADORES --- */
void manAlarm (int sig) { 
    FILE* pidVotantes=NULL;
    pid_t pid;
    
    /* Abrimos el fichero y obtenemos los ids para mandar SIGTERM */
    pidVotantes = fopen("pidVotantes.txt", "r");
    if (!pidVotantes) {
        fprintf(stderr, "Error en la lectura del archivo\n");
        exit(EXIT_FAILURE);
    }

    /* Mandamos señal */
    while(fscanf(pidVotantes, "%d,",&pid)==1){
        kill(pid, SIGTERM);
    }
    fclose(pidVotantes);
    
    printf("Finishing by alarm\n");
    return; 
} 


void manSig1 (int sig) { return; }  // Señal SIGUSR1 
void manSig2 (int sig) { return; } // Señal SIGUSR2

void manInt (int sig) { 
    FILE* pidVotantes=NULL;
    pid_t pid;

    /* Abrimos el fichero y obtenemos los ids para mandar SIGTERM */
    pidVotantes = fopen("pidVotantes.txt", "r");
    if (!pidVotantes) {
        fprintf(stderr, "Error en la lectura del archivo\n");
        exit(EXIT_FAILURE);
    }

    while(fscanf(pidVotantes, "%d,",&pid)==1){
        kill(pid, SIGTERM);
    }
    fclose(pidVotantes);

    printf("Finishing by signal\n");
    return; 
} // Señal SIGINT

void manejaTodo () {

    /* SIGUSR1 */
    man_Sig1.sa_handler = manSig1;
    sigemptyset(&(man_Sig1.sa_mask));
    man_Sig1.sa_flags = 0;

    if (sigaction(SIGUSR1, &man_Sig1, NULL) < 0) {
        fprintf(stderr, "Error en la funcion sigaction\n");
        exit(EXIT_FAILURE);
    }

    /* SIGUSR2 */
    man_Sig2.sa_handler = manSig2;
    sigemptyset(&(man_Sig2.sa_mask));
    man_Sig2.sa_flags = 0;

    if (sigaction(SIGUSR2, &man_Sig2, NULL) < 0) {
        fprintf(stderr, "Error en la funcion sigaction\n");
        exit(EXIT_FAILURE);
    }

    /* SIGINT */
    man_Int.sa_handler = manInt;
    sigemptyset(&(man_Int.sa_mask));
    man_Int.sa_flags = 0;

    if (sigaction(SIGINT, &man_Int, NULL) < 0) {
        fprintf(stderr, "Error en la funcion sigaction\n");
        exit(EXIT_FAILURE);
    }

    /* ALARM */
    man_Alarm.sa_handler = manAlarm;
    sigemptyset(&(man_Alarm.sa_mask));
    man_Alarm.sa_flags = 0;

    if (sigaction(SIGALRM, &man_Alarm, NULL) < 0) {
        fprintf(stderr, "Error en la funcion sigaction\n");
        exit(EXIT_FAILURE);
    }

}

/* --- MASCARAS  --- */
void mascaras () {
    /* Definimos máscaras de señales */
    sigemptyset(&mask); //
    sigemptyset(&oldmask); 

    /* Añadimos las señales */
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigaddset(&mask, SIGALRM);
    sigaddset(&mask, SIGINT);

    /* Aplicamos al proceso */
    if (sigprocmask(SIG_BLOCK, &mask, &oldmask) < 0) {
        fprintf(stderr, "Error al aplicar la máscara de señales\n");
        exit(EXIT_FAILURE);
    }
}

/* --- SEMAFOROS  --- */
void semaforos () {

    /* Semáforo para limitar un único candidato */
    if ((semCandidato = sem_open("EligeCandidato", O_CREAT, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

    /* Semáforo para definir zona crítica */
    if ((semVotacion = sem_open("FicheroVotacion", O_CREAT, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

    /* Semáforo que indica al candidato que todos los votantes están listos para votar */
    if ((semReady = sem_open("PodemosEmpezarVotacion", O_CREAT, S_IRUSR | S_IWUSR, (n_procs-1))) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }
    
    /* Semáforo que indica al principal que todos los hijos están listos para empezar */
    if ((semStart = sem_open("PodemosEmpezarRonda", O_CREAT, S_IRUSR | S_IWUSR, n_procs)) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

    /* Semáforo que indica al candidato que todos los votantes han votado */
    if ((semRecuento = sem_open("RecuentoVotos", O_CREAT, S_IRUSR | S_IWUSR, n_procs-1)) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }
}



