#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

enum errors {
    ERR_COMMON=1,
    ERR_FATAL,
    ERR_IO,
    ERR_TYPE,
    ERR_QUEUE,
    ERR_NAME,
};

#define RAISE_FOPEN(file) \
printf("IoError: Unable to open '%s'\n", file); \
exit(ERR_IO);

#define RAISE_USAGE() \
puts("Usage: qli [option] file"); \
exit(ERR_COMMON);

#define RAISE_UNREACHABLE(queue) \
dumpQueue(queue); \
puts("FatalError: Unreachable(bytecode might be corrupted)"); \
exit(ERR_FATAL);

#define RAISE_RAW_UNREACHABLE() \
puts("FatalError: Unreachable(bytecode might be corrupted)"); \
exit(ERR_FATAL);

#define RAISE_NAME_ERROR(queue) \
dumpQueue(queue); \
puts("NameError: Undefined variable"); \
exit(ERR_NAME);

#define RAISE_EXECUTION_TYPE(queue) \
dumpQueue(queue); \
puts("TypeError: Cannot execute type data"); \
exit(ERR_TYPE);

#define RAISE_EXPECT_DATA(queue) \
dumpQueue(queue); \
puts("TypeError: Expected type data"); \
exit(ERR_TYPE);

#define RAISE_EMPTY_QUEUE() \
puts("QueueError: Cannot dequeue empty queue"); \
exit(ERR_QUEUE);

#endif
