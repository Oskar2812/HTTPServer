#ifndef INTERNAL_UTILITY_MACROS
#define INTERNAL_UTILITY_MACROS

#define ASSERT_SUCCESS(x) \
    do { if ((x) == -1) return -1; } while (0)

#define ASSERT_AND_LOG_SUCCESS(server, x, ...) \
    do { \
        if ((x) == -1) { \
        Log((server), LOG_ERROR, __VA_ARGS__); \
        return -1; \
    }} while (0)

#define PRINT_SV(sv) \
    printf("%s: %.*s\n", #sv, (int)(sv).Count, (sv).Content)    

#endif

#define MIN(x, y) \
    (x) < (y) ? (x) : (y)