#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <string.h>

typedef struct HM_pair {
    size_t key;
    void *item, *next;
} HM_pair;

typedef struct HM {
    HM_pair *hashmap;
    size_t size;
} HM;

HM HM_init( size_t size ) {
    return (HM){ (HM_pair*)malloc( sizeof(HM_pair)*size ), size };
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

void HM_set( HM hashmap, const char* strkey, void *item ) {
    size_t key = HM_hash(strkey);
    hashmap.hashmap[key % hashmap.size].item = item;
}

void *HM_get( HM hashmap, const char* strkey ) {
    size_t key = HM_hash(strkey);
    return hashmap.hashmap[key % hashmap.size].item;
}


#endif