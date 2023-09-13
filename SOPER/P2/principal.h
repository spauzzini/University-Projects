/**
 * @file principal.h
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */
#define _XOPEN_SOURCE 700//mi pc lo necesita
#define MAX_VOT 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "votante.h"

#ifndef _PRINCIPAL_H
#define _PRINCIPAL_H


/* START [_ProcessInfo] */
typedef struct _ProcessInfo ProcessInfo;
/* END [_ProcessInfo] */

#endif /* PRINCIPAL H */