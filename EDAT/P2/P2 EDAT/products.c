#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#include "products.h"


/**
 * @brief Shows and processes the products menu.
 *
 * This function implements a loop that prints the products
 * menu, analyze the user selection and prints
 * the selected choice
 * @return void
 * @author Lucía Gil and Paula Beltrán
 */

int ShowProductMenu() {
    int nChoice = 0;
    

    do {
        nChoice = ShowProductSubMenu();
        switch (nChoice) {

            case 1: {
                printf("You are entering to the product's stock menu:\n\n");
                if((Product_Stock())==EXIT_FAILURE) return EXIT_FAILURE;             
            }
                break;

            case 2: {
                /*Product_Find();*/
                printf("You are entering to the product's find menu:\n\n");
                if((Product_Find())==EXIT_FAILURE) return EXIT_FAILURE;  
            }
                break;

            case 3: {
                printf("You are leaving\n\n");
            }
                break;
        }
    } while (nChoice != 3);

    return EXIT_SUCCESS;
}

/**
 * @brief prints product menu and allows to select an option.
 *
 * @return selected option
 * @author Lucía Gil and Paula Beltrán
 */

 int ShowProductSubMenu() {
    int nSelected = 0;
    char buf[16];

    do {

        printf(" (1) Stock\n"
               " (2) Find\n"
               " (3) Back\n\n");

        printf("Enter a number that corresponds to your choice > ");
        if (!fgets(buf, 16, stdin))
            /* reading input failed, give up: */
            nSelected =0;
        else
            /* have some input, convert it to integer: */
            nSelected = atoi(buf);
        printf("\n");


        if ((nSelected < 1) || (nSelected > 3)) {
            printf("You have entered an invalid choice. Please try again\n\n\n");
        }
    } while ((nSelected < 1) || (nSelected > 3));

    return nSelected;
}

/**
 * @brief querie for the stock submenu of product.
 *
 * @return number of units of the product in stock
 * @author Lucía Gil and Paula Beltrán
 */

int Product_Stock(){
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLINTEGER stock;
    SQLCHAR pCode[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*) "select quantityinstock from products where productcode = ?;", SQL_NTS);

    printf("Insert the product code you want to find:\n ");
    fflush(stdout);
    while (scanf("%s", pCode) != EOF) {
        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, pCode, 0, NULL);
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

int Product_Find() {
    SQLHENV env;
    SQLHDBC dbc;
    SQLHSTMT stmt;
    SQLRETURN ret; /* ODBC API return status */
    SQLCHAR pCode[512];
    SQLCHAR pName[512];
    SQLCHAR pNamCom[512];

    /* CONNECT */
    ret = odbc_connect(&env, &dbc);
    if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
    }

    /* Allocate a statement handle */
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepare(stmt, (SQLCHAR*) "select productcode, productname from products where productname = '%'||?||'%' order by productcode ;", SQL_NTS);

    printf("Insert the product name you want to find:\n ");
    fflush(stdout);
    while (scanf("%s", pName) != EOF) {
        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, pName, 0, NULL);
        if (!SQL_SUCCEEDED(ret)) {
        return EXIT_FAILURE;
        }
        
        SQLExecute(stmt);
        
        SQLBindCol(stmt, 1, SQL_C_SLONG, pNamCom, sizeof(SQL_CHAR), NULL);
        SQLBindCol(stmt, 1, SQL_C_SLONG, pCode, sizeof(SQL_CHAR), NULL);

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret = SQLFetch(stmt))) {
            
            printf("Product Code: = %s\n", pCode);
            printf("Product Name: = %s\n", pNamCom);
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

