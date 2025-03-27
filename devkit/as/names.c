#include "as.h"

char _names[NAMES_MAX];
int _names_next = 0;

char *add_name(char *name)
{
    int i;
    char *ptr;
    for(i = 0; i < _names_next; i += strlen(&_names[i]) + 1)
    {
        if(!strcmp(&_names[i], name))
        {
            return &_names[i];
        }
    }
    ptr = &_names[_names_next];
    _names_next += strlen(name) + 1;
    if(_names_next >= NAMES_MAX)
    {
        error_at(curr(), "name list overflow: %s", name);
    }
    strcpy(ptr, name);
    return ptr;
}
