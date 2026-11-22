#ifndef DA_H
#define DA_H
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

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
	size_t capacity;
	size_t item_size;
	char *type;
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
	RESIZE_ERROR = 3,
	NULL_ARRAY_ERROR = 4,
	NULL_VALUE_ERROR = 5,
	UNKNOWN_TYPE_ERROR = 6
} DAerrors;

/*
 * get_error_msg reads the error state of the most recent da_* call.
 * Every public da_* function resets the slot to SUCCESS on entry and sets
 * it to a specific code on failure, so the slot describes "this call"
 * without the caller needing to clear it manually.
 *
 * The slot is per-thread when the compiler supports thread-local storage
 * (see DA_THREAD_SAFE_ERROR_STATE). When that flag is 0, the slot is a
 * process-global static and these reads are NOT thread-safe.
 */
const char* get_error_msg(void);

#define DA_INSERT(a, i, v) ((a) ? (a)->vtable->da_insert((a), (i), (v)) : false)
#define DA_APPEND(a, v) ((a) ? (a)->vtable->da_append((a), (v)) : false)
#define DA_GET(a, i) ((a) ? (a)->vtable->da_get((a), (i)) : NULL)
#define DA_SET(a, i, v) ((a) ? (a)->vtable->da_set((a), (i), (v)) : false)
#define DA_POP(a, i, b) ((a) ? (a)->vtable->da_pop((a), (i), (b)) : false)
#define DA_PRINT(a) ((a) ? (a)->vtable->da_print(a) : (void)0)

#define DA_TYPE(T)    sizeof(T)
#define DA_PTR        sizeof(void *)
#define DA_SIZE       sizeof(size_t)

#define DA_CHAR       sizeof(char)
#define DA_STR        sizeof(char *)
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

#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define DA_THREAD_LOCAL _Thread_local
#  define DA_THREAD_SAFE_ERROR_STATE 1
#elif defined(__GNUC__)
#  define DA_THREAD_LOCAL __thread
#  define DA_THREAD_SAFE_ERROR_STATE 1
#elif defined(_MSC_VER)
#  define DA_THREAD_LOCAL __declspec(thread)
#  define DA_THREAD_SAFE_ERROR_STATE 1
#else
#  define DA_THREAD_LOCAL /* empty / not thread safe branch */
#  define DA_THREAD_SAFE_ERROR_STATE 0
#endif

