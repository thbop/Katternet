#ifndef CORE_H
#define CORE_H

// Nonspecific core functions/macros

#define CORE_str_to_hstr( src, dst ) \
    dst = malloc(sizeof(src)); \
    strcpy_s(dst, strlen(src), src);




#endif