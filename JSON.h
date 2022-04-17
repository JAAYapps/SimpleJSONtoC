#ifndef JSON_H
#define JSON_H

#ifndef external
#include "Common-Code-For-C-Projects\Common.h"
#endif
#ifdef external
#include "..\Common-Code-For-C-Projects\memory.h"
#endif

#define SUCCESS 0x0
#define SYNTAX_ERROR 0x1
#define FILE_READ_ERROR 0x2
#define VALUE_IDENTIFICATION_ERROR 0x3
#define MEMORY_CREATION_ERROR 0x4

typedef enum ObjectType
{
    number,
    string,
    boolean,
    element,
    null,
    empty
} ObjectType;

typedef struct Error
{
    BOOL isError;
    UINT8 code;
} Error;

typedef struct JSON_Element
{
    char *name;
    ObjectType type;
    BOOL isArray;
    UINT64 arraySize;
    void *parent;
    void *prev;
    void *object;
    void *next;
    Error error;
} JSON_Element;

JSON_Element *CreateElement(void);

JSON_Element *BuildJSONFromFile(char *file);

void FreeJSONObjectFromMemory(JSON_Element *root);

#endif