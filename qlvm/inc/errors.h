#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdlib.h>

enum errors {
    ERR_IO=1,
    ERR_UNREACHABLE,
    ERR_TYPE,
    ERR_QUEUE,
};

#define RAISE_FOPEN(filename)\
printf("IoError: Unable to open file: %s\n", filename);\
exit(ERR_IO);

#define RAISE_UNREACHABLE()\
printf("FatalError: Unreachable (bytecode might be corrupted)\n");\
exit(ERR_UNREACHABLE);

#define RAISE_EXEC_DATA()\
printf("TypeError: Cannot execute type: 'data'\n");\
exit(ERR_TYPE);

#define RAISE_INVALID_ARG()\
printf("TypeError: Argument must have type: 'data'\n");\
exit(ERR_TYPE)

#define RAISE_EMPTY_QUEUE()\
printf("QueueError: Dequeued empty queue\n");\
exit(ERR_QUEUE);

#endif
