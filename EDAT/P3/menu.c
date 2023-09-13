#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


/**
 * @brief Declarations of functions that doesn't need to be exported
 */
static int ShowMainMenu();
bool ShowUseMenu(char *tableName, char *indexName);
bool ShowInsertMenu(char *tableName);
bool ShowPrintMenu(char *tableName, char *indexName);

/**
 * @brief Shows and processes the main menu.
 *
 * This function implements a loop that prints the
 * menu we select, analyzes the user selection and prints
 * the selected choice.
 * 
 * Here is where we are going to make the connection and
 * the disconnection to the database.
 * 
 * @return EXIT_SUCCESS if everything goes ok
 * @return EXIT_FAILURE if there is any problem
 * @author Lucía Gil and Paula Beltrán
 */
int main() {
    char tableName[MAX_TABLE_NAME], indexName[MAX_TABLE_NAME];
    int nChoice = 0;
    
    while (nChoice != 4) {
        nChoice = ShowMainMenu();
        switch (nChoice) {
            case 1: {
                if((ShowUseMenu(tableName, indexName))==false) return EXIT_FAILURE;
            }
                break;

            case 2: {
                if((ShowInsertMenu(tableName))==false) return EXIT_FAILURE;
            }
                break;

            case 3: {
               if((ShowPrintMenu(tableName, indexName))==false) return EXIT_FAILURE;
            }
                break;

            case 4: {
                printf("Bye Bye\n\n");
            }
                break;
        }
    }
    
    
    return EXIT_SUCCESS;
}

/**
 * @brief prints main menu and allows to select an option.
 * 
 * This function prints with a loop the main menu, allows
 * the user to select an option and process and scan the option
 * 
 * @return selected option
 * @author Lucía Gil and Paula Beltrán
 */
int ShowMainMenu() {
    int nSelected = 0;
    char buf[16]; 

    while ((nSelected < 1) || (nSelected > 4)) {
        printf("This is a program to access to different funcionalities of data and index files\nYou can choose to display a number of different menus\n\n");

        printf(" (1) Use\n"
               " (2) Insert\n"
               " (3) Print\n"
               " (4) Exit\n\n"
               "Enter your choice > ");
        if(!(fscanf(stdin, "%s", buf)))
            /* reading input failed, give up: */
            nSelected =0;
        else{
            if((strcmp(buf, "Use"))==0){
                nSelected = 1;
            }
            else if((strcmp(buf, "Insert"))==0){
                nSelected = 2;
            }
            else if((strcmp(buf, "Print"))==0){
                nSelected = 3;
            }
            else if((strcmp(buf, "Exit"))==0){
                nSelected = 4;
            }
        }
        printf("\n");

    } 

    return nSelected;
}

/**
 * @brief Shows and processes the use menu.
 *
 * This function reads the table's name and calls createTable function
 * 
 * @return true if everything goes ok
 * @return false if there is any problem
 * @author Lucía Gil and Paula Beltrán
 */

bool ShowUseMenu(char *tableName, char *indexName) {
    bool res=true;

    printf("Enter the name of the table > ");
    scanf("%s", tableName);
    printf("\n");
    printf("You are creating the table with the name you have chosen:\n\n");
    res = createTable(tableName);

    replaceExtensionByIdx(tableName, indexName);
      
        
    return res;
}

/**
 * @brief Shows and processes the insert menu.
 *
 * This function implements addTableEntry function
 * 
 * @return true if everything goes ok
 * @return false if there is any problem
 * @author Lucía Gil and Paula Beltrán
 */

bool ShowInsertMenu(char *tableName){
    FILE *table=NULL;
    Book *book = NULL;
    char key[PK_SIZE];
    char title[MAX_TABLE_NAME];
    char indexname[sizeof(tableName)];
    bool res = true;
    int size = 0;

    printf("Insert the name of the file where you want to write > ");
    scanf("%s", tableName);
    table = fopen(tableName, "r");
    if(table == NULL){
        printf("\nIndex haven't been selected before, use 'use' option\n\n");
        return true;
    }

    book = (Book*)malloc(sizeof(Book));
    if(book == NULL) {
        return false;
    }

    printf("Write the book's key > ");
    scanf("%s", key);
    memcpy(book->book_id, key, 4);

    printf("\n");
    printf("Write book's title > ");
    getchar();
    scanf("%[^\n]s", title);

    size = strlen(title);
    if(title[size - 1] == '\n') {
        title[size - 1] = 0;
    }

    book->title = (char*)malloc((strlen(title)+1) * sizeof(char));
    if((book->title)==NULL){
        free(book);
        return false;
    }

    strcpy(book->title, title);

    book->title_len = strlen(book->title);
    
    replaceExtensionByIdx(tableName, indexname);
    res = addTableEntry(book, tableName, indexname);

    strcpy(title, "\0");
    free(book->title);
    free(book);

    fclose(table);
    return res;


}

/**
 * @brief Shows and processes the print menu.
 *
 * This function calls printTree function to print the archive.
 * 
 * @return true if everything goes ok
 * @return false if there is any problem
 * @author Lucía Gil and Paula Beltrán
 */

bool ShowPrintMenu(char *tableName, char *indexName) {
    int level=-1;
    FILE *index=NULL;

    printf("Insert the name of the index file to print > ");
    scanf("%s", indexName);
    index = fopen(indexName, "r");
    if(index == NULL){
        printf("\nIndex haven't been selected before, use 'use' option\n\n");
        return true;
    }
    fclose(index);
    printf("Insert the level until you want to print the index file > ");
    scanf("%d", &level);

    printf("\nInsert the name of the table file to print > ");
    scanf("%s", tableName);

    index = fopen(tableName, "r");
    if(index == NULL){
        printf("\nIndex haven't been selected before, use 'use' option\n\n");
        return true;
    }
    fclose(index);
    printTree((size_t)level, indexName);

    printTable(tableName);
    
    return true;
}

