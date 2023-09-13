#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sql.h>
#include <sqlext.h>
#include "odbc.h"

#include "customers.h"

/*Declarations of functions thar doesn't need to be exported*/

static int ShowCustomerSubMenu();
static int Customer_Find(SQLHDBC dbc);
static int Customer_ListProducts(SQLHDBC dbc);
static int Customer_Balance(SQLHDBC dbc);

/**
 * @brief Shows and processes the customers menu.
 *
 * This function implements a loop that prints the customers
 * menu, analyze the user selection and prints
 * the selected choice
 * @return void
 * @author Lucía Gil and Paula Beltrán
 */

int ShowCustomerMenu(SQLHDBC dbc) {
    int nChoice = 0;

    do {
        nChoice = ShowCustomerSubMenu();
        switch (nChoice) {

            case 1: {
                printf("You are entering to the customer's find menu:\n\n");
                if((Customer_Find(dbc))==EXIT_FAILURE) return EXIT_FAILURE;             
            }
                break;

            case 2: {
                printf("You are entering to the customer's list products menu:\n\n");
                if((Customer_ListProducts(dbc))==EXIT_FAILURE) return EXIT_FAILURE;  
            }
                break;

            case 3: {
                printf("You are entering to the customers's balance menu:\n\n");
                if((Customer_Balance(dbc))==EXIT_FAILURE) return EXIT_FAILURE;
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

int Customer_Find(SQLHDBC dbc){
    SQLHSTMT stmt=NULL;
    SQLRETURN ret2; /* ODBC API return status */
    char cContact[CharSize]="\0";
    char cCliName[CharSize]="\0";
    char cConName[CharSize]="\0";
    char cLastName[CharSize]="\0";
    char cIdent[CharSize]="\0";
    SQLINTEGER page = 0;
    char nextp[CharSize]="\0";
    char a[5]="\0";
    char b[5]="\0";
    int flag=1, cont=0;

    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        odbc_extract_error("", stmt, SQL_HANDLE_ENV);
        return EXIT_FAILURE;
    }
    
    printf("Enter customer name > ");
    (void)fflush(stdout);
    (void)scanf("%s", cContact);
    while (flag==1){
        cont = 0;

        ret2 = SQLPrepare(stmt, (SQLCHAR*) "select customername, contactfirstname, contactlastname, customernumber from customers where (contactfirstname like '%%'||?||'%%') or (contactlastname like '%%'||?||'%%') order by customernumber OFFSET ? LIMIT 10;", SQL_NTS);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, cContact, 0, NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, cContact, 0, NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &page, 0, NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindCol(stmt, 1, SQL_C_CHAR, cCliName, (SQLLEN)sizeof(cCliName), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindCol(stmt, 2, SQL_C_CHAR, cConName, (SQLLEN)sizeof(cConName), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindCol(stmt, 3, SQL_C_CHAR, cLastName, (SQLLEN)sizeof(cLastName), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindCol(stmt, 4, SQL_C_CHAR, cIdent, (SQLLEN)sizeof(cIdent), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))){
            cont++;
            printf("%s %s %s %s\n", cIdent, cCliName, cConName, cLastName);
        }
        
        ret2 = SQLCloseCursor(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        printf("\n");

        if(cont<10){
            flag = 0;
        }

        printf("Next page? (< , >)\n");
        (void)fflush(stdout);
        (void)fscanf(stdin, "%s", nextp);

            a[0]='>';
            b[0]='<';
        if(strcmp(nextp, a)==0){
            page += 10;
        }
        else if(strcmp(nextp, b)==0){
            if(page>0){
                page -= 10;
                flag = 1;
            }
            
        }
        else flag = 0;
    }


    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
     }

    return EXIT_SUCCESS;
}

int Customer_ListProducts(SQLHDBC dbc){
    SQLHSTMT stmt=NULL;
    SQLRETURN ret2; /* ODBC API return status */
    SQLINTEGER n=0;
    SQLINTEGER page=0;
    char cIdent[CharSize]="\0";
    char productname[CharSize]="\0";
    char nextp[CharSize]="\0";
    char a[5]="\0";
    char b[5]="\0";
    int flag = 1;
    int cont = 0;

    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
    }

    printf("Enter customer number > ");
    (void)fflush(stdout);
    (void)scanf("%s", cIdent);

    while(flag == 1){
        cont =0;

        ret2 = SQLPrepare(stmt, (SQLCHAR*) "select p.productname, sum(od.quantityordered ) as n from products p join orderdetails od on p.productcode = od.productcode join orders o on od.ordernumber = o.ordernumber join customers c on o.customernumber = c.customernumber where c.customernumber = ? group by p.productcode order by p.productcode asc OFFSET ? LIMIT 10;", SQL_NTS);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, cIdent, 0, NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &page, 0, NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindCol(stmt, 1, SQL_C_CHAR, productname, (SQLLEN)sizeof(productname), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        ret2 = SQLBindCol(stmt, 2, SQL_C_LONG, &n, (SQLLEN)sizeof(n), NULL);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        while (SQL_SUCCEEDED(ret2 = SQLFetch(stmt))){
            cont++;
            printf("%s %d\n", productname, n);
        }
        
        ret2 = SQLCloseCursor(stmt);
        if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
        }

        printf("\n");

        if(cont<10){
            flag = 0;
        }

        printf("Next page? (< , >)\n");
        (void)fflush(stdout);
        (void)fscanf(stdin, "%s", nextp);

            a[0]='>';
            b[0]='<';
        if(strcmp(nextp, a)==0){
            page += 10;
        }
        else if(strcmp(nextp, b)==0){
            if(page>0){
                page -= 10;
                flag = 1;
            }
            
        }
        else flag = 0;
    }

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int Customer_Balance(SQLHDBC dbc){
    SQLHSTMT stmt=NULL;
    SQLRETURN ret2; /* ODBC API return status */
    char cIdent[CharSize]="\0";
    char balance[CharSize]="\0";

    /* Allocate a statement handle */
    ret2 = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
    }

    ret2 = SQLPrepare(stmt, (SQLCHAR*) "select a.paid-b.debit as balance from (select sum(p.amount) paid from payments p where p.customernumber = ?) a natural join (select sum(od.quantityordered*od.priceeach) debit from orders o join orderdetails od on od.ordernumber=o.ordernumber where o.customernumber=?) b;", SQL_NTS);
    if (!SQL_SUCCEEDED(ret2)) {
            odbc_extract_error("", stmt, SQL_HANDLE_ENV);
            return EXIT_FAILURE;
    }

    printf("Enter customer number > ");
    (void)fflush(stdout);
    (void)scanf("%s", cIdent);

    ret2 = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, cIdent, 0, NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        return EXIT_FAILURE;
    }

    ret2 = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0, cIdent, 0, NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        return EXIT_FAILURE;
    }

    ret2 = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        return EXIT_FAILURE;
    }

    ret2 = SQLBindCol(stmt, 1, SQL_C_CHAR, balance, (SQLLEN)sizeof(balance), NULL);
    if (!SQL_SUCCEEDED(ret2)) {
        return EXIT_FAILURE;
    }

    while(SQL_SUCCEEDED(ret2 = SQLFetch(stmt))){
        printf("Balance = %s\n", balance);
    }

    ret2 = SQLCloseCursor(stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        return EXIT_FAILURE;
    }

    printf("\n");

    /* free up statement handle */
    ret2 = SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    if (!SQL_SUCCEEDED(ret2)) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}