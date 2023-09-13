/**
 * @file miner.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include "miner.h"
#include "pow.h"

struct mq_attr attr;

int main (int argc, char *argv[]) {
    int blocks, lag, target=0;
    mqd_t queue;
    Block myblock;

    /* Comprobamos si el número de argumentos es correcto */
    if (argc != 3){
        fprintf(stderr, "Usage: %s <ROUNDS> <LAG> \n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    blocks = atoi(argv[1]);
    if(blocks < 1 ) {
        fprintf(stderr, "Número de bloques inválido\n");
        exit(EXIT_FAILURE);
    }
    
    lag = atoi(argv[2]);
    if (lag < 0 ) {
        fprintf(stderr, "Lag no válido\n");
        exit(EXIT_FAILURE);
    }

    /* Inicializacion estructura de cola de mensajes */
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

    /* Generamos argv[1] bloques */
    printf("[%d] Generating blocks...\n", getpid());
    while(blocks>0) {
        for (int i=0; i<PRIME; i++) {
            if (target == pow_hash(i)) {
                myblock.sol = i;
                myblock.obj = target;
                myblock.accepted = 0;
                break;
            }
        }

        /* Enviamos en la cola el bloque */
        if(mq_send(queue, (char*)&myblock, sizeof(Block), 1)) {
            fprintf(stderr, "Error sending message\n");
            exit(EXIT_FAILURE);
        }
        
        /* Realizamos lag */
        usleep(lag*1000);

        /* Actualizamos target */
        target = myblock.sol;
        blocks--;
    }

    /* Creamos bloque final y enviamos */
    myblock.sol = -1;
    myblock.obj = -1;
    myblock.accepted = -1;
    if(mq_send(queue, (char*)&myblock, sizeof(Block),1)) {
        fprintf(stderr, "Error sending message\n");
   
        exit(EXIT_FAILURE);
    }

    printf("[%d] Finishing\n", getpid());
    /* Liberacion de recursos */
    mq_close(queue);
    mq_unlink(MQ_NAME);
    exit(EXIT_SUCCESS);
}

