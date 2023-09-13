/*
* Authors: Lucía Gil and Paula Beltrán
* File: utils.c
* It contains the function to implement in practice 3
* This is the partial delivery for practice 3
* Date: 06/12/2020
* 
*/

#include "utils.h"

int no_deleted_registers = NO_DELETED_REGISTERS;

/**
 * @brief function that changes the extension of an archive to .idx
 * @authors Lucía Gil and Paula Beltrán
 * 
 * @param fileName: original name to change the extension.
 * @param indexName: char to store the name with the extension changed.
 * 
 * @return it is a void function, it does not return anything.
*/
void replaceExtensionByIdx(const char *fileName, char *indexName){
    int size=0;

    if (fileName == NULL || indexName == NULL){
        return;
    }

    while (fileName[size] != '.'){
        indexName[size] = fileName[size];
        size++;
    }
    
    indexName[size] = '\0';
    indexName[size + 4] = '\0';
    strcat(indexName, ".idx");
    return;
}

/**
 * @brief function that create a table file.
 * @authors Lucía Gil and Paula Beltrán
 * 
 * @param tableName: name for the file.
 * 
 * @return true if everything went ok and false if anything went wrong.
*/
bool createTable(const char *tableName){
    FILE *arch=NULL;
    char *indxname;
    bool res=false;

    if(!tableName){
        return false;
    }

    arch = fopen (tableName, "r");
    if(arch==NULL) {
     arch = fopen(tableName, "wb+");
     if(arch == NULL){
         return false;
     }   
     fwrite(&no_deleted_registers, sizeof(int), 1, arch);
    }

    else{
        fclose(arch);
        arch= fopen(tableName, "rb+");
    }
    fclose(arch);

    indxname = (char *)malloc(strlen(tableName)+1);
    replaceExtensionByIdx(tableName, indxname);
    res = createIndex(indxname);
    free(indxname);

    return res;

}
/**
 * @brief function that create an index file.
 * @authors Lucía Gil and Paula Beltrán
 * 
 * @param indexName: name for the file.
 * 
 * @return true if everything went ok and false if anything went wrong.
*/
bool createIndex(const char *indexName){
    FILE *arch=NULL;

    arch = fopen (indexName, "r");
    if(arch==NULL) {
     arch = fopen(indexName, "wb+");
     if(arch == NULL){
         return false;
     }   
     fwrite(&no_deleted_registers, sizeof(int), 1, arch);
     fwrite(&no_deleted_registers, sizeof(int), 1, arch);
    }

    else{
        fclose(arch);
        arch= fopen(indexName, "rb+");
    }
    fclose(arch);

    return true;
}
/**
 * @brief Function that prints recursively the nodes.
 * @authors Lucía Gil and Paula Beltrán
 * 
 * @param _level: level that we are printing.
 * @param level: level up to which we want to print.
 * @param  indexFileHandler: name of the index file.
 * @param node_id: identifier of the node we want to print
 * @param side: side of the tree where the node we are printing is.
 * 
 * @return void function that does not return nothing.
*/
void printnode(size_t _level, size_t level, FILE *indexFileHandler, int node_id, char side) { 
    char primary_key[PK_SIZE];
    int node_left = -1;
    int node_right = -1;
    int node_father = -1;
    int node_offset = -1;
    int i=0;

    if (indexFileHandler==NULL || node_id == -1){
        return;
    }

    if (_level > level){
        return;
    }

    _level++;

    fseek(indexFileHandler, (LINE_SIZE * node_id + INDEX_HEADER_SIZE), SEEK_SET);
    fread(primary_key, PK_SIZE, 1, indexFileHandler);
    fread(&node_left, sizeof(int), 1, indexFileHandler);
    fread(&node_right, sizeof(int), 1, indexFileHandler);
    fread(&node_father, sizeof(int), 1, indexFileHandler);
    fread(&node_offset, sizeof(int), 1, indexFileHandler);

    
    for (i = 0; i < (int)_level; i++){
        printf("\t");
    }
    if(side == 'r' || side == 'l'){
        printf("%c %s (%d) : %d \n", side, primary_key, node_id, node_offset);
    }
    else{
        printf("%s (%d) : %d \n", primary_key, node_id, node_offset);
    }
    
    side='l';
    printnode(_level, level, indexFileHandler, node_left, side);
    side='r';
    printnode(_level, level, indexFileHandler, node_right, side);

}

/**
 * @brief Function that prints a tree.
 * @authors Lucía Gil and Paula Beltrán
 * 
 * @param level: indicates the level up to which we want to print.
 * @param indexName: name of the index file.
 * 
 * @return void function that does not return nothing
*/
void printTree(size_t level, const char *indexName){
    FILE *idx=NULL;
    int node_root=0;
    int _level=0;
    char side='s';

    if(!indexName){
        return;
    }

    idx = fopen(indexName, "rb");
    if(!idx) return;

    fread(&node_root, sizeof(int), 1, idx); /*root*/
    printnode(_level, level, idx, node_root, side);
    fclose(idx);
    return;
}


