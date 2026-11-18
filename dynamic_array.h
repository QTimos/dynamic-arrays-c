#ifndef DA_H
#define DA_H
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

struct sDynamicArray;

typedef struct sDynamicArrayVTable {
	bool (*da_insert)(struct sDynamicArray *da, size_t index, void *value);
	bool (*da_append)(struct sDynamicArray *da, void *value);
	void *(*da_get)(struct sDynamicArray *da, size_t index);
	bool (*da_set)(struct sDynamicArray *da, size_t index, void *value);
	bool (*da_pop)(struct sDynamicArray *da, size_t index, void *buf);
	void (*da_print)(struct sDynamicArray *da);
} tDynamicArrayVTable;

typedef struct sDynamicArray
{
	void *array;
	size_t count;
	size_t max;
	size_t capacity;
	size_t item_size;
	const char *type;
	const tDynamicArrayVTable *vtable;
} tDynamicArray;

bool da_insert(tDynamicArray *da, size_t index, void *value);
bool da_append(tDynamicArray *da, void *value);
void *da_get(tDynamicArray *da, size_t index);
bool da_set(tDynamicArray *da, size_t index, void *value);
bool da_pop(tDynamicArray *da, size_t index, void *buf);
void da_print(tDynamicArray *da);


tDynamicArray *da_create(size_t item_size, const char *type);
void da_free(tDynamicArray *da);


typedef enum {
	SUCCESS = 0,
	INDEX_OOB_ERROR = 1,
	CREATION_ERROR = 2,
	RESIZE_ERROR = 3
} DAerrors;
const char* get_error_msg(void);
static inline int8_t* get_err(void);
#define _DAERROR (get_err())
#define DAERROR *_DAERROR

#define DA_INSERT(a, i, v) (a->vtable->da_insert(a, i, v))
#define DA_APPEND(a, v) (a->vtable->da_append(a, v))
#define DA_GET(a, i) (a->vtable->da_get(a, i))
#define DA_SET(a, i, v) (a->vtable->da_set(a, i, v))
#define DA_POP(a, i, b) (a->vtable->da_pop(a, i, b))
#define DA_PRINT(a) (a->vtable->da_print(a))

#define DA_TYPE(T)    sizeof(T)
#define DA_PTR        sizeof(void *)
#define DA_SIZE       sizeof(size_t)

#define DA_CHAR       sizeof(char)
#define DA_STR        sizeof(char)
#define DA_BOOL       sizeof(bool)

#define DA_SHORT      sizeof(short)
#define DA_USHORT     sizeof(unsigned short)
#define DA_INT        sizeof(int)
#define DA_UINT       sizeof(unsigned int)
#define DA_LONG       sizeof(long)
#define DA_ULONG      sizeof(unsigned long)
#define DA_LLONG      sizeof(long long)
#define DA_ULLONG     sizeof(unsigned long long)

#define DA_FLOAT      sizeof(float)
#define DA_DOUBLE     sizeof(double)

#define DA_I8         sizeof(int8_t)
#define DA_I16        sizeof(int16_t)
#define DA_I32        sizeof(int32_t)
#define DA_I64        sizeof(int64_t)
#define DA_U8         sizeof(uint8_t)
#define DA_U16        sizeof(uint16_t)
#define DA_U32        sizeof(uint32_t)
#define DA_U64        sizeof(uint64_t)

/* Cast Helpers */
#define CAST_GEN(x)     ((void *)(x))
#define CAST_AS(T, x)   (*(T *)(x))
#define CAST_PTR(T, x)  ((T *)(x))

#define CAST_STR(x)     ((char *)(x))
#define CAST_CHAR(x)    (*(char *)(x))
#define CAST_BOOL(x)    (*(bool *)(x))
#define CAST_SIZE(x)    (*(size_t *)(x))

#define CAST_INT(x)     (*(int *)(x))
#define CAST_UINT(x)    (*(unsigned int *)(x))
#define CAST_LONG(x)    (*(long *)(x))
#define CAST_LLONG(x)   (*(long long *)(x))
#define CAST_INT_ARR(x) ((int *)(x))

#define CAST_FLOAT(x)   (*(float *)(x))
#define CAST_DOUBLE(x)  (*(double *)(x))


#ifdef DA_IMPLEMENTATION

static inline int8_t* get_err(void)
{
	static int8_t err = 0;
	return &err;
}

void da_free(tDynamicArray *da)
{
	if (!da) return;
	free(da->array);
	da->array = NULL;
	free(da);
	da = NULL;
}

tDynamicArray *da_create(size_t item_size, const char *type)
{
	if (!item_size)
	{
		int8_t *err = _DAERROR;
		*err = CREATION_ERROR;
		return NULL;
	}
	tDynamicArray *da = malloc(sizeof(tDynamicArray));
	if (!da)
	{
		int8_t *err = _DAERROR;
		*err = CREATION_ERROR;
		return NULL;
	}
	da->array = NULL;
	da->count = 0;
	da->capacity = 0;
	da->max = 0;
	da->item_size = item_size;
	da->type = type;
	static const tDynamicArrayVTable vtable = {da_insert, da_append, da_get, da_set, da_pop, da_print};
	da->vtable = &vtable;
	return da;
}

