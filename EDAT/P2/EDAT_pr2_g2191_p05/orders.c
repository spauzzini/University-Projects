#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#include "orders.h"

/*Declarations of functions thar doesn't need to be exported*/

static int ShowOrderSubMenu();
static int Order_Open(SQLHDBC dbc);
static int Order_Range(SQLHDBC dbc);
static int Order_Detail(SQLHDBC dbc);


/**
 * @brief Shows and processes the order's menu.
 *
 * This function implements a loop that prints the order's
 * menu, analyze the user selection and prints
 * the selected choice
 * 
 * @return EXIT_FAILURE in case of failure and EXIT_SUCCESS in case of success
 * @author Lucía Gil and Paula Beltrán
 */

int ShowOrderMenu(SQLHDBC dbc) {
    int nChoice = 0;
    

    do {
        nChoice = ShowOrderSubMenu();
        switch (nChoice) {

            case 1: {
                printf("You are entering to the order's open menu:\n\n");
                if((Order_Open(dbc))==EXIT_FAILURE) return EXIT_FAILURE;             
            }
                break;

            case 2: {
                printf("You are entering to the order's range menu:\n\n");
                if((Order_Range(dbc))==EXIT_FAILURE) return EXIT_FAILURE; 
            }
                break;

            case 3: {
                printf("You are entering to the order's detail menu:\n\n");
                if((Order_Detail(dbc))==EXIT_FAILURE) return EXIT_FAILURE;
            }
                break;

            case 4: {
                printf("You are leaving\n\n");
                return EXIT_SUCCESS;
            }
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
 * @brief querie for the OPEN submenu of order.
 *
 * @return 
 * @author Lucía Gil and Paula Beltrán
 */
int Order_Open(SQLHDBC dbc){
    SQLHSTMT stmt=NULL;
    SQLRETURN ret2; /* ODBC API return status */
    SQLINTEGER ordernumber=0;

    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select ordernumber from orders where shippeddate is NULL order by ordernumber asc;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }   

    ret2 = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 1, SQL_C_LONG, &ordernumber, (SQLLEN)sizeof(ordernumber), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }
        

        /* Loop through the rows in the result-set */
    while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {

        printf("%d\n", ordernumber);

    }
    ret2 = SQLCloseCursor(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }
    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }


    return EXIT_SUCCESS;
}

/**
 * @brief querie for the RANGE submenu of order.
 *
 * @return 
 * @author Lucía Gil and Paula Beltrán
 */

int Order_Range(SQLHDBC dbc){
    SQLHSTMT stmt=NULL;
    SQLRETURN ret2; /* ODBC API return status */
    SQLINTEGER ordernumber=0;
    char orderdate[CharSize]="\0";
    char shippeddate[CharSize]="\0";
    char alm[50];
    char *date1=NULL;
    char *date2=NULL;

    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select ordernumber, orderdate, shippeddate from orders where orderdate >= ? and orderdate <= ? order by ordernumber;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    printf("Enter dates (YYYY-MM-DD - YYYY-MM-DD) > ");
    (void)fflush(stdout);

    if(!fgets(alm, 50, stdin)){
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    date1 = strtok(alm, " ");
    if(!date1){
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }
    (void)strtok(NULL, " ");
    date2 = strtok(NULL, "\n");
    if(!date2){
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, date1, 0, NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }
    ret2 = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, date2, 0, NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 1, SQL_C_LONG, &ordernumber, (SQLLEN)sizeof(ordernumber), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 2, SQL_C_CHAR, orderdate, (SQLLEN)sizeof(orderdate), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 3, SQL_C_CHAR, shippeddate, (SQLLEN)sizeof(shippeddate), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {

        printf("%d %s %s\n", ordernumber, orderdate, shippeddate);

    }
    ret2 = SQLCloseCursor(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }
    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/**
 * @brief querie for the DETAIL submenu of order.
 *
 * @return 
 * @author Lucía Gil and Paula Beltrán
 */
int Order_Detail(SQLHDBC dbc){
    SQLHSTMT stmt=NULL;
    SQLRETURN ret2; /* ODBC API return status */
    char on[CharSize];
    char orderdate[CharSize]="\0";
    char status[CharSize]="\0";
    char productcode[CharSize]="\0";
    SQLINTEGER ordernumber=0;
    SQLINTEGER quantityordered=0;
    SQLDOUBLE expense=0;
    SQLDOUBLE priceeach=0;

    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select o.ordernumber, o.orderdate, o.status, sum(od.priceeach*od.quantityordered) as expense from orders o join orderdetails od on o.ordernumber = od.ordernumber where o.ordernumber = ? group by o.ordernumber;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    printf("Enter ordernumber > ");
    (void)fflush(stdout);
    (void)scanf("%s", on);

    ret2 = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, on, 0, NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 1, SQL_C_LONG, &ordernumber, (SQLLEN)sizeof(ordernumber), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 2, SQL_C_CHAR, orderdate, (SQLLEN)sizeof(orderdate), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 3, SQL_C_CHAR, status, (SQLLEN)sizeof(status), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 4, SQL_C_DOUBLE, &expense, (SQLLEN)sizeof(expense), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {

        printf("Order Date=%s - status=%s\n", orderdate, status);
	printf("Total sum = %.2f\n", expense);

    }
    
    ret2 = SQLCloseCursor(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    printf("\n");

    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select od.productcode, od.quantityordered, od.priceeach from orderdetails od where od.ordernumber = ? order by (od.orderlinenumber);", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }


    ret2 = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, on, 0, NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 1, SQL_C_CHAR, productcode, (SQLLEN)sizeof(productcode), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 2, SQL_C_LONG, &quantityordered, (SQLLEN)sizeof(quantityordered), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 3, SQL_C_DOUBLE, &priceeach, (SQLLEN)sizeof(priceeach), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))) {

        printf("%s %d %.2f\n", productcode, quantityordered, priceeach);

    }
    ret2 = SQLCloseCursor(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}
