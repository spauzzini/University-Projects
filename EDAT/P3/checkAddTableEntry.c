#include "tester.h"

void checkAddTableEntry(const char * tableName, const char *indexName){
    Book *book=NULL;
    bool result;
    int nodeIDOrDataOffset;

    book = (Book*)malloc(sizeof(Book));
    if(book == NULL) {
        exit(EXIT_FAILURE);
    } 

    printf("-----Original table-------\n");
    printTable(tableName);
    strcpy(book->book_id, "MAR2");
    book->title_len = 10;
    book->title = (char *)malloc(10*sizeof(char));
    strcpy(book->title, "MAR2_zero");
    /* 1) add existing node should complain*/
    result = addTableEntry(book, tableName, indexName);
    if (result==true){
        fprintf(stderr,
                "Error in addTableEntry, inserted existing key %s",
                book->book_id);
                free(book->title);
                free(book);
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("The existing book has not been inserted. Good work :)\n");
    }
    free(book->title);
    
    /*deleted records and a title of the same size of one existed*/
    strcpy(book->book_id, "LOL2");
    book->title_len = 5;
    book->title = (char *)malloc(5*sizeof(char));
    strcpy(book->title, "Hola");
    result = addTableEntry(book, tableName, indexName);
    if (result == false){
        fprintf(stderr,
                "Error in addTableEntry while adding element %s at the end of the file.\n",
                book->book_id);
                free(book->title);
                free(book);
        exit(EXIT_FAILURE);
    }

    if((findKey(book->book_id, indexName, &nodeIDOrDataOffset))==false){
       fprintf(stderr,
                "Error in addTableEntry while adding element %s at the end of the file.\n",
                book->book_id);
                free(book->title);
                free(book);
        exit(EXIT_FAILURE);
    }

    printf("-----after adding LOL2-------\n");
    printTable(tableName);
    free(book->title);

    /*deleted records and a title shorter than one existed*/
    strcpy(book->book_id, "LOL3");
    book->title_len = 3;
    book->title = (char *)malloc(3*sizeof(char));
    strcpy(book->title, "Hi");
    result = addTableEntry(book, tableName, indexName);
    if (result == false){
        fprintf(stderr,
                "Error in addTableEntry while adding element %s at the end of the file.\n",
                book->book_id);
                free(book->title);
                free(book);
        exit(EXIT_FAILURE);
    }

    if((findKey(book->book_id, indexName, &nodeIDOrDataOffset))==false){
       fprintf(stderr,
                "Error in addTableEntry while adding element %s at the end of the file.\n",
                book->book_id);
                free(book->title);
                free(book);
        exit(EXIT_FAILURE);
    }
    printf("-----after adding LOL3-------\n");
    printTable(tableName);
    
    free(book->title);

    /*No deleted records*/
    strcpy(book->book_id, "LOL4");
    book->title_len = 6;
    book->title = (char *)malloc(6*sizeof(char));
    strcpy(book->title, "Hola5");
    result = addTableEntry(book, tableName, indexName);
    if (result == false){
        fprintf(stderr,
                "Error in addTableEntry while adding element %s at the end of the file.\n",
                book->book_id);
                free(book->title);
                free(book);
        exit(EXIT_FAILURE);
    }

    if((findKey(book->book_id, indexName, &nodeIDOrDataOffset))==false){
       fprintf(stderr,
                "Error in addTableEntry while adding element %s at the end of the file.\n",
                book->book_id);
                free(book->title);
                free(book);
        exit(EXIT_FAILURE);
    }    
    printf("-----after adding LOL4-------\n");
    printTable(tableName);
    

    free(book->title);
    free(book);
    fprintf(stdout, "* checkAddTableEntry: OK\n");
    exit(EXIT_SUCCESS);
}