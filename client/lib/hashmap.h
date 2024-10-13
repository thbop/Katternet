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

void HM_free( HM hashmap ) {
    free(hashmap.hashmap);
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

// for ( lastpair = pair; pair->next != NULL; lastpair = pair->next );

#define HM_str_to_hstr( src, dst ) \
    dst = malloc(sizeof(src)); \
    strcpy_s(dst, strlen(src), src);


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
        // HM_str_to_hstr(strkey, pair->strkey);
        strcpy_s(pair->strkey, HM_MAX_KEY_LENGTH, strkey);
        hashmap->hashmap[key] = pair;
        // printf("New pair\n");
    }
    else {
        pair = hashmap->hashmap[key];
        HM_pair *newpair = (HM_pair*)calloc(1, sizeof(HM_pair)); // Needs clean up
        // HM_str_to_hstr(strkey, newpair->strkey);
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


#endif