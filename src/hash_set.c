#include <time.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define BYTE_SIZE 8
#define HASH_SIZE_MULTIPLIER 2
#define HASH_SET_DEFAULT_CAPACITY 10

#define FNV_OFFSET 236528657
#define FNV_PRIME 12838907


// test section
int add_iter = 0;
int add_cnt = 0;
int in_iter = 0;
int in_cnt = 0;
int delete_cnt = 0;
// test section



static uint32_t hash(const char* key, uint16_t base_value) {
    uint32_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}

typedef struct hash_set {
    uint32_t base;
    uint32_t size;
    uint32_t capacity;


    char* type;
    char** set;
} hash_set;

// declaration of functions
uint32_t mod(hash_set* set, uint32_t hash_value);

void hash_set_free(hash_set* set);

hash_set* hash_set_alloc(int count, ...);

hash_set* hash_set_realloc(hash_set* set);

bool hash_set_in(hash_set* set, const char* key);

hash_set* hash_set_add(hash_set* set, const char* key);

void hash_set_delete(hash_set* set, const char* key);

// list of functions
uint32_t mod(hash_set* set, uint32_t hash_value) {
    // calculating slide lenght (aka count of bits which should be reset to zero)
    uint32_t slide = (sizeof(hash_value) * BYTE_SIZE - set->capacity);

    // get mod and return value
    return ((hash_value << slide) >> slide);
}

hash_set* hash_set_alloc(int count, ...) {
    // set new rand() seed
    srand(time(NULL));

    // get capacity if it setted
    uint32_t capacity = HASH_SET_DEFAULT_CAPACITY;
    va_list args;
    va_start(args, count);
    for (int i = 0; i < count; ++i) {
        capacity = va_arg(args, uint32_t);
    }
    va_end(args);

    // allocate memory for new hash_set object
    hash_set* new_set = calloc(1, sizeof(*new_set));

    // set default values for structure's fields 
    new_set->base = (uint32_t)rand() + 1;
    new_set->size = 0;
    new_set->capacity = capacity;
    new_set->type = calloc(1u << new_set->capacity, sizeof(*new_set->type));
    new_set->set = calloc(1u << new_set->capacity, sizeof(*new_set->set));

    // return pointer to new hash_set
    return new_set;
}

void hash_set_free(hash_set* set) {
    // free type array
    free(set->type);

    // free elements in set (strings)
    for (int i = 0; i < (1u << set->capacity); ++i) {
        if (set->set[i] != NULL) {
            free(set->set[i]);
        }
    }

    // free array for set
    free(set->set);

    // free current structure
    free(set);
}

hash_set* hash_set_realloc(hash_set* set) {
    uint32_t new_capacity = set->capacity + 1;
    hash_set* new_set = hash_set_alloc(1, new_capacity);
    if (new_set == NULL) {
        return set;
    }

    for (int i = 0; i < (1u << set->capacity); ++i) {
        if (set->type[i] == 1) {
            hash_set_add(new_set, set->set[i]);
        }
    } 

    hash_set_free(set);
    return new_set;
}

bool hash_set_in(hash_set* set, const char* key) {
    uint16_t current_hash = mod(set, hash(key, set->base));
    uint16_t step = 1;

    while (set->type[current_hash] != 0 && strcmp(set->set[current_hash], key) != 0) {
        ++in_iter;
        current_hash += step;
        current_hash = mod(set, current_hash); 
        ++step;
    }
    if (set->type[current_hash] == 0) {
        return false;
    }
    return true;
}

hash_set* hash_set_add(hash_set* set, const char* key) {
    if (4 * set->size >= 3 * (1u << set->capacity)) {
        // regenerate all hash_set
        set = hash_set_realloc(set);
        if (4 * set->size >= 3 * (1u << set->capacity)) {
            return set;
        }
        printf("realloc set!!\n");
    }
    if (hash_set_in(set, key)) {
        return set;
    }
    uint32_t current_hash = mod(set, hash(key, set->base));
    uint32_t step = 1, start_hash = current_hash;
    while (set->type[current_hash] == 1) {
        ++add_iter;
        current_hash += step;
        current_hash = mod(set, current_hash);
        ++step;
        if (start_hash == current_hash) {
            return set;
        }
    }
    set->set[current_hash] = calloc(strlen(key) + 1, sizeof(*(set->set)));
    strcpy(set->set[current_hash], key);
    set->type[current_hash] = 1;
    ++set->size;
    return set;
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
    set->set[current_hash] = NULL;
    set->type[current_hash] = 2; 
    --set->size;
}


int main(int argc, char* argv[]) {
    hash_set* set = hash_set_alloc(0);

    int cnt = 100000;
    
    char** array = calloc(cnt, sizeof(*array));
    for (int i = 0; i < cnt; ++i) {
        array[i] = calloc(31, sizeof(*(array[i])));
        scanf("%s", array[i]);
    }

    for (int i = 0; i < cnt / 2; ++i) {
        printf("start adding %d element\n", i);
        set = hash_set_add(set, array[i]);
        ++add_cnt;
        printf("added %d element\n", i);
    }
    for (int i = 0; i < cnt / 2; ++i) {
        if (!hash_set_in(set, array[i])) {
            printf("lost %s\n", array[i]);
        }
        ++in_cnt;
    }
    for (int i = cnt / 2; i < cnt; ++i) {
        if (hash_set_in(set, array[i])) {
            printf("find fake %s\n", array[i]);
        }
        ++in_cnt;
    }

    for (int i = 0; i < cnt; ++i) {
        free(array[i]);
    }
    free(array);
    hash_set_free(set);

    printf("%d/%d - add\n", add_iter, add_cnt);
    printf("%d/%d - in\n", in_iter, in_cnt);
    return 0;
}
