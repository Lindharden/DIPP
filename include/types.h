typedef struct Data {
    int value;
} Data;

typedef void (*ProcessFunction)(Data *);

typedef struct {
    ProcessFunction *functions;
    size_t size;
} Pipeline;