/*
* Hash Table Demo.
* Hash is an integer, key is a string
* Use linear probing to avoid hash collisions
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h> //printf("%" PRIu64 "\n", key);
#include <string.h>
#include <stdbool.h>

#include "hashtable.h"

#define STR_LIMIT 25
#define LOAD_FACTOR 0.75

//Hash vars
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

float load_factor = LOAD_FACTOR;

//Fowler–Noll–Vo hash function
static uint64_t hash_key(const char* key) {
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

void initArray(Array *a, size_t initialSize) {
    a->array = malloc(initialSize * sizeof(char) * STR_LIMIT);
    a->used = 0;
    a->size = initialSize;
    //initialise string array
    int i = 0;
    for (i = 0; i < initialSize; i++){
        a->array[i] = NULL;
    }
}

//rehash array after expanding it
void rehash(Array *a){
    char** tmp = malloc(a->size * sizeof(char) * STR_LIMIT);
    int i, tmpIncrementor, newIndex = 0;

    //initialise temp array
    for(i = 0; i < a->size; i++){
        tmp[i] = NULL;
    }

    //fill temp array with new hashes
    for(i = 0; i < a->size; i++){
        if (a->array[i] != NULL){
            newIndex = hash_key(a->array[i]) % a->size;

            //double check if collision in temp array just to be sure
            if (tmp[newIndex] != NULL){
                for (tmpIncrementor = 0; tmpIncrementor < a->size; tmpIncrementor++){
                    if (tmp[newIndex + tmpIncrementor % a->size] == NULL){
                        tmp[newIndex + tmpIncrementor % a->size] = a->array[i];
                        break;
                    }
                }
            } else {
                tmp[newIndex] = a->array[i];
            }
        }
    }

    memcpy(a->array, tmp, a->size * sizeof(char) * STR_LIMIT);
    free(tmp);
}

//loops over array to find an empty spot
void collisionResolve(Array *a, char *element, int problemIndex){
    int i = 0, end = a->size;
    for (i = 0; i < end; i++){
        if (a->array[i + problemIndex % end] == NULL){
            a->array[i + problemIndex % end] = element;
            a->used++;
            printf("Inserting '%s' Into Index: %d\n\n", element, i);
            break;
        }
    }
}

void insertArray(Array *a, char *element) {
    //if array is at load, increase it's size
    long load_limit = a->size * (float)LOAD_FACTOR;

    if (a->used == load_limit) {
        int oldSize = a->size;
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(char) * STR_LIMIT);
        printf("\nIncreasing Array Size\n\n");

        //initialise the rest of the array
        int i = 0;
        for (i = oldSize; i < a->size; i++){
            a->array[i] = NULL;
        }

        //rehash array
        rehash(a);
    }
    uint64_t hash = hash_key(element);
    int index = hash % a->size;
    printf("Hash: %" PRIu64 "\n", hash);
    //allocate space for temp value
    char* tmp = malloc( sizeof(char) * STR_LIMIT );

    //check if collision
    if (a->array[index] != NULL){
        printf("Collision Detected With Index %d\n", index);
        //check if duplicate value
        if ( strncmp( element, a->array[index], sizeof(char) * STR_LIMIT ) == 0 ){
            printf("Duplicate Value Found: %s\n\n", element);
        } else { // perform collision insertion
            strcpy(tmp, element);
            collisionResolve(a, tmp, index);
        }
    } else { // normal insert
        printf("Inserting %s Into Index: %d\n\n", element, index);
        strcpy(tmp, element);
        a->array[index] = tmp;
        a->used++;
    }
}

void freeArray(Array *a) {
    int i;
    //free each index of allocated string mem
    for (i = 0; i < a->size; i++){
        free(a->array[i]);
    }
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

void printArray(Array *a) {
    printf("\nTABLE CONTENTS\n");
    float load_limit = (float)a->used / (float)a->size ;
    printf("Load Percent: %.2f%%\n", load_limit);
    int end = a->size, i = 0;
    for (i = 0; i < end; i++){
        printf("Index: %d  Value: %s\n", i, a->array[i]);
    }
    printf("\n");
}

void find(Array *a, char *target){
    //try hash index
    int index = hash_key(target) % a->size, i = 0;

    if (a->array[index] != NULL){
        //check if value is equal to target because collisions do happen
        if ( strcmp( a->array[index], target ) == 0){
            printf("Found %s At Index %d\n\n", target, index);
            return;
        }
    }
    // O(n) worst case scenario search
    for (i = 0; i < a->size; i++){
        if ( a->array[i] != NULL && strcmp( a->array[i], target ) == 0 ){
            printf("Found %s At Index %d\n\n", target, i);
            return;
        }
    }
    printf("'%s' Not Found In Table\n\n", target);
}

void delete(Array *a, char *target){
    //try hash index
    int index = hash_key(target) % a->size, i = 0;

    if (a->array[index] != NULL){
        //check if value is equal to target because collisions do happen
        if ( strcmp( a->array[index], target ) == 0){
            a->array[index] = NULL;
            a->used--;
            printf("Removed %s At Index %d\n\n", target, index);
            return;
        }
    }
    // O(n) worst case scenario search
    for (i = 0; i < a->size; i++){
        if ( a->array[i] != NULL && strcmp( a->array[i], target ) == 0 ){
            a->array[i] = NULL;
            a->used--;
            printf("Removed %s At Index %d\n\n", target, i);
            return;
        }
    }
    printf("'%s' Not Found In Table\n\n", target);
}

bool startsWith(const char *pre, const char *str){
    size_t lenpre = strlen(pre), lenstr = strlen(str);
    return lenstr < lenpre ? 0 : memcmp(pre, str, lenpre) == 0;
}

int main(){
    Array a;
    initArray(&a, 3);

    size_t size = a.size;
    printf("Array Size %d\n", size);
    //helper strings
    char str[STR_LIMIT];

    while(1){
        printf( "Enter a value: ");
        fgets( str, STR_LIMIT, stdin );
        //remove newline
        str[strcspn(str, "\n")] = 0;

        //exit condition
        if  (strcmp(str, "!exit") == 0 ){
            break;
        } else if( strcmp(str, "!print") == 0 ){
            //print entire array
            printArray(&a);
        } else if( startsWith("!find", str) ){
            //Find element in array if it exists
            strncpy(str, str + 6, STR_LIMIT - 5);
            find(&a, str);
        } else if( startsWith("!rm", str) ){
            //Remove element in array if it exists
            strncpy(str, str + 4, STR_LIMIT - 3);
            delete(&a, str);
        } else {
            insertArray( &a, str );
        }
    }

    //clean up because C
    freeArray(&a);
    return 0;
}

//Dynamic Array: https://stackoverflow.com/questions/3536153/c-dynamically-growing-array
//Hash Table Theory: https://benhoyt.com/writings/hash-table-in-c/