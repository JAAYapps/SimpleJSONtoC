#include "JSON.h"
#ifndef external
#include "Common-Code-For-C-Projects\memory.h"
#endif
#ifdef external
#include "..\Common-Code-For-C-Projects\memory.h"
#endif

JSON_Element *CreateElement()
{
    JSON_Element *element = NULL;
    element = AssignAndInitMemory(sizeof(JSON_Element), 1);
    Print(" a JSON Element.");
    if (element != NULL)
    {
        element->name = NULL;
        element->isArray = false;
        element->object = NULL;
        element->type = null;
        element->next = NULL;
        element->parent = NULL;
        element->prev = NULL;
        element->error.isError = false;
        element->error.code = 0x00;
        return element;
    }
    Print("There was a problem creating a JSON element.\r\nFailed to allocate memory.");
    return NULL;
}

unsigned long long StringLength(const char *str)
{
    const char *strCount = str;

    while (*strCount++)
        ;
    return strCount - str - 1;
}

int compareString(const char *a, const char *b)
{
    int length = StringLength(a);
    int length2 = StringLength(b);
    if (length == length2)
    {
        for (int i = 0; i < length; i++)
        {
            if (a[i] != b[i])
            {
                return 0;
            }
        }
    }
    else
        return 0;
    return 1;
}

void NumberToString(unsigned long int n, char *buffer, int basenumber)
{
    unsigned long int hold;
    int i, j;
    hold = n;
    i = 0;

    do
    {
        hold = n % basenumber;
        buffer[i++] = (hold < 10) ? (hold + '0') : (hold + 'a' - 10);
    } while (n /= basenumber);
    buffer[i--] = 0;

    for (j = 0; j < i; j++, i--)
    {
        hold = buffer[j];
        buffer[j] = buffer[i];
        buffer[i] = hold;
    }
}

BOOL IsWhitespace(UINT8 *buffer, UINT64 index)
{
    return buffer[index] == ' ' || buffer[index] == '\n' || buffer[index] == '\r';
}

char LookAhead(UINT8 *buffer, UINT64 index)
{
    if (!IsWhitespace(buffer, index))
        return buffer[index];
    else
    {
        while (IsWhitespace(buffer, index) && buffer[index] != '\0')
            index++;
        return buffer[index];
    }
}

BOOL IsTokenCharacter(UINT8 *buffer, UINT64 index, BOOL strRecord)
{
    BOOL openBr = buffer[index] == '{' && !strRecord;
    BOOL closeBr = buffer[index] == '}' && !strRecord;
    BOOL openBrck = buffer[index] == '[' && !strRecord;
    BOOL closeBrck = buffer[index] == ']' && !strRecord;
    BOOL com = buffer[index] == ',' && !strRecord;
    BOOL col = buffer[index] == ':' && !strRecord;
    BOOL dqut = buffer[index] == '"';
    BOOL dqutInStr = buffer[index] == '"' && buffer[index - 1] != '\\';
    return openBr || closeBr || openBrck || closeBrck || dqut || dqutInStr || com || col;
}

