#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <string.h>

#define HM_MAX_KEY_LENGTH 16

typedef struct HM_pair {
    size_t key;
    // char *strkey;
    char strkey[HM_MAX_KEY_LENGTH];
    void *item, *next;
} HM_pair;

typedef struct HM {
    HM_pair **hashmap;
    size_t size;
} HM;

HM HM_init( size_t size ) {
    return (HM){ (HM_pair**)calloc( size, sizeof(HM_pair*) ), size };
}

void HM_free( HM *hashmap ) {
    for ( size_t i = 0; i < hashmap->size; i++ ) {
        if ( hashmap->hashmap[i] != NULL ) free(hashmap->hashmap[i]);
    }
    free(hashmap->hashmap);
}

size_t HM_hash( const char* strkey ) {
    size_t
        key  = 0,
        key2 = 0;
    for ( size_t i = 0; i < strlen(strkey); i++ ) {
        switch (key % 3) {
            case 0: key += strkey[i];   key2 *= i;             break;
            case 1: key += i;           key2 ^= strkey[i]*295; break;
            case 2: key ^= i*strkey[i]; key2 &= i*20617;       break;
        }
    }
    for ( size_t i = strlen(strkey); i > 0; i-- ) {
        switch (key % 3) {
            case 0: key += strkey[i];   key2 ^= i*strkey[i]; break;
            case 1: key += i;           key2 *= i;           break;
            case 2: key ^= i*strkey[i]; key2 += strkey[i];   break;
        }
    }
    return key*key2;
}


int HM_set( HM *hashmap, char *strkey, void *item ) {
    if ( strlen(strkey) > HM_MAX_KEY_LENGTH ) {
        printf("HASHMAP ERROR: Key \"%s\" length exceeds HM_MAX_KEY_LENGTH = %d!\n", strkey, HM_MAX_KEY_LENGTH);
        return 1;
    }
    size_t key = HM_hash(strkey) % hashmap->size;
    // printf("Create key: %d\n", key);
    HM_pair *pair;
    if ( hashmap->hashmap[key] == NULL ) {
        pair = (HM_pair*)calloc(1, sizeof(HM_pair)); // Needs clean up
        pair->item = item;
        strcpy_s(pair->strkey, HM_MAX_KEY_LENGTH, strkey);
        hashmap->hashmap[key] = pair;
        // printf("New pair\n");
    }
    else {
        pair = hashmap->hashmap[key];
        HM_pair *newpair = (HM_pair*)calloc(1, sizeof(HM_pair)); // Needs clean up
        strcpy_s(newpair->strkey, HM_MAX_KEY_LENGTH, strkey);
        newpair->item = item;
        newpair->next = pair;
        hashmap->hashmap[key] = newpair;
        // printf("Collision\n");
    }
    return 0;
}


void *HM_get( HM *hashmap, const char* strkey ) {
    size_t key = HM_hash(strkey) % hashmap->size;
    HM_pair *pair = hashmap->hashmap[key];
    
    while ( pair ) {
        // printf("pair->strkey: %s\n", pair->strkey);
        if ( !strncmp(strkey, pair->strkey, HM_MAX_KEY_LENGTH) ) return pair->item;
        pair = pair->next;
    }
    return NULL;
}

size_t HM_calc_str_size( HM *hashmap ) {
    size_t size = 0;
    for ( size_t i = 0; i < hashmap->size; i++ ) {
        if ( hashmap->hashmap[i] != NULL ) {
            HM_pair *pair = hashmap->hashmap[i];
            do {
                size += strlen(pair->strkey) + strlen((char*)pair->item) + 2; // Assumes item is string... does not handle other datatypes
            }
            while ( pair = pair->next );
        }
    }
    return size;
}

char *HM_dumps( HM *hashmap ) {
    char
        *str = (char*)malloc(HM_calc_str_size(hashmap)),
        *str_c = str; // Cursor

    for ( size_t i = 0; i < hashmap->size; i++ ) {
        if ( hashmap->hashmap[i] != NULL ) {
            HM_pair *pair = hashmap->hashmap[i];
            do {
                size_t delta = strlen(pair->strkey);
                strcpy_s(str_c, delta, pair->strkey);
                str_c += delta; str_c[0] = ' ';
                delta = strlen((char*)pair->item); // Also assumes item is string
                strcpy_s(str_c, delta, (char*)pair->item);
                str_c += delta; str_c[0] = '\n';
            }
            while ( pair = pair->next );
        }
    }
    return str;
}

    // char f[8] = "aaaa";
    // char d[8] = "bb";

    // HM hashmap = HM_init(6);
    // HM_set(&hashmap, "aa", f);
    // HM_set(&hashmap, "bb", d);

    // // printf("%s\n", HM_get(&hashmap, "water"));
    // char *hm_s = HM_dumps(&hashmap);
    // printf("%s\n", hm_s);

    // HM_free(&hashmap);


#endif