#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#include "orders.h"


/**
 * @brief Shows and processes the order's menu.
 *
 * This function implements a loop that prints the order's
 * menu, analyze the user selection and prints
 * the selected choice
 * @return EXIT_FAILURE in case of failure and EXIT_SUCCESS in case of success
 * @author Lucía Gil and Paula Beltrán
 */

int ShowOrderMenu() {
    int nChoice = 0;
    

    do {
        nChoice = ShowOrderSubMenu();
        switch (nChoice) {

            case 1: {
                printf("You are entering to the order's open menu:\n\n");
                if((Order_Open())==EXIT_FAILURE) return EXIT_FAILURE;             
            }
                break;

            case 2: {
                printf("You are entering to the order's range menu:\n\n");
                if((Order_Range())==EXIT_FAILURE) return EXIT_FAILURE; 
            }
                break;

            case 3: {
                printf("You are entering to the order's detail menu:\n\n");
                if((Order_Detail())==EXIT_FAILURE) return EXIT_FAILURE;
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
 * @brief prints order menu and allows to select an option.
 *
 * @return selected option
 * @author Lucía Gil and Paula Beltrán
 */

 int ShowOrderSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Open\n"
               " (2) Range\n"
               " (3) Detail\n"
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

/**
 * @brief querie for the find submenu of order.
 *
 * @return number of units of the product in stock
 * @author Lucía Gil and Paula Beltrán
 */

int Order_Open(){
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLCHAR ordernumber[512];
    SQLDATE *shippeddate=NULL;

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*) "select ordernumber from orders where shippeddate is null order by ordernumber;", SQL_NTS);

    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_DATE, SQL_DATE, 0, 0, shippeddate, 0, NULL);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }
        
    SQLExecute(stmt);
        
    SQLBindCol(stmt, 1, SQL_C_CHAR, ordernumber, sizeof(SQL_CHAR), NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            
            printf("OrderNumbers: = %s\n", ordernumber);
        }

        SQLCloseCursor(stmt);
    
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

int Order_Range(){
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLCHAR ordernumber[512];
    SQLDATE orderdate;
    SQLDATE shippeddate;
    SQLDATE date1;
    SQLDATE date2;

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*) "select ordernumber, orderdate, shippeddate from orders where orderdate >= ? and orderdate < ? order by ordernumber;", SQL_NTS);

    printf("Enter dates (YYYY-MM-DD - YYYY-MM-DD) > ");
    fflush(stdout);

    while (scanf("%s", pCode) != EOF) {
        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, date1, 0, NULL);
        if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
        }
        
        SQLExecute(stmt);
        
        SQLBindCol(stmt, 1, SQL_C_SLONG, &stock, sizeof(SQL_INTEGER), NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            
            printf("Stock: = %d\n", stock);
        }

        SQLCloseCursor(stmt);

        break;

        printf("If you want to stop searching press enter\n Otherwise insert an other product code:\n");
        printf("ProductCode = ");
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
int Order_Detail(){

}

