/**
 * @file miner.h
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#ifndef _MINER_H
#define _MINER_H

#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>

#define MQ_NAME "/mq_queue_minercomp"
#define MQ_MAXMSG 7
#define MQ_FLAGS 0
#define MQ_CURMSG 0

extern struct mq_attr attr;

typedef struct {
    int obj;
    int sol;
    int accepted; // 1: accepted | 0: rejected
} Block;

#endif /* MINER H */