void SetupArray(JSON_Element *e, UINT8 *value, UINT64 index)
{
    Print(" In array object. ");
    UINT64 count = index;
    UINT64 arraySize = 0;
    ObjectType arrayType = null;
    BOOL inStr = false;
    BOOL gotVal = false;
    BOOL isTrue = false;
    BOOL isFalse = false;
    while (value[count] != ']' || inStr)
    {
        if (value[count] == '\0')
        {
            e->error.isError = true;
            e->error.code = SYNTAX_ERROR;
            Print("End of file has occurred.\r\nYou are missing an end square bracket ']',\r\nor you forget an ending double quote '\"' in one of your strings.");
            return;
        }
        if (!IsWhitespace(value, count))
        {
            Print(" ");
            char s[2];
            s[0] = value[count];
            s[1] = '\0';
            Print(s);
            if (value[count] == ',' && gotVal == true && inStr == false)
            {
                gotVal = false;
                arraySize++;
            }
            else if (value[count] == ',' && gotVal == false && inStr == false)
            {
                char er[2];
                er[0] = value[count];
                er[1] = '\0';
                Print(" Got ");
                Print(er);
                e->error.isError = true;
                e->error.code = SYNTAX_ERROR;
                Print(". Value is missing in one of the array elements.\r\n");
                return;
            }
            if ((arrayType == null || arrayType == number) &&
                (value[count] >= '0' && value[count] <= '9'))
            {
                gotVal = true;
                arrayType = number;
            }
            else
            {
                if (arrayType == number && value[count] != ',' && value[count] != ']')
                {
                    char er[2];
                    er[0] = value[count];
                    er[1] = '\0';
                    Print(" Got ");
                    Print(er);
                    e->error.isError = true;
                    e->error.code = SYNTAX_ERROR;
                    Print(" in number. Value is missing in one of the array elements\r\nor value has a non numerical charactar.\r\n");
                    e->type = null;
                    return;
                }
            }
            if ((arrayType == null || arrayType == boolean) &&
                (value[count] == 't' || value[count] == 'T'))
            {
                if ((value[count] == 't' || value[count] == 'T') &&
                    (value[count + 1] == 'r' || value[count + 1] == 'R') &&
                    (value[count + 2] == 'u' || value[count + 2] == 'U') &&
                    (value[count + 3] == 'e' || value[count + 3] == 'E') &&
                    (value[count + 4] == ',' || LookAhead(value, count + 4) == ']'))
                {
                    gotVal = true;
                    arrayType = boolean;
                    isTrue = true;
                }
            }
            else if ((arrayType == null || arrayType == boolean) &&
                     (value[count] == 'f' || value[count] == 'F'))
            {
                if ((value[count] == 'f' || value[count] == 'F') &&
                    (value[count + 1] == 'a' || value[count + 1] == 'A') &&
                    (value[count + 2] == 'l' || value[count + 2] == 'L') &&
                    (value[count + 3] == 's' || value[count + 3] == 'S') &&
                    (value[count + 4] == 'e' || value[count + 4] == 'E') &&
                    (value[count + 5] == ',' || LookAhead(value, count + 5) == ']'))
                {
                    gotVal = true;
                    arrayType = boolean;
                    isFalse = true;
                }
            }
            else
            {
                if (arrayType == boolean && !isFalse && !isTrue &&
                    value[count] != ',' && value[count] != ']')
                {
                    char er[2];
                    er[0] = value[count];
                    er[1] = '\0';
                    Print(" Got ");
                    Print(er);
                    isFalse = false;
                    isTrue = false;
                    e->error.isError = true;
                    e->error.code = SYNTAX_ERROR;
                    Print(" in boolean. Value is missing in one of the array elements\r\nor value is a non boolean value.\r\n");
                    e->type = null;
                    return;
                }
            }
            if (value[count] == '"' && inStr == false && (arrayType == null || arrayType == string))
            {
                inStr = true;
                gotVal = true;
                arrayType = string;
            }
            else if (value[count] == '"' && arrayType == string && inStr == true)
            {
                inStr = false;
                gotVal = true;
            }
            else
            {
                if (arrayType == string && inStr == false && value[count] != ',' && value[count] != ']' && value[count] != '"')
                {
                    e->error.isError = true;
                    e->error.code = SYNTAX_ERROR;
                    Print("Value is outside the string element.\r\n");
                    e->type = null;
                    return;
                }
            }
        }
        count++;
    }
    if (arraySize == 0 && gotVal == false)
    {
        e->type = empty;
        return;
    }
    else if (arraySize > 0 && gotVal == false)
    {
        e->error.isError = true;
        e->error.code = SYNTAX_ERROR;
        Print("Value is missing after the last appearing comma ',' or closing brace ']'.");
        e->type = null;
        return;
    }
    Print(" ]");
    arraySize++;
    UINT64 typeSize = 0;
    if (arrayType == string)
        typeSize = sizeof(char **);
    else if (arrayType == boolean)
        typeSize = sizeof(BOOL);
    else if (arrayType == number)
        typeSize = sizeof(UINT64);
    e->object = AssignAndInitMemory(typeSize, arraySize);
    Print(" of an array ");
    if (e->object != NULL)
    {
        e->type = arrayType;
        e->arraySize = arraySize;
        char s[12];
        NumberToString(arraySize, s, DECIMAL);
        s[11] = '\0';
        Print("of size: ");
        Print(s);
        Print("\r\n[");
    }
    else
    {
        e->error.isError = true;
        e->error.code = MEMORY_CREATION_ERROR;
        Print("Can't allocate memory.\r\n");
    }
}