/**
 * @brief Function that finds a key in an index.
 * @authors Lucía Gil and Paula Beltrán
 * 
 * @param book_id: key we want to search.
 * @param indexName: name of the index file.
 * @param nodeIDOrDataOffset: identifer of the last node visited if the key was not found, or the ofset of the data record if the key was found.
 * 
 * @return true if everything goes right and false if there is any problem.
*/
bool findKey(const char *book_id, const char *indexName, int *nodeIDOrDataOffset){
    FILE *idx=NULL;
    char prim_key[PK_SIZE];
    int node_id=0;
    int node_pos=0;
    int prim_key_comp=0;

    if(book_id == NULL || indexName == NULL || nodeIDOrDataOffset == NULL){
        return false;
    }

    idx = fopen(indexName, "rb");
    if (idx==NULL){
        return false;
    }

    fread(&node_id, sizeof(int), 1, idx);
    while(node_id != -1){

        fseek(idx, (LINE_SIZE * node_id + INDEX_HEADER_SIZE), SEEK_SET);
        fread(&prim_key, PK_SIZE, 1, idx);
        node_pos = node_id;
        prim_key_comp = strncmp(prim_key, book_id, PK_SIZE);

        if(prim_key_comp == 0){
            fseek(idx, 12, SEEK_CUR);
            fread(&node_pos, sizeof(int), 1, idx);
            *nodeIDOrDataOffset = node_pos;
            fclose(idx);
            return true;
        }

        else if(prim_key_comp > 0){ /*The pointer moves to the left son*/
            fread(&node_id, sizeof(int), 1, idx);
        }

        else{ /*The pointer moves to the right son*/
            fseek(idx, DATA_HEADER_SIZE, SEEK_CUR);
            fread(&node_id, sizeof(int), 1, idx);
        }
    }
    fclose(idx);
    
    *nodeIDOrDataOffset = node_pos;
    return false;
}

