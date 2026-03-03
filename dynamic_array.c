#include "dynamic_array.h"
#include <string.h>

void    da_free(t_da *da)
{
        free(da->array);
}

bool    da_append(t_da *da, void *value)
{
        void     *new_array;
        void     *target;

        if (da->count >= da->capacity)
        {
                if (da->capacity == 0)
                        da->capacity = 256;
                else
                        da->capacity *= 2;
                new_array = realloc(da->array, da->capacity * da->item_size);
                if (!new_array)
                        return (false);
                da->array = new_array;
        }
        target = (char *)da->array + (da->count * da->item_size);
        memcpy(target, value, da->item_size);
        da->count++;
        return (true);
}

void    *da_get(t_da *da, int index)
{
        if (index < da->count)
                return ((char *)da->array + (index * da->item_size));
        else
        {
                printf("Accessing index [%d] outside of array bounds", index);
                exit(1);
        }
        return NULL;
}

void     da_set(t_da *da, int index, void *value)
{
        void     *target;

        if (index >= da->count)
        {
                printf("Accessing index [%d] outside of array bounds", index);
                exit(1);
        }
        target = (char *)da->array + (index * da->item_size);
        memcpy(target, value, da->item_size);
}

bool     da_insert(t_da *da, int index, void *value)
{
        void    *new_array;
        void    *target;
        void    *src;
        void    *dst;
        int     elements_to_move;

        if (index > da->count)
        {
                printf("Accessing index [%d] outside of array bounds", index);
                exit(1);
        }
        if (da->count >= da->capacity)
        {
                if (da->capacity == 0)
                        da->capacity = 256;
                else
                        da->capacity *= 2;
                new_array = realloc(da->array, da->capacity * da->item_size);
                if (!new_array)
                        return (false);
                da->array = new_array;
        }
        elements_to_move = da->count - index;
        src = (char *)da->array + (index * da->item_size);
        dst = (char *)da->array + ((index + 1) * da->item_size);
        if (elements_to_move > 0)
                memmove(dst, src, elements_to_move * da->item_size);
        target = (char *)da->array + (index * da->item_size);
        memcpy(target, value, da->item_size);
        da->count++;
        return (true);
}

void     da_pop(t_da *da, int index)
{
        int     elements_to_move;
        void    *src;
        void    *dst;

        if (index >= da->count)
        {
                printf("Accessing index [%d] outside of array bounds", index);
                exit(1);
        }
        elements_to_move = da->count - index - 1;
        src = (char *)da->array + ((index + 1) * da->item_size);
        dst = (char *)da->array + (index * da->item_size);
        if (elements_to_move > 0)
                memmove(dst, src, elements_to_move * da->item_size);
        da->count--;
}

void    print_da(t_da *da, char *type)
{
        if (da->count == 0)
        {
                printf("[]\n");
                return;
        }

        printf("[");
        for (size_t i = 0; i < da->count; i++)
        {
                void *item = da_get(da, i);

                if (!strcmp(type, "int"))
                        printf("%d", *(int *)item);
                else if (!strcmp(type, "char"))
                        printf("'%c'", *(char *)item);
                else if (!strcmp(type, "str") || !strcmp(type, "char*"))
                        printf("\"%s\"", *(char **)item);
                else if (!strcmp(type, "float"))
                        printf("%f", *(float *)item);
                else if (!strcmp(type, "double"))
                        printf("%lf", *(double *)item);
                else if (!strcmp(type, "size_t"))
                        printf("%zu", *(size_t *)item);
                else if (!strcmp(type, "long"))
                        printf("%ld", *(long *)item);
                else if (!strcmp(type, "short"))
                        printf("%hd", *(short *)item);
                else if (!strcmp(type, "ptr"))
                        printf("%p", *(void **)item);
                else
                        printf("?");
                if (i < da->count - 1)
                        printf(", ");
        }
        printf("]\n");
}

void    da_init(t_da *da, size_t item_size)
{
        da->array = NULL;
        da->count = 0;
        da->capacity = 0;
        da->item_size = item_size;
}

int     main(int argc, char *argv[])
{
        t_da    da;
        int     value;

        if (argc < 2)
                return EXIT_FAILURE;
        da_init(&da, sizeof(int));
        for (int i = 1; i < argc; i++)
        {
                value = atoi(argv[i]);
                if (!da_append(&da, &value))
                        return EXIT_FAILURE;
        }
        int     num = 420;
        print_da(&da, "int");
        da_insert(&da, 9, &num);
        print_da(&da, "int");
        da_pop(&da, 0);
        print_da(&da, "int");
        da_free(&da);
        return EXIT_SUCCESS;
}