void SetJsonType(ObjectType type, JSON_Element *e, void *value)
{
    switch (type)
    {
    case string:
        if (e->isArray == false)
        {
            e->object = value;
            e->type = string;
        }
        break;
    case number:
        if (e->isArray == false)
        {
            e->object = AssignAndInitMemory(sizeof(UINT64), 1);
            if (e->object != NULL)
            {
                *(UINT64 *)e->object = *(UINT64 *)value;
                Print(" of a number that stores ");
                char s[12];
                NumberToString(*((UINT64 *)e->object), s, DECIMAL);
                s[11] = '\0';
                Print(s);
                e->type = number;
            }
            else
            {
                e->error.isError = true;
                e->error.code = MEMORY_CREATION_ERROR;
                Print("Can't allocate memory for number.\r\n");
            }
        }
        break;
    case boolean:
        if (e->isArray == false)
        {
            e->object = AssignAndInitMemory(sizeof(BOOL), 1);
            if (e->object != NULL)
            {
                *(BOOL *)e->object = *(BOOL *)value;
                Print(" of a boolean that stores ");
                if (*((BOOL *)e->object))
                    Print("True");
                else
                    Print("False");
                e->type = boolean;
            }
            else
            {
                e->error.isError = true;
                e->error.code = MEMORY_CREATION_ERROR;
                Print("Can't allocate memory for boolean.\r\n");
            }
        }
        break;
    case element:
        if (e->isArray == false)
        {
            e->object = value;
            ((JSON_Element *)e->object)->parent = (void *)e;
            e->type = element;
        }
        else
        {
            e->error.isError = true;
            e->error.code = SYNTAX_ERROR;
            Print("Can't have an array of elements.\r\n");
        }
        break;
    default:
        e->type = type;
        break;
    }
}

BOOL CheckStartingBracket(UINT8 *buffer, UINT64 size)
{
    for (UINT64 i = 0; i < size; i++)
    {
        if (buffer[i] != '{' && !IsWhitespace(buffer, i))
            return false;
        else
            break;
    }
    return true;
}

Error OpenBracketCheck(UINT8 *buffer, UINT64 index, JSON_Element **e, BOOL *strRecord, BOOL *keyMode, BOOL *start)
{
    if (buffer[index] == '{' && *strRecord == false && *keyMode == true && *start == false)
    {
        *start = true;
    }
    else if (buffer[index] == '{' && *strRecord == false && *keyMode == true && *start == true)
    {
        (*e)->error.isError = true;
        (*e)->error.code = SYNTAX_ERROR;
        Print("There is a syntax error in the file '{{'.\r\n");
        Print("Double brackets are not allowed ------^\r\n");
    }
    else if (buffer[index] == '{' && *strRecord == false && *keyMode == false && (*e)->type == null)
    {
        SetJsonType(element, *e, (void *)CreateElement());
        *keyMode = true;
        *e = (JSON_Element *)((*e)->object);
        Print("{\r\n");
    }
    return (*e)->error;
}

