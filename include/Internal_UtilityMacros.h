#ifndef INTERNAL_UTILITY_MACROS
#define INTERNAL_UTILITY_MACROS

#define ASSERT_SUCCESS(x) \
    do { if ((x) == -1) return -1; } while (0)

#define PRINT_SV(sv) \
    printf("%s: %.*s\n", #sv, (int)(sv).Count, (sv).Content)    

#endif