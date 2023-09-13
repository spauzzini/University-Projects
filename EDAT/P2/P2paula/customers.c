#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#include "customers.h"


/**
 * @brief Shows and processes the customers menu.
 *
 * This function implements a loop that prints the customers
 * menu, analyze the user selection and prints
 * the selected choice
 * @return void
 * @author Lucía Gil and Paula Beltrán
 */

int ShowCustomerMenu() {
    int nChoice = 0;

    do {
        nChoice = ShowCustomerSubMenu();
        switch (nChoice) {

            case 1: {
                printf("You are entering to the customer's find menu:\n\n");
                if((Customer_Find())==EXIT_FAILURE) return EXIT_FAILURE;             
            }
                break;

            case 2: {
                printf("You are entering to the customer's list products menu:\n\n");
                if((Customer_ListProducts())==EXIT_FAILURE) return EXIT_FAILURE;  
            }
                break;

            case 3: {
                printf("You are entering to the customers's balance menu:\n\n");
                if((Customer_Balance())==EXIT_FAILURE) return EXIT_FAILURE;
            }
                break;

            case 4: {
                printf("You are leaving\n\n");
            }
                break;
        }
    } while (nChoice != 4);

    return EXIT_SUCCESS;
}

/**
 * @brief prints customers menu and allows to select an option.
 *
 * @return selected option
 * @author Lucía Gil and Paula Beltrán
 */

 int ShowCustomerSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Find\n"
               " (2) List Products\n"
               " (3) Balance\n"
               " (4) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");


        if ((nSelected < 1) || (nSelected > 4)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 4));

    return nSelected;
}

int Customer_Find(){
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLCHAR cContact[512];
    SQLCHAR cCliName[512];
    SQLCHAR cConName[512];
    SQLCHAR cLastName[512];
    SQLCHAR cIdent[512];


    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    /*ver si la query sta bn hecha xd*/
    SQLPrepare(stmt, (SQLCHAR*) "select customername, contactfirstname, contactlastname, customernumber from customers where contactfirstname or contactlastname like '%%'||?||'%%' order by customernumber;", SQL_NTS);

    printf("Insert the name or surname of the client you want to find:\n ");
    fflush(stdout);
    while (scanf("%s", cContact) != EOF) {
        /*ver si el ret este de kk esta bien pq como te piden el contact1stname O el lastname ???*/
        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, cContact, 0, NULL);
        if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
        }
        
        SQLExecute(stmt);
        
        SQLBindCol(stmt, 1, SQL_C_SLONG, cCliName, sizeof(cCliName), NULL);
        SQLBindCol(stmt, 2, SQL_C_SLONG, cConName, sizeof(cConName), NULL);
        SQLBindCol(stmt, 3, SQL_C_SLONG, cLastName, sizeof(cLastName), NULL);
        SQLBindCol(stmt, 4, SQL_C_SLONG, cIdent, sizeof(cIdent), NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            
            printf("Client Name: = %s\n", cCliName);
            printf("Contact Name: = %s\n", cConName);
            printf("Last Name: = %s\n", cLastName);
            printf("Client Identifier: = %s\n", cIdent);
        }

        SQLCloseCursor(stmt);

        fflush(stdout);
    }
    printf("\n");

    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Customer_ListProducts(){
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*) "", SQL_NTS);

    
    printf("\n");

    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Customer_Balance(){
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*) "", SQL_NTS);

    
    printf("\n");

    /* free up statement handle */
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    /* DISCONNECT */
    ret = odbc_disconnect(env, dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