bool da_append(tDynamicArray *da, void *value)
{
	if (!da || !value) return false;
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
		{
			int8_t *err = _DAERROR;
			*err = RESIZE_ERROR;
			return (false);
		}
		da->array = new_array;
	}
	target = (char *)da->array + (da->count * da->item_size);
	memcpy(target, value, da->item_size);
	da->count++;
	da->max++;
	return (true);
}

void *da_get(tDynamicArray *da, size_t index)
{
	if (!da) return NULL;
	if (index >= da->count)
	{
			int8_t *err = _DAERROR;
			*err = INDEX_OOB_ERROR;
			return NULL;
	}
	else
		return ((uint8_t *)da->array + (index * da->item_size));
}

bool da_set(tDynamicArray *da, size_t index, void *value)
{
	if (!da || !value) return false;
	void     *target;

	if (index >= da->count)
	{
		int8_t *err = _DAERROR;
		*err = INDEX_OOB_ERROR;
		return false;
	}
	target = (char *)da->array + (index * da->item_size);
	memcpy(target, value, da->item_size);
	return true;
}

bool da_insert(tDynamicArray *da, size_t index, void *value)
{
	if (!da || !value) return false;
	void    *new_array;
	void    *target;
	void    *src;
	void    *dst;
	size_t     elements_to_move;

	if (index > da->count)
	{
		int8_t *err = _DAERROR;
		*err = INDEX_OOB_ERROR;
		return false;
	}
	if (da->count >= da->capacity)
	{
		size_t cap_tmp = da->capacity;
		if (cap_tmp == 0)
			cap_tmp = 256;
		else
			cap_tmp *= 2;
		new_array = realloc(da->array, da->capacity * da->item_size);
		if (!new_array)
		{
			int8_t *err = _DAERROR;
			*err = RESIZE_ERROR;
			return (false);
		}
		da->capacity = cap_tmp;
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
	da->max++;
	return (true);
}

bool da_pop(tDynamicArray *da, size_t index, void *buf)
{
	if (!da || !buf) return false;
	size_t elements_to_move;
	void *src;
	void *dst;

	if (index >= da->count)
	{
		int8_t *err = _DAERROR;
		*err = INDEX_OOB_ERROR;
		return false;
	}
	if (buf != NULL) memcpy(buf, ((uint8_t *)da->array + (index * da->item_size)), da->item_size);
	elements_to_move = da->count - index - 1;
	src = (uint8_t *)da->array + ((index + 1) * da->item_size);
	dst = (uint8_t *)da->array + (index * da->item_size);
	if (elements_to_move > 0)
		memmove(dst, src, elements_to_move * da->item_size);
	da->count--;
	return true;
}

void da_print(tDynamicArray *da)
{
	if (!da) return;
	if (!da->type)
	{
		printf("Can't print dynamic array as there is no type specified.");
		return;
	}
	if (da->count == 0)
	{
		printf("[]\n");
		return;
	}
	const char *type = da->type;
	printf("[");
	for (size_t i = 0; i < da->count; i++)
	{

		void *item = da_get(da, i);
		if (!strcmp(type, "int"))
			printf("%d", *(int *)item);
		else if (!strcmp(type, "unsigned int"))
			printf("%u", *(unsigned int *)item);
		else if (!strcmp(type, "bool"))
		{
			if (!(*(bool *)item))
				printf("'False'");
			else printf("'True'");
		}
		else if (!strcmp(type, "char"))
			printf("'%c'", *(char *)item);
		else if (!strcmp(type, "str"))
			printf("\"%s\"", *(char **)item);
		else if (!strcmp(type, "float"))
			printf("%f", *(float *)item);
		else if (!strcmp(type, "double"))
			printf("%lf", *(double *)item);
		else if (!strcmp(type, "size_t"))
			printf("%zu", *(size_t *)item);
		else if (!strcmp(type, "long"))
			printf("%ld", *(long *)item);
		else if (!strcmp(type, "unsigned long"))
			printf("%lu", *(unsigned long *)item);
		else if (!strcmp(type, "long long"))
			printf("%ll", *(long long *)item);
		else if (!strcmp(type, "unsigned long long"))
			printf("%llu", *(unsigned long long *)item);
		else if (!strcmp(type, "short"))
			printf("%hd", *(short *)item);
		else if (!strcmp(type, "unsigned short"))
			printf("%hu", *(unsigned short *)item);
		else if (!strcmp(type, "ptr"))
			printf("%p", *(void **)item);
		else
		{
			printf("Unknown type!!");
			break;
		}
		if (i < da->count - 1)
			printf(", ");
	}
	printf("]\n");
}

const char* get_error_msg(void)
{
	int8_t *error = _DAERROR;

	switch (*error)
	{
		case SUCCESS: return ("Success");
		case INDEX_OOB_ERROR: return ("Accessing index outside of array bounds");
		case CREATION_ERROR: return ("Failed to create dynamic array");
		case RESIZE_ERROR: return ("Failed to resize dyanamic array");
		default: return ("Unknown internal system error");
	}
}
#endif

#endif
