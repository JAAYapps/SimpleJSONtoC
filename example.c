#include <stdio.h>
#include <stdlib.h>
#include "Common-Code-For-C-Projects\common.h"
#include "Common-Code-For-C-Projects\memory.h"
#include "JSON.h"

void traverseJSON(JSON_Element *root);

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        printf("The argument supplied is %s\n", argv[1]);
        printf("\n");
        JSON_Element *config = BuildJSONFromFile(argv[1]);
        if (config->error.isError)
            Print("Json error occured.");
        else
            traverseJSON(config);

        printf("\nFreeing Memory.");
        FreeJSONObjectFromMemory(config);
        printf("\nDone!!!");
    }
    else if (argc > 2)
    {
        printf("Too many arguments supplied.\n");
        printf("%d ", argc);
    }
    else
    {
        printf("One argument expected.\n");
    }
    return 0;
}

void traverseJSON(JSON_Element *root)
{
    JSON_Element *e = root;
    while (e->next != NULL)
    {
        Print("\r\n\r\nIn Loop");
        Print("\r\nType: ");
        if (e->type == boolean)
            Print("Boolean\r\n");
        if (e->type == number)
            Print("Number\r\n");
        if (e->type == string)
            Print("String\r\n");
        if (e->type == empty)
            Print("Empty\r\n");
        if (e->type == null)
            Print("NULL\r\n");
        if (e->type == element)
            Print("Element\r\n");
        Print("Name: ");
        if (e->name == NULL)
            Print("is Null for some reason.\r\n");
        else
            Print(e->name);
        if (e->type == element)
        {
            if (e->object == NULL)
                Print("Object is Null for some reason.\r\n");
            else
                traverseJSON((JSON_Element *)(e->object));
        }
        e = (JSON_Element *)e->next;
    }
    Print("\r\n\r\nType: ");
    if (e->type == boolean)
        Print("Boolean\r\n");
    if (e->type == number)
        Print("Number\r\n");
    if (e->type == string)
        Print("String\r\n");
    if (e->type == empty)
        Print("Empty\r\n");
    if (e->type == null)
        Print("NULL\r\n");
    if (e->type == element)
        Print("Element\r\n");
    Print("Name: ");
    if (e == NULL)
        Print("is Null for some reason.\r\n");
    else
        Print(e->name);
    if (e->type == element)
    {
        if (e->object == NULL)
            Print("Object is Null for some reason.\r\n");
        else
            traverseJSON((JSON_Element *)(e->object));
    }
}

void *AssignAndInitMemory(UINT64 typeSize, UINT64 ElementSize)
{
    static int count = 0;
    char BLINFO[12];
    Print("\r\nCreated ");
    itoa(++count, BLINFO, DECIMAL);
    Print(BLINFO);
    return calloc(ElementSize, typeSize);
}

void *AssignMemory(UINT64 typeSize, UINT64 ElementSize)
{
    static int count = 0;
    char BLINFO[12];
    Print("\r\nCreated no init ");
    itoa(++count, BLINFO, DECIMAL);
    Print(BLINFO);
    return malloc(ElementSize * typeSize);
}

void *ResizeMemory(void *element, UINT64 originalSize, UINT64 typeSize, UINT64 ElementSize)
{
    static int count = 0;
    char BLINFO[12];
    Print("\r\nResize ");
    itoa(++count, BLINFO, DECIMAL);
    Print(BLINFO);
    return realloc(element, ElementSize * typeSize);
}

void FreeMemory(void *element)
{
    static int count = 0;
    char BLINFO[12];
    Print("\r\nDeleted ");
    itoa(++count, BLINFO, DECIMAL);
    Print(BLINFO);
    free(element);
}

unsigned char *OpenAndWriteFileToMemory(char *fileName, UINT64 *size)
{
    FILE *file = fopen((const char *)fileName, "rb");

    if (file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long sz = ftell(file);
        fseek(file, 0, SEEK_SET);
        unsigned char *byte = AssignAndInitMemory(sizeof(char), sz + 1);
        Print(" for buffer of file.");
        for (long i = 0; i < sz; i++)
            byte[i] = (char)fgetc(file);
        byte[sz] = '\0';
        fclose(file);
        *size = sz;
        return byte;
    }
    else
    {
        printf("File %s encountered an error.\n", fileName);
    }
    return NULL;
}

void Print(char *str)
{
#ifdef PRINT
    printf((const char *)str);
#endif
}