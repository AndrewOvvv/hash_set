#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdint.h>
#include <stdbool.h>

uint16_t hash(const char* key, uint16_t base_value) {
    return 0;
}

typedef struct hash_set {
    uint16_t counter;
    uint16_t base_values[USHRT_MAX];
    uint16_t base_counter[USHRT_MAX];

    uint16_t own_base[USHRT_MAX];
    char* set[USHRT_MAX];
} hash_set;

hash_set* hash_set_create(void) {
    hash_set* set = calloc(1, sizeof(*set));
    return set;
}

void hash_set_destroy(hash_set* set) {
    free(set);
}

bool hash_set_in(hash_set* set, const char* key) {
    for (int i = 0; i < set->counter; ++i) {
        uint16_t current_hash = hash(key, set->base_values[i]); 
        if (strcmp(key, set->set[current_hash]) == 0) {
            return true;
        }
    }
    return false;
}

bool hash_set_add(hash_set* set, const char* key) {
    if (set->counter == 0) {
        // add new hash base
    } 
    // use newest hash base for hashing new key
    // it's garanted that set->counter > 0, because we have 'if' statement
    // at the begafterining of that function
    uint16_t current_hash = hash(key, set->base_values[set->counter - 1]);
    if (set->own_base[current_hash] == 0) {
        // all is OK. current_hash is free. no collisions so put new key to set
        set->own_base[current_hash] = set->base_values[set->counter - 1];
        set->set[current_hash] = calloc(strlen(key) + 1, sizeof(*set->set[current_hash]));
        strcpy(set->set[current_hash], key);
        return true;
    }

    // collision happens 
    if (set->own_base[current_hash] != set->base_values[set->counter - 1]) {
        // if place fiiled by word with old hash_base value
        if (hash_set_add(set, set->set[current_hash])) {
            // if successfully replace collisioned word with newest hash_base
            for ()
        }

    }

    // collision happens inside one hash_base value => generate new hash_base and code
    // key with it
    return false;
}


