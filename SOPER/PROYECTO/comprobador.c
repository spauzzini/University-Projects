/**
 * @file comprobador.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include "comprobador.h"

struct mq_attr attr; // Estructura para la cola de mensajes (inicializada en Minero)
sem_t *semActivo; // Semaforo que indica que el monitor está activo

void semaphores(ComminMem *shm_commin);

int main() {
    pid_t pid;
    Block currentBlock, printblock;
    int res, fd_shm;
    ComminMem *shm_commin=NULL;
    mqd_t queue;

    /* Abrimos el semáforo común a Minero */
    if ((semActivo = sem_open("semActivo", O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

    /* Indicamos a Minero que el Monitor está activo */
    sem_post(semActivo);

    /* Creamos segmento de memoria compartida */
    fd_shm = shm_open(SHM_NAME_CM, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if(fd_shm == -1) {
        if(errno == EEXIST) {
            if ((fd_shm = open(SHM_NAME_CM, O_RDWR , 0)) == -1) {
                perror("open");
                exit(EXIT_FAILURE);
            }
        } else {
            perror("open");
            exit(EXIT_FAILURE);
        }
    } else {
        if(ftruncate(fd_shm, sizeof(ComminMem)) == -1) {
            fprintf(stderr, "ftruncate error\n");
            exit(EXIT_FAILURE);
        }
    }

    shm_commin = mmap(NULL, sizeof(ComminMem), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if(shm_commin == MAP_FAILED) {
        fprintf(stderr, "Error en el mmap\n");
        exit(EXIT_FAILURE);
    }

    /* Inicializamos semáforos de la memoria compartida (Comprobador-Monitor) */
    semaphores(shm_commin);

    pid = fork();
    if (pid<0) {
        fprintf(stderr, "Error en el fork\n");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        while(1){
        /* --- INICIO ZONA CRÍTICA --- */   
            sem_wait(&shm_commin->semFill);
            sem_wait(&shm_commin->semMutex);

            /* Mandamos bloque comprobado a Monitor */
            printblock = shm_commin->bloques[0]; 
            /* Actualizamos indice y buffer */
            for(int k=1; k <= shm_commin->idx_last; k++) {
                shm_commin->bloques[k-1] = shm_commin->bloques[k];
            }
            shm_commin->idx_last--;
            if (shm_commin->idx_last == -1) {
                shm_commin->idx_last++;
            }

        sem_post(&shm_commin->semMutex);
        sem_post(&shm_commin->semEmpty);
        /* --- FIN ZONA CRÍTICA --- */  

        /* Mostramos en la pantalla el Bloque recibido */
            if (printblock.id < 10) {
                /* ID de 1 cifra */
                printf("Id:       000%d\n", printblock.id);
            } else if (printblock.id < 99) {
                /* ID de 2 cifras */
                printf("Id:       00%d\n", printblock.id);
            } else if (printblock.id < 999) {
                /* ID de 3 cifras */
                printf("Id:       0%d\n", printblock.id);
            } else {
                /* ID de 4 cifras */
                printf("Id:       %d\n", printblock.id);
            }
            
            printf("Winner:   %d\n", printblock.winner);
            printf("Target:   %d\n", printblock.target);
            printf("Solution: %d ", printblock.solution);
            if (printblock.correct == 1) {
                printf("(validated)\n");
            } else if (printblock.correct == 0) {
                printf("(rejected)\n");
            }
            printf("Votes:    %d/%d\n", printblock.posVotes, printblock.totalVotes);
            printf("Wallets:  ");
            /* Imprimimos las carteras de los que han participado en esta ronda */
            for(int p=0; p<printblock.totalVotes; p++) {
                printf("%d:0%d   ", printblock.carteras[p].pid, printblock.carteras[p].monedas);
            }
            printf("\n\n");
        
        }
       
    } else {
        /* COMPROBADOR */

        attr.mq_flags=MQ_FLAGS;
        attr.mq_maxmsg=MQ_MAXMSG;
        attr.mq_curmsgs=MQ_CURMSG;
        attr.mq_msgsize=sizeof(Block);

        /* Inicializamos cola con la estructura (La cola debe crearla el minero, sino aqui da error) */
        queue = mq_open(MQ_NAME, O_RDONLY, S_IRUSR | S_IWUSR, &attr);
        if (queue == (mqd_t)-1) {
            perror("Error al abrir la cola de mensajes");
            return 1;
        }

        sem_wait(&shm_commin->semMutex);
        shm_commin->idx_last = -1; // Mandamos bloque comprobado a Monitor
        sem_post(&shm_commin->semMutex);
        
        while(1) {
            /* Extraemos bloque de la cola */
            if(mq_receive(queue, (char *)&currentBlock, sizeof(Block), NULL) == -1) {
                fprintf(stderr, "Error recceiving message\n");
                return 1;
            }

            /* Comprobamos que la solucion es correcta */
            res = pow_hash(currentBlock.solution);
            if (currentBlock.target == res) {
                /* Si es correcta, modificamos el valor de la estructura para que Monitor lo sepa */
                currentBlock.correct = 1;
            }

            /* --- INICIO ZONA CRITICA --- */
            sem_wait(&shm_commin->semEmpty);
            sem_wait(&shm_commin->semMutex);

                /* Mandamos bloque comprobado a Monitor */
                if(shm_commin->idx_last == -1){
                    shm_commin->bloques[0] = currentBlock;
                    shm_commin->idx_last++;

                }else{
                    shm_commin->bloques[shm_commin->idx_last] = currentBlock;

                }
                shm_commin->idx_last++;
                shm_commin->idx_last = shm_commin->idx_last%5;

            sem_post(&shm_commin->semMutex);
            sem_post(&shm_commin->semFill);
            /* --- FIN ZONA CRITICA --- */

        }
    }
    sem_wait(semActivo);
}

void semaphores(ComminMem *shm_commin) {
    if(sem_init(&shm_commin->semEmpty, 1, 5) == -1) {
        fprintf(stderr, "Error abriendo semaforo\n");
        exit(EXIT_FAILURE);
    }

    if(sem_init(&shm_commin->semMutex, 1, 1) == -1) {
        fprintf(stderr, "Error abriendo semaforo\n");
        exit(EXIT_FAILURE);
    }

    if(sem_init(&shm_commin->semFill, 1, 0) == -1) {
        fprintf(stderr, "Error abriendo semaforo\n");
        exit(EXIT_FAILURE);
    }
}