#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#include "products.h"

/*Declarations of functions thar doesn't need to be exported*/

static int ShowProductSubMenu();
static int Product_Stock(SQLHDBC dbc);
static int Product_Find(SQLHDBC dbc);

/**
 * @brief Shows and processes the products menu.
 *
 * This function implements a loop that prints the products
 * menu, analyze the user selection and prints
 * the selected choice
 * @return void
 * @author Lucía Gil and Paula Beltrán
 */

int ShowProductMenu(SQLHDBC dbc) {
    int nChoice = 0;
    
    do {
        nChoice = ShowProductSubMenu();
        switch (nChoice) {

            case 1: {
                printf("You are entering to the product's stock menu:\n\n");
                if((Product_Stock(dbc))==EXIT_FAILURE) return EXIT_FAILURE;             
            }
                break;

            case 2: {
                printf("You are entering to the product's find menu:\n\n");
                if((Product_Find(dbc))==EXIT_FAILURE) return EXIT_FAILURE;
            }
                break;

            case 3: {
                printf("You are leaving\n\n");
                return EXIT_SUCCESS;
            }
        }
    } while (nChoice != 4);

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


    } while ((nSelected < 1) || (nSelected > 3));

    return nSelected;
}

/**
 * @brief querie for the STOCK submenu of product.
 *
 * @return number of units of the product in stock
 * @author Lucía Gil and Paula Beltrán
 */

int Product_Stock(SQLHDBC dbc){
    SQLHSTMT stmt = NULL;
    SQLRETURN ret2; /* ODBC API return status */
    SQLINTEGER stock=0;
    char pCode[CharSize] = "\0";

    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select quantityinstock from products where productcode = ?;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    printf("Enter productcode > ");
    (void)fflush(stdout);
    while (scanf("%s", pCode) != EOF) {
        ret2 = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 0, 0, pCode, 0, NULL);
        if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
        }
        
        ret2 = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindCol(stmt, 1, SQL_C_SLONG, &stock, (SQLLEN)sizeof(SQL_INTEGER), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {
            
            printf("Stock: = %d\n", stock);
        }

        ret2 = SQLCloseCursor(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
        }
        (void)fflush(stdout);
        break;

    }
    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }
 
    return EXIT_SUCCESS;
}

/**
 * @brief querie for the FIND submenu of product.
 *
 * @return a list with the productcode and productname of the products 
 * with the same name as the string entered
 * 
 * @author Lucía Gil and Paula Beltrán
 */
int Product_Find(SQLHDBC dbc) {
    SQLHSTMT stmt = NULL;
    SQLRETURN ret2; /* ODBC API return status */
    char pCode[CharSize]="\0";
    char pName[CharSize]="\0";
    char pNamCom[CharSize]="\0";

    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select productcode, productname from products where productname like '%%'||?||'%%' order by productcode;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    printf("Enter productname > ");
    (void)fflush(stdout);

    while (scanf("%s", pName) != EOF) {
        ret2=SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_INTEGER, 0, 0, pName, 0, NULL);
        if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
        }
        
        ret2 = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
        }
        
        ret2 = SQLBindCol(stmt, 1, SQL_C_CHAR, pCode, (SQLLEN)sizeof(pCode), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
        }
        
        ret2 = SQLBindCol(stmt, 2, SQL_C_CHAR, pNamCom, (SQLLEN)sizeof(pNamCom), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
        }
        
        printf("ProductCode | ProductName\n");
        /* Loop through the rows in the result-set */
        while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {
            
            printf("%11s | %s \n", pCode, pNamCom);

        }

        ret2 = SQLCloseCursor(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
        }

        (void)fflush(stdout);
        break;
    }
    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_STMT);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}