Error CloseBracketCheck(UINT8 *buffer, UINT64 index, JSON_Element **e, UINT64 size, BOOL *strRecord, BOOL *keyMode, BOOL *start)
{
    if (buffer[index] == '}' && *strRecord == false && *keyMode == true)
    {
        Print("}");
        if ((*e)->prev == NULL && ((*e)->name == NULL || compareString((const char *)(*e)->name, (const char *)"root") == true))
            (*e)->type = empty;
        else
        {
            (*e)->error.isError = true;
            (*e)->error.code = SYNTAX_ERROR;
            if ((*e)->prev != NULL)
                Print("Cannot end element with a ','.\r\n");
            if (((*e)->name != NULL && compareString((const char *)(*e)->name, (const char *)"root") != true))
                Print("':' is missing.\r\n");
        }
    }
    else if (buffer[index] == '}' && *strRecord == false && *keyMode == false)
    {
        Print("}");
        if ((*e)->parent == NULL && compareString((const char *)(*e)->name, (const char *)"root") == true)
        {
            if (index < size - 1)
            {
                for (UINT64 j = index + 1; j < size; j++)
                {
                    if (buffer[index] == '}')
                    {
                        (*e)->error.isError = true;
                        (*e)->error.code = SYNTAX_ERROR;
                        Print("JSON file has too many closed brackets '}'.\r\n");
                        Print("Check JSON file to make sure all closed brackets have an\r\nequal amount of open brackets.\r\n");
                        break;
                    }
                }
            }
        }
        else if ((*e)->parent != NULL)
        {
            *e = (JSON_Element *)(*e)->parent;
            Print(" ");
            Print((char *)(*e)->name);
            Print(" ");
        }
    }
    return (*e)->error;
}

Error CollonCheck(UINT8 *buffer, UINT64 index, JSON_Element **e, BOOL *strRecord, BOOL *keyMode, BOOL *start)
{
    if (buffer[index] == ':' && *strRecord == false && *keyMode == true && (*e)->name != NULL)
    {
        *keyMode = false;
        Print(":");
    }
    else if (buffer[index] == ':' && *strRecord == false && *keyMode == true && (*e)->name == NULL)
    {
        (*e)->error.isError = true;
        (*e)->error.code = SYNTAX_ERROR;
        Print("JSON Object must have a string as a key identifier.\r\n");
    }
    else if (buffer[index] == ':' && *strRecord == false && *keyMode == false)
    {
        (*e)->error.isError = true;
        (*e)->error.code = SYNTAX_ERROR;
        Print("':' cannot appear more than once per JSON element.\r\n");
    }
    return (*e)->error;
}

Error OpenBraceCheck(UINT8 *buffer, UINT64 index, JSON_Element **e, BOOL *strRecord, BOOL *keyMode, BOOL *start)
{
    if (buffer[index] == '[' && *strRecord == false && *keyMode == false && (*e)->object == NULL)
    {
        (*e)->isArray = true;
        SetupArray(*e, buffer, index);
    }
    else if (buffer[index] == '[' && *strRecord == false)
    {
        (*e)->error.isError = true;
        (*e)->error.code = SYNTAX_ERROR;
        Print("Cannot have an array as a key identifier.\r\n");
    }
    return (*e)->error;
}

Error ClosedBraceCheck(UINT8 *buffer, UINT64 index, JSON_Element **e, BOOL *strRecord, BOOL *keyMode, BOOL *start, UINT64 *arrayIndex)
{
    if (buffer[index] == ']')
    {
        if (*strRecord == false && *keyMode == true)
        {
            (*e)->error.isError = true;
            (*e)->error.code = SYNTAX_ERROR;
            Print("Invalid character in key identifier.\r\n");
        }
        else if (*strRecord == false && (*e)->object == NULL && (*e)->type != empty)
        {
            (*e)->error.isError = true;
            (*e)->error.code = SYNTAX_ERROR;
            char er[2];
            er[0] = buffer[index];
            er[1] = '\0';
            Print("Got ");
            Print(er);
            Print(". Expected an '['.\r\n");
        }
        else if (*arrayIndex == ((*e)->arraySize - 1) && (*e)->isArray == true)
        {
            if ((*e)->type == string)
                Print(((char **)(*e)->object)[*arrayIndex]);
            else if ((*e)->type == number)
            {
                char n[12];
                NumberToString(((UINT64 *)(*e)->object)[*arrayIndex], n, DECIMAL);
                n[11] = '\0';
                Print(n);
            }
            else if ((*e)->type == boolean)
            {
                if (((BOOL *)(*e)->object)[*arrayIndex])
                    Print("True");
                else
                    Print("False");
            }
            (*arrayIndex)++;
        }
        Print("]");
    }
    return (*e)->error;
}