/**
 * @brief function that inserts a new entry in the index.
 * @authors Lucía Gil and Paula Beltrán
 * 
 * @param book_id: key that we want to insert.
 * @param bookOffset: the offset of the key we are including. 
 * @param indexName: name of the index in where we have to add the entry. 
 * 
 * @return true if everything goes right and false if there is any problem.
*/
bool addIndexEntry(char *book_id, int bookOffset, char const *indexName){
    FILE *idx=NULL;
    char prim_key[PK_SIZE];
    int node_id = -1;
    int node_left = -1;
    int node_right = -1;
    int node_father = -1;
    int node_root=-1;
    int prim_key_comp = 0;
    int node_pos = 0;
    int node_delet = -1;

    if(book_id==NULL || indexName == NULL){
        return false;
    }

    if(findKey(book_id, indexName, &node_father)==true){
        return false;
    }

    idx = fopen(indexName, "rb+");
    if(idx == NULL){
        return false;
    }

    fread(&node_root, sizeof(int), 1, idx);
    fread(&node_id, sizeof(int), 1, idx);

    if(node_id == -1){
        /*If there aren't deleted records, we need to go the end of the file to add the new record*/
        fseek(idx, 0, SEEK_END);
        node_pos = (int)ftell(idx);

        fwrite(book_id, PK_SIZE, 1, idx);
        fwrite(&node_left, sizeof(int), 1, idx);
        fwrite(&node_right, sizeof(int), 1, idx);
        fwrite(&node_father, sizeof(int), 1, idx);
        fwrite(&bookOffset, sizeof(int), 1, idx);

        node_id = (node_pos - INDEX_HEADER_SIZE) / LINE_SIZE;

        if(node_root == -1){
            node_root = 0;
            fseek(idx, 0, SEEK_SET);
            fwrite(&node_root, sizeof(int), 1, idx);
            node_root=-1;
        }
    }

    if(node_id != -1){

        fseek(idx, (LINE_SIZE * node_id + INDEX_HEADER_SIZE), SEEK_SET);

        fseek(idx, PK_SIZE, SEEK_CUR);
        fread(&node_delet, sizeof(int), 1, idx);
        fseek(idx, (LINE_SIZE * node_id + INDEX_HEADER_SIZE), SEEK_SET);

        fwrite(book_id, PK_SIZE, 1, idx);
        fwrite(&node_left, sizeof(int), 1, idx);
        fwrite(&node_right, sizeof(int), 1, idx);
        fwrite(&node_father, sizeof(int), 1, idx);
        fwrite(&bookOffset, sizeof(int), 1, idx);

        fseek(idx, INDEX_HEADER_SIZE/2, SEEK_SET);
        fwrite(&node_delet, sizeof(int), 1, idx);
    }

    fseek(idx, (LINE_SIZE * node_father + INDEX_HEADER_SIZE), SEEK_SET);
    fread(&prim_key, PK_SIZE, 1, idx);
    prim_key_comp = strncmp(prim_key, book_id, PK_SIZE);

    if(prim_key_comp>0){
        fwrite(&node_id, sizeof(int), 1, idx);   
    }
        
    else{
        fseek(idx, 4, SEEK_CUR);
        fwrite(&node_id, sizeof(int), 1, idx);
    }

    fclose(idx);
    return true;
    
}
/**
 * @brief function that inserts a new entry in the table.
 * @authors Lucía Gil and Paula Beltrán
 * 
 * @param book: pointer to a book structure.
 * @param dataName: name of the table in where we have to add the entry.
 * @param indexName: name of the index in where we have to add the entry. 
 * 
 * @return true if everything goes right and false if there is any problem.
*/
bool addTableEntry(Book *book, const char *dataName, const char *indexName){
    FILE *table = NULL;
    int node_deleted=-1;
    int node_size=-1;
    int node_pos=-1;
    int offset=-1;
    int node_te=-1;
    int aux=0;
    char a=' ';

    if(book == NULL || dataName == NULL || indexName == NULL){
        return false;
    }

    if((findKey(book->book_id, indexName, &node_pos))==true){
        printf("The book already exists in the records\n");
        return false;
    }

    table = fopen(dataName, "rb+");
    if(table == NULL){
        return false;
    }

    fread(&node_deleted, sizeof(int), 1, table);

    if(node_deleted==-1){
        fseek(table, 0, SEEK_END);
        fwrite((&book->book_id), PK_SIZE, 1, table);
        fwrite(&book->title_len, sizeof(int), 1, table);
        fwrite(book->title, book->title_len, 1, table);
    }

    else if (node_deleted != -1){
        fseek(table, 0, SEEK_SET);
        node_pos = ftell(table);
        fseek(table, node_deleted, SEEK_SET);

        do {
            fseek(table, node_deleted, SEEK_SET);
            node_deleted=ftell(table);
            fread(&offset, sizeof(int), 1, table);
            fread(&node_size, sizeof(int), 1, table);

            if ((int)book->title_len <= node_size){
                node_te = ftell(table);
                for(aux=0; aux<(int)(book->title_len); aux++){
                    fwrite(&a, sizeof(char), 1, table);
                }
                fseek(table, node_te, SEEK_SET);
                fwrite(book->title, book->title_len, 1, table);
                fseek(table, node_deleted, SEEK_SET);
                fwrite(&(book->book_id), PK_SIZE, 1, table);
                fseek(table, node_pos, SEEK_SET);
                fwrite(&offset, sizeof(int), 1, table);
                if(addIndexEntry(book->book_id, node_pos, indexName)==false){
                    return false;
                }
                fclose(table);
                return true;
            }

            else{
                node_pos = node_deleted;
                node_deleted = offset;
                fseek(table, offset, SEEK_SET); /*We jump to the next node deleted*/
            }
        } while(offset != -1);
    }
    

    if(addIndexEntry(book->book_id, node_pos, indexName)==false){
        return false;
    }
    fclose(table);
    return true;   
}

/**
 * @brief function that prints a table.
 * @authors Lucía Gil and Paula Beltrán
 *
 * @param dataName: name of the table in where we have to add the entry.
 * 
 * @return true if everything goes right and false if there is any problem.
*/
bool printTable(const char *dataName){
    FILE *table = NULL;
    int node_deleted=-1;
    int end=0;
    int pos=0;
    char prim_key[PK_SIZE];
    int len=0;
    char title[MAX_TABLE_NAME];

    if(dataName == NULL){
        return false;
    }

    table = fopen(dataName, "rb+");
    if(table == NULL){
        return false;
    }

    fseek(table, 0, SEEK_END);
    end = ftell(table);
    fseek(table, 0, SEEK_SET);
    fread(&node_deleted, sizeof(int), 1, table);

    fprintf(stdout, "%d\n", node_deleted);

    while (pos != end){
        if(pos == node_deleted){
            fread(&node_deleted, sizeof(int), 1, table);
            fread(&len, sizeof(int), 1, table);
            fread(&title, len, 1, table);
            title[len] = '\0';
            fprintf(stdout, "%d  %d %s\n", node_deleted, len, title);
            strcpy(title, " ");
        }
        else{
            fread(&prim_key, PK_SIZE, 1, table);
            fread(&len, sizeof(int), 1, table);
            fread(&title, len, 1, table);
            title[len] = '\0';
            printf("%d %.4s %d %s\n", pos, prim_key, len, title);
            strcpy(title, " ");
        }
        pos=ftell(table);
    }
    
    return true;
}

