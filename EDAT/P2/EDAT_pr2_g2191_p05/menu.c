#include <stdio.h>
#include <stdlib.h>

#include "menu.h"
#include "products.h"
#include "orders.h"
#include "customers.h"
#include "odbc.h"

/*Declarations of functions thar doesn't need to be exported*/

static int ShowMainMenu();

int main() {
    int nChoice = 0;
    SQLHENV env=NULL;
    SQLHDBC dbc=0;
    int ret=0;
    
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
    
    do {
        nChoice = ShowMainMenu();
        switch (nChoice) {
            case 1: {
                if((ShowProductMenu(dbc))==EXIT_FAILURE) return EXIT_FAILURE;
            }
                break;

            case 2: {
                if((ShowOrderMenu(dbc))==EXIT_FAILURE) return EXIT_FAILURE;
            }
                break;

            case 3: {
               if((ShowCustomerMenu(dbc))==EXIT_FAILURE) return EXIT_FAILURE;
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    } while (nChoice != 4);
    
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


/**
 * @brief prints main menu and allows to select an option.
 *
 *
 * @return selected entry in the menu
 * @author Lucía Gil and Paula Beltrán
 */

int ShowMainMenu() {
    int nSelected = 0;
    char buf[16]; 

    do {
        printf("This is a program to access to a data base\nYou can choose to display a number of different menus\n\n");

        printf(" (1) Products\n"
               " (2) Orders\n"
               " (3) Costumers\n"
               " (4) Exit\n\n"
               "Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");

    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}