Error CommaCheck(UINT8 *buffer, UINT64 index, JSON_Element **e, BOOL *strRecord, BOOL *keyMode, BOOL *start, UINT64 *arrayIndex)
{
    if (buffer[index] == ',')
    {
        if (*strRecord == false && *keyMode == false && (*e)->object != NULL && (*e)->isArray == false)
        {
            if (compareString((const char *)(*e)->name, (const char *)"root") != true)
            {
                (*e)->next = (void *)CreateElement();
                ((JSON_Element *)(*e)->next)->parent = ((JSON_Element *)((*e)->parent));
                ((JSON_Element *)((*e)->next))->prev = (void *)(*e);
                *e = (JSON_Element *)(*e)->next;
                *keyMode = true;
                Print(",\r\n");
            }
            else
            {
                (*e)->error.isError = true;
                (*e)->error.code = SYNTAX_ERROR;
                Print("Cannot have more than one root JSON Object.\r\n");
            }
        }
        else if (*strRecord == false && (*e)->type != null && (*e)->isArray == false)
        {
            (*e)->error.isError = true;
            (*e)->error.code = SYNTAX_ERROR;
            Print("Must have a key and a value before moving to next element.\r\n");
        }
        else if (*strRecord == false && (*e)->object != NULL && (*e)->isArray == true)
        {
            if (*arrayIndex < (*e)->arraySize)
            {
                if ((*e)->type == string)
                    Print(((char **)(*e)->object)[*arrayIndex]);
                else if ((*e)->type == number)
                {
                    char n[12];
                    NumberToString(((UINT64 *)(*e)->object)[*arrayIndex], n, DECIMAL);
                    n[11] = '\0';
                    Print(n);
                }
                else if ((*e)->type == boolean)
                {
                    if (((BOOL *)(*e)->object)[*arrayIndex])
                        Print("True");
                    else
                        Print("False");
                }
                (*arrayIndex)++;
                Print(", ");
            }
            else
            {
                (*e)->next = (void *)CreateElement();
                ((JSON_Element *)(*e)->next)->parent = ((JSON_Element *)((*e)->parent));
                ((JSON_Element *)((*e)->next))->prev = (void *)(*e);
                *e = (JSON_Element *)(*e)->next;
                *keyMode = true;
                *arrayIndex = 0;
                Print(",\r\n");
            }
        }
        else if (*strRecord == false && (*e)->object == NULL && (*e)->isArray == true)
        {
            (*e)->error.isError = true;
            (*e)->error.code = SYNTAX_ERROR;
            Print("Must have a value before moving to next array element.\r\n");
        }
    }
    return (*e)->error;
}

