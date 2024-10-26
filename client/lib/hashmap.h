#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HM_MAX_KEY_LENGTH       16
#define HM_MAX_FILE_LINE_LENGTH 128
#define HM_MAX_STR_ITEM_LENGTH  HM_MAX_FILE_LINE_LENGTH-HM_MAX_KEY_LENGTH-2

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

void HM_free( HM *hashmap, bool free_items ) {
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
        strncpy(pair->strkey, strkey, HM_MAX_KEY_LENGTH);
        hashmap->hashmap[key] = pair;
        // printf("New pair\n");
    }
    else if ( !strncmp(hashmap->hashmap[key]->strkey, strkey, HM_MAX_KEY_LENGTH) ) {
        hashmap->hashmap[key]->item = item;
    }
    else {
        pair = hashmap->hashmap[key];
        HM_pair *newpair = (HM_pair*)calloc(1, sizeof(HM_pair)); // Needs clean up
        strncpy(newpair->strkey, strkey, HM_MAX_KEY_LENGTH);
        newpair->item = item;
        newpair->next = pair;
        hashmap->hashmap[key] = newpair;
        // printf("Collision\n");
    }
    return 0;
}

// int HM_set_str( HM *hashmap, char *strkey, char *item ) {
//     char *buf = calloc(sizeof(item), 1);
//     strcpy_s(buf, sizeof(item), item);
//     return HM_set(hashmap, strkey, buf);
// }


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

int HM_set_str( HM *hashmap, char *strkey, char *item, bool free_old ) {
    if ( free_old ) free(HM_get(hashmap, strkey));
    char *buf = calloc(sizeof(item), 1);
    strncpy(buf, item, sizeof(item));
    buf[sizeof(item)] = 0;
    return HM_set(hashmap, strkey, buf);
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
    return size+1;
}

char *HM_dumps( HM *hashmap ) {
    size_t
        size = HM_calc_str_size(hashmap),
        x = 0;
    char *str = (char*)malloc(size);

    for ( size_t i = 0; i < hashmap->size; i++ ) {
        if ( hashmap->hashmap[i] != NULL ) {
            HM_pair *pair = hashmap->hashmap[i];
            do {
                CORE_strcpy_d(str, pair->strkey, size, x );
                x += strlen(pair->strkey);
                str[x] = ' '; x++;
                CORE_strcpy_d(str, (char*)pair->item, size, x );
                x += strlen((char*)pair->item);
                str[x] = '\n'; x++;
            }
            while ( pair = pair->next );
        }
    }
    str[x] = 0;
    return str;
}

int HM_dump( HM *hashmap, const char* filename ) {
    FILE *fp = fopen(filename, "w");
    if ( fp == NULL ) return 1;
    char *str = HM_dumps(hashmap);
    if (fputs(str, fp) == EOF) {
        free(str);
        fclose(fp);
        return 1;
    }

    free(str);
    fclose(fp);
    return 0;
}

// Fills an intialized hashmap from a file
int HM_load( HM *hashmap, const char* filename ) {
    FILE *fp = fopen(filename, "r");
    if ( fp == NULL ) return 1;
    char
        buf[HM_MAX_FILE_LINE_LENGTH],
        strkey[HM_MAX_KEY_LENGTH],
        *item;

    size_t kx, ix;
    while ( fgets( buf, HM_MAX_FILE_LINE_LENGTH, fp ) ) {
        kx = ix = 0;
        if ( strlen(buf) > 3 ) {
            kx = CORE_find_char(buf, ' ');
            ix = CORE_find_char(buf, '\n');
            if ( kx < HM_MAX_KEY_LENGTH ) { strncpy( strkey, buf, kx+1 ); strkey[kx] = 0; }
            else { fclose(fp); return 1; }

            item = (char*)calloc(HM_MAX_STR_ITEM_LENGTH, 1);
            if ( ix < HM_MAX_FILE_LINE_LENGTH ) strncpy( item, buf+kx+1, ix-kx-1 );
            else { fclose(fp); free(item); return 1; }
            
            // printf("|%s|: |%s|\n", strkey, item);
            // CORE_printhex(strkey);
            // CORE_printhex(item);
            HM_set(hashmap, strkey, item);

            memset(strkey, 0, HM_MAX_KEY_LENGTH);
        }
    }
    fclose(fp);
    return 0;
}



#endif
