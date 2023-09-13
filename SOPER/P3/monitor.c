/**
 * @file monitor.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include "monitor.h"

/* Declaracion funciones */
void semaforos(CircularBuffer *shm_struct);
void limpiaSem(CircularBuffer *shm_struct);

sem_t *semReady;

/* Estructura para la cola de mensajes (inicializada en Minero) */
struct mq_attr attr;

int main(int argc, char *argv[]) {
    int fd_shm;
    CircularBuffer *shm_struct_comp=NULL;
    CircularBuffer *shm_struct_mon=NULL;
    int lag;

    /* Comprobacion argumentos */
    if (argc != 2){
        fprintf(stderr, "Usage: %s <LAG> \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    /* Guardamos lag */
    lag = atoi(argv[1]);
    if (lag < 0 ) {
        fprintf(stderr, "Lag no válido\n");
        exit(EXIT_FAILURE);
    }

    if ((semReady = sem_open("semReady", O_CREAT, S_IRUSR | S_IWUSR, 0)) == SEM_FAILED) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

    /* Creamos segmento de memoria compartida */
    fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if(fd_shm  == -1) { 
        /* Comprobamos si ya estaba creada */
        if (errno == EEXIST) {
            /* Estaba creada -> PROCESO MONITOR */
            fd_shm = shm_open(SHM_NAME, O_RDWR, 0);
            if (fd_shm  == -1) {
                fprintf(stderr, "shmopen error\n");
                limpiaSem(shm_struct_mon);
                exit(EXIT_FAILURE);
            }

            /* Mapeamos la memoria compartida para MONITOR */
            shm_struct_mon = mmap(NULL, sizeof(CircularBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
            close(fd_shm);
            if(shm_struct_mon == MAP_FAILED) {
                perror("Error en el mmap del monitor\n");
                limpiaSem(shm_struct_mon);
                exit(EXIT_FAILURE);
            }

            sem_post(semReady);

            if (monitor(lag, shm_struct_mon) == 0){
                /* UNMAP y LIBERAR MEMORIA */
                limpiaSem(shm_struct_mon);
                munmap(shm_struct_mon, sizeof(CircularBuffer));
                shm_unlink(SHM_NAME);
                printf("[%d] Finishing\n", getpid());
                exit(EXIT_SUCCESS);
            } else {
                limpiaSem(shm_struct_mon);
                munmap(shm_struct_mon, sizeof(CircularBuffer));
                shm_unlink(SHM_NAME);
                exit(EXIT_FAILURE);
            }
        } 
    }
    
    printf("[%d] Checking blocks...\n", getpid());
    if (comprobador(lag, shm_struct_comp, fd_shm) == 0){
        printf("[%d] Finishing\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        limpiaSem(shm_struct_comp);
        munmap(shm_struct_mon, sizeof(CircularBuffer));
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    
}

/**
 * @brief Monitor extrae bloques de la memoria compartida y muestra por pantalla
 * el resultado.
 *
 * @param lag int: retraso entre cada ronda.
 * @param shm_struct_mon CircularBuffer: estructura de la memoria compartida.
*/
int monitor (int lag, CircularBuffer * shm_struct_mon) {
    Block bloquerecibido;

    sem_wait(&shm_struct_mon->semMutexMemoria);
    shm_struct_mon->idx_last = 0;
    sem_post(&shm_struct_mon->semMutexMemoria);

    printf("[%d] Printing blocks...\n", getpid());
   
    while(1) {
        /* --- INICIO ZONA CRÍTICA --- */
        sem_wait(&shm_struct_mon->semFillMemoria);
        sem_wait(&shm_struct_mon->semMutexMemoria);

        /* Extraemos bloque */
        bloquerecibido = shm_struct_mon->bloques[0];

        /* Actualizamos indice y buffer */
        for(int k=1; k <= shm_struct_mon->idx_last; k++) {
            shm_struct_mon->bloques[k-1] = shm_struct_mon->bloques[k];
        }
        shm_struct_mon->idx_last--;
        if (shm_struct_mon->idx_last == -1) {
            shm_struct_mon->idx_last++;
        }

        if(bloquerecibido.accepted == -1) {
            /* Si recibimos bloque especial terminamos */
            sem_post(&shm_struct_mon->semMutexMemoria);
            sem_post(&shm_struct_mon->semEmptyMemoria);
            break;
        } else if(bloquerecibido.accepted == 1) {
            printf("Solution accepted: %d --> %d\n", bloquerecibido.obj, bloquerecibido.sol);
        } else {
            printf("Solution rejected: %d --> %d\n", bloquerecibido.obj, bloquerecibido.sol);
        }

        sem_post(&shm_struct_mon->semMutexMemoria);
        sem_post(&shm_struct_mon->semEmptyMemoria);
        /* --- FIN ZONA CRÍTICA --- */
        
        /* Realizamos lag */
        usleep(lag*1000);
    }
    return 0;
}


/**
 * @brief Comprobador recibe bloques por la cola de mensajes de Minero, los comprueba 
 * y manda cada bloque a Monitor hasta que recibe el bloque especial.
 *
 * @param lag int: retraso entre cada ronda.
 * @param shm_struct_comp CircularBuffer: estructura de la memoria compartida.
*/
int comprobador (int lag, CircularBuffer * shm_struct_comp, int fd_shm) {
    int res; 
    mqd_t queue;
    Block myblock;

    /* Definimos tamaño de la memoria compartida */
    if(ftruncate(fd_shm, sizeof(CircularBuffer)) == -1) {
        fprintf(stderr, "ftruncate error\n");
        limpiaSem(shm_struct_comp);
        exit(EXIT_FAILURE);
    }

    /* Mapeamos la memoria compartida para COMPROBADOR*/
    shm_struct_comp = mmap(NULL, sizeof(CircularBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, fd_shm, 0);
    close(fd_shm);
    if(shm_struct_comp == MAP_FAILED) {
        fprintf(stderr, "Error en el mmap\n");
        limpiaSem(shm_struct_comp);
        exit(EXIT_FAILURE);
    }

    /* Inicializamos semaforos de la memoria compartida */
    semaforos(shm_struct_comp);

    /* Inicializamos indice del buffer circular de nuestra estructura.
    *  Utilizamos semaforo para proteger el acceso a esa variable de la memoria compartida.
    */
    sem_wait(&shm_struct_comp->semMutexMemoria);
    shm_struct_comp->idx_last = 0;
    sem_post(&shm_struct_comp->semMutexMemoria);

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
    
    sem_wait(semReady);
    /* Bucle para recibir bloques de mensajes hasta recibir el especial */
    while(1) {

        if(mq_receive(queue, (char *)&myblock, sizeof(Block), NULL) == -1) {
            fprintf(stderr, "Error recceiving message\n");
            limpiaSem(shm_struct_comp);
            return 1;
        }

        /* Si recibimos el bloque especial salimos del bucle y se acaba el programa */
        if(myblock.sol == -1) {
            
            /* --- INICIO ZONA CRITICA --- */
            sem_wait(&shm_struct_comp->semEmptyMemoria);
            sem_wait(&shm_struct_comp->semMutexMemoria);

            /* Mandamos a Monitor el último bloque */
            shm_struct_comp->bloques[shm_struct_comp->idx_last] = myblock;

            sem_post(&shm_struct_comp->semMutexMemoria);
            sem_post(&shm_struct_comp->semFillMemoria);
            /* --- FIN ZONA CRITICA --- */
            
            /* Liberamos recursos y salimos */
            limpiaSem(shm_struct_comp);
            mq_close(queue);
            munmap(shm_struct_comp, sizeof(CircularBuffer));
            return 0;
        }

        /* Comprobamos que la solucion es correcta */
        res = pow_hash(myblock.sol);
        if (myblock.obj == res) {
            /* Si es correcta, modificamos el valor de la estructura para que Monitor lo sepa */
            myblock.accepted = 1;
        }

        /* --- INICIO ZONA CRITICA --- */
        sem_wait(&shm_struct_comp->semEmptyMemoria);
        sem_wait(&shm_struct_comp->semMutexMemoria);

        /* Mandamos bloque comprobado a Monitor */
        shm_struct_comp->bloques[shm_struct_comp->idx_last] = myblock;
        shm_struct_comp->idx_last++;
        shm_struct_comp->idx_last = shm_struct_comp->idx_last%6; // Para que sea buffer circular

        sem_post(&shm_struct_comp->semMutexMemoria);
        sem_post(&shm_struct_comp->semFillMemoria);
        /* --- FIN ZONA CRITICA --- */
        
        /* Realizamos lag de espera */
        usleep(lag*1000);
    }
}

/* --- SEMAFOROS  --- */
void semaforos (CircularBuffer * shm_struct) {

    if (sem_init(&shm_struct->semEmptyMemoria, 1, MEM_BLOCKSIZE) == -1) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&shm_struct->semFillMemoria, 1, 0) == -1) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

    if (sem_init(&shm_struct->semMutexMemoria, 1, 1) == -1) {
        fprintf(stderr, "Error creando semáforo\n");
        exit(EXIT_FAILURE);
    }

}

void limpiaSem(CircularBuffer * shm_struct) {
    sem_close(semReady);
    sem_unlink("semReady");

    sem_destroy(&shm_struct->semMutexMemoria);
    sem_destroy(&shm_struct->semFillMemoria);
    sem_destroy(&shm_struct->semEmptyMemoria);
}