Error DoubleQuoteCheck(UINT8 *buffer, UINT64 index, JSON_Element **e, BOOL *strRecord, BOOL *keyMode, BOOL *start, UINT64 *strSize, UINT64 *arrayIndex)
{
    if (buffer[index] == '"' && *strRecord == false && ((*e)->object == NULL || (*e)->isArray == true))
    {
        *strRecord = true;
        *strSize = 0;
    }
    else if ((buffer[index] == '"' && buffer[index - 1] != '\\') && *strRecord == true && ((*e)->object == NULL || (*e)->isArray == true))
    {
        *strRecord = false;
        char *str;
        str = AssignAndInitMemory(sizeof(char), (*strSize + 1));
        Print(" of a string called ");
        if (str != NULL)
        {
            Print("\"");
            for (UINT64 j = *strSize; j > 0; j--)
            {
                str[*strSize - j] = buffer[index - j];
                char s[2];
                s[0] = str[*strSize - j];
                s[1] = '\0';
                if (s[0] == '%')
                    Print("%%");
                else
                    Print(s);
            }
            str[*strSize] = '\0';
            *strSize = 0;
            if (*keyMode == true)
                (*e)->name = str;
            else if ((*e)->isArray == true)
                ((char **)(*e)->object)[*arrayIndex] = str;
            else
                SetJsonType(string, *e, (void *)str);
            Print("\".");
        }
        else
        {
            (*e)->error.isError = true;
            (*e)->error.code = MEMORY_CREATION_ERROR;
            Print("Could not create memory allocation.\r\n");
        }
    }
    return (*e)->error;
}

BOOL ReadStringRecord(BOOL *strRecord, UINT64 *strSize)
{
    if (*strRecord == true)
    {
        (*strSize)++;
        return true;
    }
    return false;
}

BOOL ReadNumber(UINT8 *buffer, UINT64 index, JSON_Element **e, BOOL keyMode, UINT64 *num, UINT64 *arrayIndex, BOOL *strRecord)
{
    if ((buffer[index] >= '0' && buffer[index] <= '9') && keyMode == false)
    {
        if ((*e)->type == null)
            (*e)->type = number;
        *num += (buffer[index] - '0');
        if (buffer[index + 1] >= '0' && buffer[index + 1] <= '9')
            *num *= 10;
        else if ((LookAhead(buffer, index + 1) == ',' ||
                  LookAhead(buffer, index + 1) == '}') &&
                 (*e)->isArray == false)
        {
            SetJsonType(number, *e, (void *)num);
            *num = 0;
        }
        else if ((LookAhead(buffer, index + 1) == ',' ||
                  LookAhead(buffer, index + 1) == ']') &&
                 (*e)->isArray == true)
        {
            ((UINT64 *)(*e)->object)[*arrayIndex] = *num;
            *num = 0;
        }
        return true;
    }
    return false;
}

BOOL ReadBOOL(UINT8 *buffer, UINT64 *index, JSON_Element **e, UINT64 *arrayIndex)
{
    if ((buffer[*index] == 't' || buffer[*index] == 'T') && (buffer[*index + 1] == 'r' || buffer[*index + 1] == 'R') && (buffer[*index + 2] == 'u' || buffer[*index + 2] == 'U') && (buffer[*index + 3] == 'e' || buffer[*index + 3] == 'E'))
    {
        if ((*e)->isArray == false)
        {
            BOOL isTrue = true;
            SetJsonType(boolean, *e, &isTrue);
        }
        else
            ((BOOL *)(*e)->object)[*arrayIndex] = true;
        *index += 3;
        return true;
    }
    else if ((buffer[*index] == 'f' || buffer[*index] == 'F') && (buffer[*index + 1] == 'a' || buffer[*index + 1] == 'A') && (buffer[*index + 2] == 'l' || buffer[*index + 2] == 'L') && (buffer[*index + 3] == 's' || buffer[*index + 3] == 'S') && (buffer[*index + 4] == 'e' || buffer[*index + 4] == 'E'))
    {
        if ((*e)->isArray == false)
        {
            BOOL isFalse = false;
            SetJsonType(boolean, *e, &isFalse);
        }
        else
            ((BOOL *)(*e)->object)[*arrayIndex] = false;
        *index += 4;
        return true;
    }
    return false;
}

