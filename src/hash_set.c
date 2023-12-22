#include <time.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define BYTE_SIZE 8
#define HASH_SET_DEFAULT_CAPACITY 10

uint16_t hash(const char* key, uint16_t base_value) {
    return 0;
}

typedef struct hash_set {
    uint32_t base;
    uint32_t size;
    uint32_t capacity;


    char* type;
    char** set;
} hash_set;

uint32_t mod(hash_set* set, uint32_t hash_value) {
    // calculating slide lenght (aka count of bits which should be reset to zero)
    uint32_t slide = (sizeof(hash_value) * BYTE_SIZE - set->capacity);

    // get mod and return value
    return ((hash_value << slide) >> slide);
} 

hash_set* hash_set_create(void) {
    // set new rand() seed
    srand(time(NULL));

    // allocate memory for new hash_set object
    hash_set* new_set = calloc(1, sizeof(*new_set));

    // set default values for structure's fields 
    new_set->base = (uint32_t)rand() + 1;
    new_set->size = 0;
    new_set->capacity = (1 << HASH_SET_DEFAULT_CAPACITY);
    new_set->type = calloc(new_set->capacity, sizeof(*new_set->type));
    new_set->set = calloc(new_set->capacity, sizeof(*new_set->set));

    // return pointer to new hash_set
    return new_set;
}

void hash_set_destroy(hash_set* set) {
    // free type array
    free(set->type);

    // free elements in set (strings)
    for (int i = 0; i < set->capacity; ++i) {
        if (set->set[i] != NULL) {
            free(set->set[i]);
        }
    }

    // free array for set
    free(set->set);

    // free current structure
    free(set);
}

bool hash_set_in(hash_set* set, const char* key) {
    uint16_t current_hash = mod(set, hash(key, set->base));
    uint16_t step = 1;

    while (set->type[current_hash] != 0 && strcmp(set->set[current_hash], key) != 0) {
        current_hash += step;
        current_hash = mod(set, current_hash); 
        ++step;
    }
    if (set->type[current_hash] == 0) {
        return false;
    }
    return true;
}

bool hash_set_add(hash_set* set, const char* key) {
    if (4 * set->size >= 3 * set->capacity) {
        // regenerate all hash_set
    }
    uint32_t current_hash = mod(set, hash(key, set->base));
    uint32_t step = 1, start_hash = current_hash;
    while (set->type[current_hash] == 1) {
        current_hash += step;
        current_hash = mod(set, current_hash);
        ++step;
        if (start_hash == current_hash) {
            return false;
        }
    }
    set->set[current_hash] = calloc(strlen(key) + 1, sizeof(*(set->set)));
    strcpy(set->set[current_hash], key);
    set->type[current_hash] = 1;
    return true;
}

void hash_set_delete(hash_set* set, const char* key) {
    if (!hash_set_in(set, key)) {
        return;
    }
    uint32_t current_hash = mod(set, hash(key, set->base));
    uint32_t step = 1;
    while (strcmp(set->set[current_hash], key) != 0) {
        current_hash += step;
        current_hash = mod(set, current_hash);
        ++step;
    }
    free(set->set[current_hash]);
    set->type[current_hash] = 2; 
}
