#ifndef DEBUG_LIB_H

#define DEBUG_LIB_H

#include <stdio.h>

#define PRINT_LINE do{ fprintf(stderr, "I'm at %s at line %d in %s\n", __PRETTY_FUNCTION__, __LINE__, __FILE__); }while(0);

#define PRINT_PTR(ptr) do{ fprintf(stderr, "pointer %s at %p at line %d\n", #ptr, ptr, __LINE__); }while(0);

#define PRINT_C(char) do{ fprintf(stderr, "[%s:%lu] %s = %c\n", __PRETTY_FUNCTION__, __LINE__, #char, char); }while(0);

#define PRINT_S(str) do{ fprintf(stderr, "[%s:%lu] %s = %s\n", __PRETTY_FUNCTION__, __LINE__, #str, str); }while(0);

#define PRINT(str) do{ fprintf(stderr, "%s\n", #str); }while(0);

#define PRINT_UL(num) do{ fprintf(stderr, "[%s:%lu] %s = %lu\n", __PRETTY_FUNCTION__, __LINE__, #num, num); }while(0);

#define PRINT_D(num) do{ fprintf(stderr, "[%s:%lu] %s = %d\n", __PRETTY_FUNCTION__, __LINE__, #num, num); }while(0);


#endif
