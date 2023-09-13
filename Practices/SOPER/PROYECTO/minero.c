/**
 * @file minero.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include "minero.h"

int initTgt; // objetivo inicial
static volatile int solfound; // flag q indica si un hilo encontro la sol
int candidateTgt; // objetivo a comprobar
int flagvotacion; // flag q indica si un minero encontro la sol
int rounds;

sigset_t mask, oldmask, nomask;
struct sigaction man_Sig1;
struct sigaction man_Sig2;
struct sigaction man_Int;
struct sigaction man_Alarm;

struct mq_attr attr;

sem_t *semPids;
sem_t *semGanador;
sem_t *semActivo; // Semaforo que indica que el monitor está activo 

/* Declaracion Funciones */
int sendxPipe(int tuberia[2], Block block); 
void *work(void *args); 
void semaforos();
void semSharedMem(SysMemory *shm_sys);
void mascaras();
void manejadores();
int votacion(int id, SysMemory *shm_sys, mqd_t queue); 

int main(int argc, char *argv[]) { 
    int n_threads, n_secs; //argumentos
    SysMemory *shm_sys=NULL;
    int fd_shm;
    pid_t pid; 
    int tuberia[2], pipe_status;
    WorkerInfo info[MAX_HILOS];
    pthread_t h[MAX_HILOS];
    int i, error, k, fdtxt;
    void *result;
    Block reg;
    FILE *pidMineros=NULL;
    char title[MAX_CHAR];
    ssize_t nbytesreg;
    mqd_t queue;

    /* Comprobacion argumentos */
    if (argc != 3){
        fprintf(stderr, "Usage: %s <N_SECONDS> <N_THREADS> \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    n_secs = atoi(argv[1]);
    if (n_secs < 0 ) {
        fprintf(stderr, "Los segundos no pueden ser negativos\n");
        exit(EXIT_FAILURE);
    }

    n_threads = atoi(argv[2]);
    if (n_threads > MAX_HILOS) {
        fprintf(stderr, "Límite de hilos excedido\n");
        exit(EXIT_FAILURE);
    }

    /* Comienzo temporizador */
    if(alarm(n_secs)) {
        fprintf(stderr, "Existe ya la alarma\n");
        exit(EXIT_FAILURE);
    }

    /* Creación de máscaras, manejadores y semáforos */
    mascaras();
    manejadores();
    semaforos();

    if ((semActivo = sem_open("semActivo", O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

    /*Creamos tubería y comprobamos*/
    pipe_status = pipe(tuberia);
    if (pipe_status == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    /* Inicializacion de la cola de mensajes */
    attr.mq_flags=MQ_FLAGS;
    attr.mq_maxmsg=MQ_MAXMSG;
    attr.mq_curmsgs=MQ_CURMSG;
    attr.mq_msgsize=sizeof(Block);

    /* Creamos la cola de mensajes */
    queue = mq_open(MQ_NAME, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR, &attr);
    if (queue == (mqd_t)-1) {
        perror("Error al abrir la cola de mensajes");
        mq_close(queue);
        mq_unlink(MQ_NAME);
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid<0) {
        fprintf(stderr, "Error en el fork\n");
        exit(EXIT_FAILURE);
    } else if (pid==0) {
        /* --- REGISTRADOR --- */

        while(1) {
            sleep(1);
            /* LEEMOS (pipe minero-reg) */
            nbytesreg = read(tuberia[0], &reg, sizeof(Block));
            if (nbytesreg == 0) {
                perror("read");
                exit(EXIT_FAILURE);
            }

            /* OBTENEMOS NOMBRE FILE */
            sprintf(title, "%d", getppid());
            strcat(title, ".txt");

            /* ABRIMOS ARCHIVO */
            sem_wait(semPids);
            pidMineros = fopen(title, "a");
            if (!pidMineros) {
                fprintf(stderr, "Error en la creación del archivo\n");
                exit(EXIT_FAILURE);
            }

            fdtxt = fileno(pidMineros);

            /* ESCRIBIMOS EN EL FILE EL BLOQUE*/
            if (reg.id < 10) {
                /* ID de 1 cifra */
                dprintf(fdtxt, "Id:       000%d\n", reg.id);
            } else if (reg.id < 99) {
                /* ID de 2 cifras */
                dprintf(fdtxt, "Id:       00%d\n", reg.id);
            } else if (reg.id < 999) {
                /* ID de 3 cifras */
                dprintf(fdtxt, "Id:       0%d\n", reg.id);
            } else {
                /* ID de 4 cifras */
                dprintf(fdtxt, "Id:       %d\n", reg.id);
            }
            
            dprintf(fdtxt, "Winner:   %d\n", reg.winner);
            dprintf(fdtxt, "Target:   %d\n", reg.target);
            dprintf(fdtxt, "Solution: %d ", reg.solution);
            if (reg.correct == 1) {
                dprintf(fdtxt, "(validated)\n");
            } else if (reg.correct == 0) {
                dprintf(fdtxt, "(rejected)\n");
            }
            dprintf(fdtxt, "Votes:    %d/%d\n", reg.posVotes, reg.totalVotes);
            dprintf(fdtxt, "Wallets:  ");
            /* Imprimimos las carteras de los que han participado en esta ronda */
            for(int p=0; p<reg.totalVotes; p++) {
                dprintf(fdtxt, "%d:0%d   ", reg.carteras[p].pid, reg.carteras[p].monedas);
            }
            dprintf(fdtxt, "\n\n");

            fclose(pidMineros);
            sem_post(semPids);
            sleep(1);
        }
        /* ------------------- */
        exit(EXIT_SUCCESS);
    } else {
        /* ----- MINERO ----- */
        fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        if(fd_shm  == -1) { 
            if (errno == EEXIST) { 
                /* INICIALIZAR MEMORIA */
                
                fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
                if (fd_shm  == -1) {
                    fprintf(stderr, "shmopen error\n");
                    exit(EXIT_FAILURE);
                }
                
                shm_sys = mmap(NULL, sizeof(SysMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
                close(fd_shm);
                if(shm_sys == MAP_FAILED) {
                    perror("Error en el mmap del monitor\n");
                    exit(EXIT_FAILURE);
                }

                /* REGISTRO ID EN MEMORIA COMPARTIDA */
                sem_wait(&shm_sys->semMutex);
                shm_sys->pids[shm_sys->currentSysMiners] = getpid();
                if (shm_sys->currentSysMiners == MAX_MINEROS) {
                    sem_post(&shm_sys->semMutex);
                    printf("Se ha excedido el número de Mineros en el Sistema\n");
                    exit(EXIT_SUCCESS);
                }
                shm_sys->currentSysMiners++;
                sem_post(&shm_sys->semMutex);

                while(1) {
                    sigsuspend(&oldmask);
                    
                    sem_wait(&shm_sys->semMutex);
                    shm_sys->playingMiners++;
                    sem_post(&shm_sys->semMutex);

                    for(i=0; i<n_threads; i++) {
                        /* Inicializamos estructura de cada hilo */
                        info[i].target = initTgt;
                        info[i].threadNum = i;
                        info[i].totalThreads = n_threads;

                        error = pthread_create(&h[i], NULL, work, &info[i]);
                        /* Comprobamos que el pthread create se ha hecho bien */
                        if (error != 0) {
                            perror("pthread_create");
                            exit(EXIT_FAILURE);
                        }
                    }

                    sleep(1);

                    /*Limpieza de hilos*/
                    for (i=0; i<n_threads; i++) {
                        pthread_join(h[i], &result);
                    }
                    
                    if (sigprocmask(SIG_SETMASK, &nomask, NULL) < 0 ) {
                        perror("sigprocmask\n");
                        exit(EXIT_FAILURE);
                    }

                    if (solfound == 1 && flagvotacion == 0) {
                        sem_wait(&shm_sys->semMutex);
                        for (k=0; k<shm_sys->playingMiners; k++) {
                            if(shm_sys->pids[k] != getpid()) {
                                kill(shm_sys->pids[k], SIGUSR2);
                            }
                        }
                        sem_post(&shm_sys->semMutex);
                    }
                    
                    votacion(rounds, shm_sys, queue);
                    
                    while(1){
                        sleep(1);
                        sem_wait(&shm_sys->semMutex);
                        if(shm_sys->playingMiners == shm_sys->currentBlock.totalVotes) {
                            sem_post(&shm_sys->semMutex);
                            break;
                        }
                        sem_post(&shm_sys->semMutex);
                    }
                    /* Mandamos por tuberia el bloque a nuestro hijo reg */
                    sem_wait(&shm_sys->semMutex);
                    sendxPipe(tuberia, shm_sys->currentBlock);
                    sem_post(&shm_sys->semMutex);

                    initTgt = candidateTgt;
                    flagvotacion = 0;
                    solfound = 0;
                }

                close(tuberia[0]); 
                close(tuberia[1]); 
                exit(EXIT_SUCCESS);       
            }
        }
        /*---------------------------------------------------------------------------------------------------------*/
        // PRIMER MINERO, SE HACE FTRUNCATE Y SE MAPEA (esto solo se ejecuta una vez)
        if(ftruncate(fd_shm, sizeof(SysMemory)) == -1) {
            fprintf(stderr, "ftruncate error\n");
            exit(EXIT_FAILURE);
        }

        /* Mapeamos la memoria compartida para COMPROBADOR*/
        shm_sys = mmap(NULL, sizeof(SysMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
        close(fd_shm);
        if(shm_sys == MAP_FAILED) {
            fprintf(stderr, "Error en el mmap\n");
            exit(EXIT_FAILURE);
        }
        
        /* REGISTRO ID EN MEMORIA COMPARTIDA Y SUMO CURRENTSYSMINERS */
        semSharedMem(shm_sys);
        sem_wait(&shm_sys->semMutex);
        shm_sys->pids[0] = getpid();
        shm_sys->currentSysMiners++;
        shm_sys->playingMiners=1;
        sem_post(&shm_sys->semMutex);

        // establecer objetivo inicial
        initTgt = 0;
        rounds = 0;
        flagvotacion = 0;
        
        while(1) {
            sleep(1);
            
            // envia SIGUSR1 a los demas
            sem_wait(&shm_sys->semMutex);
            for (k=1; k<shm_sys->currentSysMiners; k++) {
                kill(shm_sys->pids[k], SIGUSR1);
            }
            sem_post(&shm_sys->semMutex);

            if (sigprocmask(SIG_SETMASK, &nomask, NULL) < 0 ) {
                perror("sigprocmask\n");
                exit(EXIT_FAILURE);
            }

            
            for(i=0; i<n_threads; i++) {
                /* Inicializamos estructura de cada hilo */
                info[i].target = initTgt;
                info[i].threadNum = i;
                info[i].totalThreads = n_threads;

                error = pthread_create(&h[i], NULL, work, &info[i]);
                /* Comprobamos que el pthread create se ha hecho bien */
                if (error != 0) {
                    perror("pthread_create");
                    exit(EXIT_FAILURE);
                }
            }
            
            sleep(1);

            /* Limpieza de hilos */
            for (i=0; i<n_threads; i++) {
                pthread_join(h[i], &result);
            }

            if (solfound == 1 && flagvotacion == 0) {
                
                sem_wait(&shm_sys->semMutex);
                for (k=0; k<shm_sys->playingMiners; k++) {
                    if(shm_sys->pids[k] != getpid()) {
                        kill(shm_sys->pids[k], SIGUSR2);
                    }
                }
                sem_post(&shm_sys->semMutex);
            }
            
            votacion(rounds, shm_sys, queue);

            while(1) {
                sleep(1);
                sem_wait(&shm_sys->semMutex);
                if(shm_sys->playingMiners == shm_sys->currentBlock.totalVotes) {
                    rounds++;
                    shm_sys->playingMiners=1;
                    sem_post(&shm_sys->semMutex);
                    break;
                }
                sem_post(&shm_sys->semMutex);
            }
            /* Mandamos por tuberia el bloque a nuestro hijo reg */
            sem_wait(&shm_sys->semMutex);
            sendxPipe(tuberia, shm_sys->currentBlock);
            sem_post(&shm_sys->semMutex);

            initTgt = candidateTgt;
            flagvotacion = 0;
            solfound = 0;
        }
        
        close(tuberia[0]); 
        close(tuberia[1]); 
        exit(EXIT_SUCCESS);
    }
}


/**
 * @brief Funcion de votación. El primer proceso que entra es Ganador, los demás Perdedores.
 * Ganador: inicializa bloque, vota y espera a que los perdedores voten. Luego actualiza monedas y carteras y si el 
 * Monitor está activo le manda por la cola el bloque.
 * Perdedor: comprueba que la solucion del bloque es correcta y vota.
 *
 * @param id int: número de ronda.
 * @param shm_sys SysMemory: memoria compartida del sistema.
 * @param queue mqd_t: cola compartida entre Minero-Comprobador. (Solo accede Ganador)
*/
int votacion(int id, SysMemory* shm_sys, mqd_t queue) {
    Block cb;//bloque a meter en memoria
    int res, idx1, idx2, sval;

    if (sem_trywait(semGanador) == 0) {
        
        /* Inicializacion Bloque */
        cb.id = id;
        cb.target = initTgt; //actualizar siguiente ronda?
        cb.solution = candidateTgt; //actualizar siguiente ronda?
        cb.totalVotes = 1;
        cb.winner = getpid();
        cb.posVotes = 1;

        sem_wait(&shm_sys->semMutex);
            for(int i=0; i<shm_sys->playingMiners; i++) {
                cb.carteras[i].pid = shm_sys->pids[i];
                cb.carteras[i].monedas = 0;
            }
            
            /* Metemos bloque en memoria compartida */
            shm_sys->currentBlock = cb;
            for (int c=0; c<shm_sys->playingMiners;c++) {
                if(shm_sys->pids[c]==getpid()){
                    idx1=c;
                    break;
                }
            }
            shm_sys->votos[idx1] = 1;
            shm_sys->currentBlock.correct=0; //De momento es 0, si el recuento es favorable lo modifica a 1 (validated) sino se queda en 0 (rejected)
        
            while(1){
                sleep(1);
                sem_wait(&shm_sys->semMutex);
                    if(shm_sys->playingMiners == shm_sys->currentBlock.totalVotes) {
                        sem_post(&shm_sys->semMutex);
                        break;
                    }
                sem_post(&shm_sys->semMutex);
            }

            if(shm_sys->currentBlock.posVotes >= shm_sys->currentBlock.totalVotes/2) {
                /* Actualizamos monedas en sysmem */
                shm_sys->monedas[idx1]++;
                shm_sys->currentBlock.correct=1; // 1:Validated & 0:rejected
                /* Actualizo las carteras en bloque */
                shm_sys->currentBlock.carteras[idx1].pid=getpid();
                shm_sys->currentBlock.carteras[idx1].monedas = shm_sys->monedas[idx1];
            }
            
            /* COMPROBAMOS SI HAY MONITOR ACTIVO: sem=1 */
            sem_getvalue(semActivo, &sval);
            if (sval == 1) {
                /* GANADOR envia a COMPROBADOR el bloque */
                if(mq_send(queue, (char*)&shm_sys->currentBlock, sizeof(Block), 1)) {
                    fprintf(stderr, "Error sending message\n");
                    exit(EXIT_FAILURE);
                }
            } // Si no está activo, no se envia el bloque
            
        sem_post(&shm_sys->semMutex);
        sem_post(semGanador);
        
        return EXIT_SUCCESS;
    } else {
        sleep(1);//espero a que ganador me inicialice todo, cuando gestionemos señales cambiamos
        //sigsuspend(&oldmask);
        
        sem_wait(&shm_sys->semMutex);
            /* Comprobamos que la solucion es correcta */
            res = pow_hash(shm_sys->currentBlock.solution);
            if (shm_sys->currentBlock.target == res) {
                //es valido -> votas +1
                shm_sys->currentBlock.posVotes++;
                for(int j=0; j<shm_sys->playingMiners;j++) {
                    if(shm_sys->pids[j]==getpid()){
                        idx2=j;
                        break;
                    }
                }
                shm_sys->votos[idx2] = 1;
            }
            shm_sys->votos[idx2] = 0;
            /* Notificas que has votado */
            shm_sys->currentBlock.totalVotes++;
        sem_post(&shm_sys->semMutex);
        
        return EXIT_SUCCESS;
    }

}

/**
 * @brief Funcion que envía un bloque por una tubería.
 *
 * @param tuberia int: descriptor de tubería
 * @param block Block: bloque a enviar
*/
int sendxPipe(int tuberia[2], Block block) {
    ssize_t nbytes;
    
    nbytes = write(tuberia[1], &block, sizeof(Block));
    if(nbytes == -1) {
        perror("write");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief Funcion que realiza el minado. Accedida por hilos.
 *
 * @param args void*: argumentos necesarios
*/
void* work(void * args) {
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

    for(i=inicio; i<myRange; i++) {
        // Solución encontrada
        if(flagvotacion == 1) {
            break;
        }
        
        if(winfo->target == pow_hash(i)) {
            candidateTgt = i;
            solfound = 1;
            break;
        }
    }
    pthread_exit(NULL);
}

/* --- MANEJADORES --- */
void manAlarm (int sig) { 
    int fd_shm, k;
    SysMemory *shm_sys=NULL;

    fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
    if (fd_shm  == -1) {
        fprintf(stderr, "shmopen error\n");
        exit(EXIT_FAILURE);
    }
    
    shm_sys = mmap(NULL, sizeof(SysMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if(shm_sys == MAP_FAILED) {
        perror("Error en el mmap del monitor\n");
        exit(EXIT_FAILURE);
    }
    
    sem_wait(&shm_sys->semMutex);
    for (k=0; k<shm_sys->playingMiners; k++) {
        kill(shm_sys->pids[k], SIGTERM);
        printf("Le envio SIGTERM al proceso %d\n", shm_sys->pids[k]);
    }
    sem_post(&shm_sys->semMutex);
    
    munmap(shm_sys, sizeof(SysMemory));
    printf("Finishing by alarm\n");
    return; 
} 

void manSig1 (int sig) { 
}  // Señal SIGUSR1 

void manSig2 (int sig) { 
    printf("%d: Recibo SIGUSR2, flag=1\n", getpid());
    flagvotacion=1;
} // Señal SIGUSR2

void manInt (int sig) { 
    int fd_shm, k;
    SysMemory *shm_sys=NULL;

    fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
    if (fd_shm  == -1) {
        fprintf(stderr, "shmopen error\n");
        exit(EXIT_FAILURE);
    }
    
    shm_sys = mmap(NULL, sizeof(SysMemory), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if(shm_sys == MAP_FAILED) {
        perror("Error en el mmap del monitor\n");
        exit(EXIT_FAILURE);
    }
    
    sem_wait(&shm_sys->semMutex);
    for (k=0; k<shm_sys->playingMiners; k++) {
        kill(shm_sys->pids[k], SIGTERM);
        printf("Le envio SIGTERM al proceso %d\n", shm_sys->pids[k]);
    }
    sem_post(&shm_sys->semMutex);
    
    munmap(shm_sys, sizeof(SysMemory));
    printf("Finishing by signal\n");
    return; 
} // Señal SIGINT

void manejadores () {

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
    sigemptyset(&nomask); 

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

void semaforos() {

    /* Semáforo para proteger el acceso al archivo de PIDS, impedir que varios registradores accedan simultaneamente */
    if ((semPids = sem_open("FicheroPids", O_CREAT, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        fprintf(stderr, "Error abriendo semaforo\n");
        exit(EXIT_FAILURE);
    }

    /* Semáforo para limitar un único ganador */
    if ((semGanador = sem_open("semGanador", O_CREAT, S_IRUSR | S_IWUSR, 1)) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

}

/* Semáforos de la memoria compartida */
void semSharedMem(SysMemory* shm_sys) {
    if(sem_init(&shm_sys->semEmpty, 1, 1) == -1) {
        fprintf(stderr, "Error abriendo semaforo\n");
        exit(EXIT_FAILURE);
    }

    if(sem_init(&shm_sys->semMutex, 1, 1) == -1) {
        fprintf(stderr, "Error abriendo semaforo\n");
        exit(EXIT_FAILURE);
    }

    if(sem_init(&shm_sys->semFill, 1, 0) == -1) {
        fprintf(stderr, "Error abriendo semaforo\n");
        exit(EXIT_FAILURE);
    }
}
