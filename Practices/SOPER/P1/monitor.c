/**
 * @file monitor.c
 * @author Elena Balseiro y Paula Beltrán
 * @date 2023-03-02
 * 
 * @version 1
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "monitor.h"
#include "minero.h"
#include "pow.h"

int monitor(int mineroSol, int target){
    long res;

    /*Cuando se envía un -1 significa que se han acabado las rondas*/
    if (mineroSol == -1) {
        return 0;
    }
    
    /* Calculamos el resultado usando la solución propuesta por el minero */
    res = pow_hash(mineroSol);

    /* Si el resultado coincide con la target, se acepta la solución */
    if(target == res){
        printf("Solution accepted: %d --> %d\n", target, mineroSol);
        return 0;
    }
    /* Si el resultado no coincide con la target, se rechaza la solución */
    printf("Solution rejected: %d !-> %d\n", target, mineroSol);
    return 1;
}