Error BuildElement(UINT8 *buffer, UINT64 *index, UINT64 size, JSON_Element **e, BOOL *strRecord, BOOL *keyMode, BOOL *start, UINT64 *strSize, UINT64 *num, UINT64 *arrayIndex)
{
    if (IsTokenCharacter(buffer, *index, *strRecord))
    {
        if (OpenBracketCheck(buffer, *index, e, strRecord, keyMode, start).isError)
            return (*e)->error;
        else if (CloseBracketCheck(buffer, *index, e, size, strRecord, keyMode, start).isError)
            return (*e)->error;
        else if (CollonCheck(buffer, *index, e, strRecord, keyMode, start).isError)
            return (*e)->error;
        else if (OpenBraceCheck(buffer, *index, e, strRecord, keyMode, start).isError)
            return (*e)->error;
        else if (ClosedBraceCheck(buffer, *index, e, strRecord, keyMode, start, arrayIndex).isError)
            return (*e)->error;
        else if (CommaCheck(buffer, *index, e, strRecord, keyMode, start, arrayIndex).isError)
            return (*e)->error;
        else if (DoubleQuoteCheck(buffer, *index, e, strRecord, keyMode, start, strSize, arrayIndex).isError)
            return (*e)->error;
    }
    else
    {
        BOOL excluded = IsWhitespace(buffer, *index);
        BOOL read = false; // Refers to the past tense of the word.
        if (!(*e)->error.isError)
            read = ReadStringRecord(strRecord, strSize) ||
                   ReadNumber(buffer, *index, e, *keyMode, num, arrayIndex, strRecord) ||
                   ReadBOOL(buffer, index, e, arrayIndex);
        if ((*e)->object == NULL && !excluded && read == false)
        {
            char er[2];
            er[0] = buffer[*index];
            er[1] = '\0';
            Print("Got ");
            Print(er);
            (*e)->error.isError = true;
            (*e)->error.code = SYNTAX_ERROR;
            Print(" and expected a string, number, or boolean.\r\n");
        }
        else if ((*e)->object != NULL && !excluded && !read && compareString((const char *)(*e)->name, (const char *)"root") != 1)
        {
            char er[2];
            er[0] = buffer[*index];
            er[1] = '\0';
            Print("Got ");
            Print(er);
            // char n[12];
            // itoa(*index, n, DECIMAL);
            // n[11] = '\0';
            // Print(" ");
            // Print(n);
            // itoa(size, n, DECIMAL);
            // n[11] = '\0';
            // Print(" ");
            // Print(n);
            (*e)->error.isError = true;
            (*e)->error.code = SYNTAX_ERROR;
            Print(" and expected a ',' after value.\r\n");
        }
    }
    return (*e)->error;
}

Error JsonBuilder(UINT8 *fileBuffer, UINT64 index, UINT64 size, JSON_Element *e)
{
    JSON_Element *root = e;
    BOOL start = false;
    BOOL keyMode = true; // When it is false, then it is in value mode.
    BOOL strRecord = false;
    UINT64 strSize = 0;
    UINT64 num = 0;
    UINT64 arrayIndex = 0;
    UINT8 *buffer = fileBuffer;
    if (!CheckStartingBracket(buffer, size))
    {
        root->error.isError = true;
        root->error.code = SYNTAX_ERROR;
        Print("JSON file did not start with an open bracket '{'\r\n");
        // Print("or there are other objects outside the root JSON object.\r\n");
        return root->error;
    }
    JSON_Element *item = ((JSON_Element *)root->object);
    for (UINT64 i = index; i < size && buffer[i] != '\0'; i++)
    {
        // char BSIZE[12];
        // NumberToString(buffer[i], BSIZE, HEX);
        // Print(BSIZE);
        // Print(" ");
        // char n[12];
        // NumberToString(i, n, DECIMAL);
        // n[11] = '\0';
        // Print(n);
        // Print(", ");
        Error error = BuildElement(buffer, &i, size, &item, &strRecord, &keyMode, &start, &strSize, &num, &arrayIndex);
        if (error.isError)
        {
            root->error.isError = true;
            root->error.code = error.code;
            break;
        }
    }
    if (!root->error.isError && compareString((const char *)item->name, (const char *)"root") != 1)
    {
        Print(" Item name:");
        Print(item->name);
        root->error.isError = true;
        root->error.code = SYNTAX_ERROR;
        Print(" JSON file did not end with a closed bracket '}'.\r\n");
        Print("Check JSON file to make sure all open brackets have an\r\nequal amount of closed brackets.\r\n");
    }
    return root->error;
}

