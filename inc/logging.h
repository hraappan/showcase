#ifndef DEBUG
#define DEBUG 1
#endif

#include <stdio.h>

#define INFO_PRINT(fmt, ...) \
    printf("[INFO] %s:%d %s(): " fmt "\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define ERROR_PRINT(fmt, ...) \
    printf("[ERROR] %s:%d %s(): " fmt "\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#if DEBUG
#define DEBUG_PRINT(fmt, ...) \
    printf("[DEBUG] %s:%d %s(): " fmt "\n", \
           __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define MEM_DEBUG_PRINT(fmt, ...) ((void)0)
#endif