#if !DA_THREAD_SAFE_ERROR_STATE && !defined(DA_ACCEPT_UNSAFE_THREAD_STATE)
#  if defined(__GNUC__) || defined(__clang__) || \
      (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
#    warning "DA: error state is not thread-local on this toolchain (not thread-safe). Define DA_ACCEPT_UNSAFE_THREAD_STATE to silence."
#  elif defined(_MSC_VER)
#    pragma message("DA: error state is not thread-local on this toolchain (not thread-safe). Define DA_ACCEPT_UNSAFE_THREAD_STATE to silence.")
#  endif
#endif


#ifdef DA_IMPLEMENTATION
static char *da_strdup(const char *s)
{
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (!p) return NULL;
    memcpy(p, s, n);
    return p;
}

static DA_THREAD_LOCAL int8_t da_err_state = 0;

static inline int8_t* get_err(void)
{
	return &da_err_state;
}
#define _DAERROR (get_err())
#define DAERROR *_DAERROR

void da_free(tDynamicArray *da)
{
    *(_DAERROR) = SUCCESS;
	if (!da)
	{
		*(_DAERROR) = NULL_ARRAY_ERROR;
		return;
	}
	free(da->array);
	free(da->type);
	free(da);
}

tDynamicArray *da_create(size_t item_size, const char *type)
{
    *(_DAERROR) = SUCCESS;
	if (!item_size)
	{
		*(_DAERROR) = CREATION_ERROR;
		return NULL;
	}
	tDynamicArray *da = malloc(sizeof(tDynamicArray));
	if (!da)
	{
		*(_DAERROR) = CREATION_ERROR;
		return NULL;
	}
	da->array = NULL;
	da->count = 0;
	da->capacity = 0;
	da->item_size = item_size;
	if (!type) da->type = NULL;
	else
	{
		da->type = da_strdup(type);
		if (!da->type)
		{
			*(_DAERROR) = CREATION_ERROR;
			free(da);
			return NULL;
		}
	}
	static const tDynamicArrayVTable vtable = {da_insert, da_append, da_get, da_set, da_pop, da_print};
	da->vtable = &vtable;
	return da;
}

bool da_append(tDynamicArray *da, void *value)
{
    *(_DAERROR) = SUCCESS;
	if (!da)
	{
		*(_DAERROR) = NULL_ARRAY_ERROR;
		return false;
	}
	if (!value)
	{
		*(_DAERROR) = NULL_VALUE_ERROR;
		return false;
	}
	void     *new_array;
	void     *target;
	void     *value_p;
	bool    aliased;

	aliased = (da->array &&
		(uintptr_t)value >= (uintptr_t)da->array &&
		(uintptr_t)value <  (uintptr_t)da->array + da->count * da->item_size);
	if (aliased)
	{
		value_p = (void *) malloc(da->item_size);
		if (!value_p)
		{
			*(_DAERROR) = RESIZE_ERROR;
			return false;
		}
		memcpy(value_p, value, da->item_size);
	}
	else value_p = value;
	if (da->count >= da->capacity)
	{
		size_t cap_tmp = da->capacity;
		if (cap_tmp == 0)
			cap_tmp = 256;
		else if (cap_tmp > SIZE_MAX / 2)
		{
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return (false);
		}
		else
			cap_tmp *= 2;
		if (cap_tmp > SIZE_MAX / da->item_size)
		{
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return (false);
		}
		new_array = realloc(da->array, cap_tmp * da->item_size);
		if (!new_array)
		{
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return (false);
		}
		da->capacity = cap_tmp;
		da->array = new_array;
	}
	target = (char *)da->array + (da->count * da->item_size);
	memcpy(target, value_p, da->item_size);
	if (aliased) free(value_p);
	da->count++;
	return (true);
}

void *da_get(tDynamicArray *da, size_t index)
{
    *(_DAERROR) = SUCCESS;
	if (!da)
	{
		*(_DAERROR) = NULL_ARRAY_ERROR;
		return NULL;
	}
	if (index >= da->count)
	{
			*(_DAERROR) = INDEX_OOB_ERROR;
			return NULL;
	}
	else
		return ((uint8_t *)da->array + (index * da->item_size));
}

bool da_set(tDynamicArray *da, size_t index, void *value)
{
    *(_DAERROR) = SUCCESS;
	if (!da)
	{
		*(_DAERROR) = NULL_ARRAY_ERROR;
		return false;
	}
	if (!value)
	{
		*(_DAERROR) = NULL_VALUE_ERROR;
		return false;
	}
	void     *target;

	if (index >= da->count)
	{
		*(_DAERROR) = INDEX_OOB_ERROR;
		return false;
	}
	target = (char *)da->array + (index * da->item_size);
	if (target == value) return true;
	memcpy(target, value, da->item_size);
	return true;
}

bool da_insert(tDynamicArray *da, size_t index, void *value)
{
	*(_DAERROR) = SUCCESS;
	if (!da)
	{
		*(_DAERROR) = NULL_ARRAY_ERROR;
		return false;
	}
	if (!value)
	{
		*(_DAERROR) = NULL_VALUE_ERROR;
		return false;
	}
	void    *new_array;
	void    *target;
	void    *src;
	void    *dst;
	void    *value_p;
	size_t     elements_to_move;
	bool    aliased;

	aliased = (da->array &&
		(uintptr_t)value >= (uintptr_t)da->array &&
		(uintptr_t)value <  (uintptr_t)da->array + da->count * da->item_size);
	if (aliased)
	{
		value_p = (void *) malloc(da->item_size);
		if (!value_p)
		{
			*(_DAERROR) = RESIZE_ERROR;
			return false;
		}
		memcpy(value_p, value, da->item_size);
	}
	else value_p = value;
	if (index > da->count)
	{
		if (aliased) free(value_p);
		*(_DAERROR) = INDEX_OOB_ERROR;
		return false;
	}
	if (da->count >= da->capacity)
	{
		size_t cap_tmp = da->capacity;
		if (cap_tmp == 0)
			cap_tmp = 256;
		else if (cap_tmp > SIZE_MAX / 2)
		{
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return (false);
		}
		else
			cap_tmp *= 2;
		if (cap_tmp > SIZE_MAX / da->item_size)
		{
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return (false);
		}
		new_array = realloc(da->array, cap_tmp * da->item_size);
		if (!new_array)
		{
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
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
	memcpy(target, value_p, da->item_size);
	if (aliased) free(value_p);
	da->count++;
	return (true);
}

bool da_pop(tDynamicArray *da, size_t index, void *buf)
{
    *(_DAERROR) = SUCCESS;
	if (!da)
	{
		*(_DAERROR) = NULL_ARRAY_ERROR;
		return false;
	}
	size_t elements_to_move;
	void *src;
	void *dst;

	if (index >= da->count)
	{
		*(_DAERROR) = INDEX_OOB_ERROR;
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
	*(_DAERROR) = SUCCESS;
	if (!da)
	{
		*(_DAERROR) = NULL_ARRAY_ERROR;
		return;
	}
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

		void *item = (uint8_t *)da->array + (i * da->item_size);
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
			printf("\"%s\"", *(char **)item ? *(char **)item : "(null)");
		else if (!strcmp(type, "float"))
			printf("%f", *(float *)item);
		else if (!strcmp(type, "double"))
			printf("%f", *(double *)item);
		else if (!strcmp(type, "size_t"))
			printf("%zu", *(size_t *)item);
		else if (!strcmp(type, "long"))
			printf("%ld", *(long *)item);
		else if (!strcmp(type, "unsigned long"))
			printf("%lu", *(unsigned long *)item);
		else if (!strcmp(type, "long long"))
			printf("%lld", *(long long *)item);
		else if (!strcmp(type, "unsigned long long"))
			printf("%llu", *(unsigned long long *)item);
		else if (!strcmp(type, "short"))
			printf("%hd", *(short *)item);
		else if (!strcmp(type, "unsigned short"))
			printf("%hu", *(unsigned short *)item);
		else if (!strcmp(type, "ptr"))
			printf("%p", *(void **)item);
		else if (!strcmp(type, "int8_t"))
			printf("%" PRId8, *(int8_t *)item);
		else if (!strcmp(type, "int16_t"))
			printf("%" PRId16, *(int16_t *)item);
		else if (!strcmp(type, "int32_t"))
			printf("%" PRId32, *(int32_t *)item);
		else if (!strcmp(type, "int64_t"))
			printf("%" PRId64, *(int64_t *)item);
		else if (!strcmp(type, "uint8_t"))
			printf("%" PRIu8, *(uint8_t *)item);
		else if (!strcmp(type, "uint16_t"))
			printf("%" PRIu16, *(uint16_t *)item);
		else if (!strcmp(type, "uint32_t"))
			printf("%" PRIu32, *(uint32_t *)item);
		else if (!strcmp(type, "uint64_t"))
			printf("%" PRIu64, *(uint64_t *)item);
		else
		{
			*(_DAERROR) = UNKNOWN_TYPE_ERROR;
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
		case RESIZE_ERROR: return ("Failed to resize dynamic array");
		case NULL_ARRAY_ERROR: return ("The array passed is NULL");
		case NULL_VALUE_ERROR: return ("The value passed is NULL");
		case UNKNOWN_TYPE_ERROR: return ("Type for array items is unknown");
		default: return ("Unknown internal system error");
	}
}
#endif

#endif