JSON_Element *BuildJSONFromFile(char *file)
{
    if (file == NULL)
        Print("JSON file name was missing.\r\n");
    else
    {
        Print("\r\nJSON Builder started.\r\n");
        JSON_Element *root = CreateElement();
        root->name = AssignAndInitMemory(sizeof(UINT8), 5);
        Print(" name called root.");
        if (root->name != NULL)
        {
            memorycopy((void *)root->name, (const void *)"root", 5);
            root->isArray = false;
            SetJsonType(element, root, (void *)CreateElement());
            UINT64 size = 0;
            UINT8 *jsonstring = OpenAndWriteFileToMemory(file, &size);
            char n[12];
            NumberToString(size, n, DECIMAL);
            n[11] = '\0';
            Print("\r\nFile is of size ");
            Print(n);
            Print(" Bytes\r\n");
            if (!JsonBuilder(jsonstring, 0, size, root).isError)
                root->type = element;
            else
            {
                root->type = null;
                Print("Failed to parse JSON.");
            }
            return root;
        }
        Print("Failed to allocate memory.\r\n");
        Print("This is not good. I think you need some memory.\r\nDoes the memory management work?\r\n");
        root->name = NULL;
        root->object = NULL;
        root->type = null;
        root->next = NULL;
        root->parent = NULL;
        root->prev = NULL;
        root->error.isError = true;
        root->error.code = FILE_READ_ERROR;
        return root;
    }
    return NULL;
}

void FreeJSONObjectFromMemory(JSON_Element *root)
{
    if (compareString(root->name, "root"))
    {
        Print("\r\nFreeing Memory from JSON.");
        JSON_Element *e = (JSON_Element *)root->object;
        while (root->object != NULL || root->type != empty)
        {
            Print("\r\nName ");
            if (e->name == NULL)
                Print("is Null for some reason.");
            else
                Print(e->name);
            Print("\r\n");
            if (e->type == element)
            {
                if (e->object == NULL)
                    Print("Object is Null for some reason.\r\n");
                else
                    e = (JSON_Element *)e->object;
            }
            else if (e->type != null)
            {
                Print("Freeing ");
                Print(e->name);
                Print(" of type ");
                if (e->type == boolean)
                    Print("Boolean");
                if (e->type == number)
                    Print("Number");
                if (e->type == string)
                    Print("String");
                if (e->type == empty)
                    Print("Empty");
                if (e->isArray && e->type != string)
                    Print(" array");
                if (e->isArray && e->type == string)
                {
                    Print(" array that says ");
                    for (long i = 0; i < e->arraySize; i++)
                    {
                        Print(((char **)e->object)[i]);
                        FreeMemory(((char **)e->object)[i]);
                        ((char **)e->object)[i] = NULL;
                    }
                }
                else if (e->type == string)
                {
                    Print(" that says ");
                    Print(((char *)e->object));
                }
                Print(".\r\n");
                FreeMemory(e->object);
                e->object = NULL;
                Print(" object.");
                FreeMemory(e->name);
                Print(" name.");
                e->name = NULL;
                if (e->next != NULL)
                {
                    e = (JSON_Element *)e->next;
                    FreeMemory((JSON_Element *)e->prev);
                    Print(" previous element.");
                    e->prev = NULL;
                }
                else if (e->parent != NULL)
                {
                    ((JSON_Element *)e->parent)->object = e;
                    e = (JSON_Element *)e->parent;
                    e->type = empty;
                }
            }
        }
        FreeMemory(e);
        Print(" root.");
        Print("\r\nJSON Memory cleared.");
    }
}