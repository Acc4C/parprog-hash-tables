#ifndef STATUS
#define STATUS

typedef enum {
    OK = 0,
    ERR_MEMORY_ERROR,
    ERR_NOT_FOUND,
    ERR_INCORRECT_INPUT,
    ERR_NOT_PRIME_NUMBER,
    END_OF_FILE = -1
} Status;

#endif