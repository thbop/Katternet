#ifndef CORE_H
#define CORE_H
#include <string.h>

// Nonspecific core functions/macros

#define CORE_str_to_hstr( src, dst ) \
    dst = malloc(sizeof(src)); \
    strcpy_s(dst, strlen(src), src);

#define CORE_input( buf, buflen ) \
    fgets(buf, buflen, stdin); \
    buf[strlen(buf)-1] = 0 // Remove newline character

void CORE_strcpy_d( char* dst, const char* src, size_t dstlen, size_t dstpos ) {
    for ( size_t i = dstpos; i < dstlen-1; i++ ) {
        if ( src[i-dstpos] ) dst[i] = src[i-dstpos];
        else i = dstlen;
    }
}

// void CORE_strcpy_s( char* dst, char* src, size_t dstlen, size_t srcpos ) {
//     for ( size_t i = 0; i < dstlen-1; i++ ) {
//         if ( src[i+srcpos] ) dst[i] = src[i+srcpos];
//         else {dst[i-2] = 0; i = dstlen;}
//     }
// }

size_t CORE_find_char( char *str, char c ) {
    for ( size_t i = 0; i < strlen(str); i++ ) {
        if ( c == str[i] ) return i;
    }
}

void CORE_printhex(const char* str) {
    for ( int i = 0; i < strlen(str); i++ )
        printf("%02X ", str[i]);
    printf("\n");
}

// char *CORE_readfile( const char* filename ) {
//     FILE *fp = fopen(filename, "rt");
//     if ( fp == NULL ) return NULL;

//     fseek(fp, 0, SEEK_END);
//     size_t fsize = (size_t)ftell(fp);
//     fseek(fp, 0, SEEK_SET);

//     char *data = (char*)malloc(fsize+1);
//     while ( fgets() )

//     fclose(fp);

//     return NULL;
// }


#endif