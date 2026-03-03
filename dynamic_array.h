#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef struct  s_da
{
        void     *array;
        size_t  count;
        size_t  capacity;
        size_t  item_size;
}       t_da;

void    da_free(t_da *da);
bool    da_append(t_da *da, void *value);
void    *da_get(t_da *da, int index);
void     da_set(t_da *da, int index, void *value);
void     da_pop(t_da *da, int index);
void    print_da(t_da *da, char *type);
void    da_init(t_da *da, size_t item_size);
