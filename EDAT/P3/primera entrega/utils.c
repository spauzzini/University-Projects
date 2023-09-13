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
void printnode(size_t _level, size_t level, FILE *indexFileHandler, int node_id, char side) { 
    char primary_key[PK_SIZE];
    int node_left = -1;
    int node_right = -1;
    int node_father = -1;
    int node_offset = -1;
    int i=0;

    if (indexFileHandler==NULL || node_id == -1 || side==NULL){
        return;
    }

    _level++;
    if (_level > level){
        return;
    }

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


    return;
}

void printTree(size_t level, const char *indexName){
    FILE *idx;
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

bool findKey(const char *book_id, const char *indexName, int *nodeIDOrDataOffset){
    
}

bool addIndexEntry(char *book_id, int bookOffset, char const *indexName){
    return true;
}

bool addTableEntry(Book *book, const char *dataName, const char *indexName){
    return true;
}