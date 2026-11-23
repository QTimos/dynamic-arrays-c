#ifndef DA_H
#define DA_H
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

/* type enum, for printing. */
typedef enum {
	DA_UNKNOWN = 0,
	DA_TYPE,
	DA_INT,
	DA_UINT,
	DA_BOOL,
	DA_CHAR,
	DA_STR,
	DA_FLOAT,
	DA_DOUBLE,
	DA_SIZE,
	DA_LONG,
	DA_ULONG,
	DA_LLONG,
	DA_ULLONG,
	DA_SHORT,
	DA_USHORT,
	DA_PTR,
	DA_I8,
	DA_I16,
	DA_I32,
	DA_I64,
	DA_U8,
	DA_U16,
	DA_U32,
	DA_U64
} tDAType;

/* Custom display: called once per element by da_print.
 * Only used when the caller sets da->print_fn. Signature must be exactly
 * void (*)(void *); do NOT cast a differently-typed function into it. */
typedef void (*tDAPrintFn)(void *item);

struct sDynamicArray;

typedef struct sDynamicArrayVTable {
	bool (*da_insert)(struct sDynamicArray *da, size_t index, void *value);
	bool (*da_append)(struct sDynamicArray *da, void *value);
	void *(*da_get)(struct sDynamicArray *da, size_t index);
	bool (*da_set)(struct sDynamicArray *da, size_t index, void *value);
	bool (*da_pop)(struct sDynamicArray *da, size_t index, void *buf);
	void (*da_print)(struct sDynamicArray *da);
} tDynamicArrayVTable;

typedef struct sDynamicArray {
	void *array;
	size_t count;
	size_t capacity;
	size_t item_size;
	tDAType type;
	/* Optional. If non-NULL, da_print calls it per element and ignores
	 * `type`. Set it directly on the struct; e.g. for a struct point:
	 *     da->print_fn = print_point;                                  */
	tDAPrintFn print_fn;
	const tDynamicArrayVTable *vtable;
} tDynamicArray;

bool da_insert(tDynamicArray *da, size_t index, void *value);
bool da_append(tDynamicArray *da, void *value);
void *da_get(tDynamicArray *da, size_t index);
bool da_set(tDynamicArray *da, size_t index, void *value);
bool da_pop(tDynamicArray *da, size_t index, void *buf);
void da_print(tDynamicArray *da);

/*
 * da_create: build an array.
 *   type != DA_UNKNOWN : custom_size must be 0; item_size derived from type.
 *   type == DA_UNKNOWN : custom_size must be > 0; caller is responsible for
 *                        providing da->print_fn later if they want printing.
 * Returns NULL on invalid arguments or allocation failure; check
 * get_error_msg() for the reason.
 */
tDynamicArray *da_create(tDAType type, size_t custom_size);
void da_set_print_fn(tDynamicArray *da, tDAPrintFn fn);
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

#define DA_TYPE_SIZE(T)    sizeof(T)

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

static DA_THREAD_LOCAL int8_t da_err_state = 0;

static inline int8_t* get_err(void)
{
	return &da_err_state;
}
#define _DAERROR (get_err())
#define DAERROR *_DAERROR

static size_t da_get_item_size(tDAType type)
{
	switch (type) {
		case DA_INT:     return sizeof(int);
		case DA_UINT:    return sizeof(unsigned int);
		case DA_BOOL:    return sizeof(bool);
		case DA_CHAR:    return sizeof(char);
		case DA_STR:     return sizeof(char *);
		case DA_FLOAT:   return sizeof(float);
		case DA_DOUBLE:  return sizeof(double);
		case DA_SIZE:    return sizeof(size_t);
		case DA_LONG:    return sizeof(long);
		case DA_ULONG:   return sizeof(unsigned long);
		case DA_LLONG:   return sizeof(long long);
		case DA_ULLONG:  return sizeof(unsigned long long);
		case DA_SHORT:   return sizeof(short);
		case DA_USHORT:  return sizeof(unsigned short);
		case DA_PTR:     return sizeof(void *);
		case DA_I8:      return sizeof(int8_t);
		case DA_I16:     return sizeof(int16_t);
		case DA_I32:     return sizeof(int32_t);
		case DA_I64:     return sizeof(int64_t);
		case DA_U8:      return sizeof(uint8_t);
		case DA_U16:     return sizeof(uint16_t);
		case DA_U32:     return sizeof(uint32_t);
		case DA_U64:     return sizeof(uint64_t);
		case DA_UNKNOWN:
		case DA_TYPE:
		default:         return 0;
	}
}

void da_free(tDynamicArray *da)
{
	*(_DAERROR) = SUCCESS;
	if (!da) { *(_DAERROR) = NULL_ARRAY_ERROR; return; }
	free(da->array);
	free(da);
}

tDynamicArray *da_create(tDAType type, size_t custom_size)
{
	*(_DAERROR) = SUCCESS;

	size_t item_size;
	if (type == DA_UNKNOWN) {
		if (!custom_size) { *(_DAERROR) = CREATION_ERROR; return NULL; }
		item_size = custom_size;
	} else {
		if (custom_size) { *(_DAERROR) = CREATION_ERROR; return NULL; }
		item_size = da_get_item_size(type);
	}
	if (!item_size) { *(_DAERROR) = CREATION_ERROR; return NULL; }

	tDynamicArray *da = malloc(sizeof(tDynamicArray));
	if (!da) { *(_DAERROR) = CREATION_ERROR; return NULL; }

	da->array     = NULL;
	da->count     = 0;
	da->capacity  = 0;
	da->item_size = item_size;
	da->type      = type;
	da->print_fn  = NULL;

	static const tDynamicArrayVTable vtable = {
		da_insert, da_append, da_get, da_set, da_pop, da_print
	};
	da->vtable = &vtable;
	return da;
}

bool da_append(tDynamicArray *da, void *value)
{
	*(_DAERROR) = SUCCESS;
	if (!da)    { *(_DAERROR) = NULL_ARRAY_ERROR; return false; }
	if (!value) { *(_DAERROR) = NULL_VALUE_ERROR; return false; }

	void *new_array;
	void *target;
	void *value_p;
	bool  aliased;

	aliased = (da->array &&
		(uintptr_t)value >= (uintptr_t)da->array &&
		(uintptr_t)value <  (uintptr_t)da->array + da->count * da->item_size);
	if (aliased)
	{
		value_p = malloc(da->item_size);
		if (!value_p) { *(_DAERROR) = RESIZE_ERROR; return false; }
		memcpy(value_p, value, da->item_size);
	}
	else value_p = value;

	if (da->count >= da->capacity)
	{
		size_t cap_tmp = da->capacity;
		if (cap_tmp == 0) cap_tmp = 256;
		else if (cap_tmp > SIZE_MAX / 2) {
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return false;
		}
		else cap_tmp *= 2;
		if (cap_tmp > SIZE_MAX / da->item_size) {
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return false;
		}
		new_array = realloc(da->array, cap_tmp * da->item_size);
		if (!new_array) {
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return false;
		}
		da->capacity = cap_tmp;
		da->array = new_array;
	}
	target = (char *)da->array + (da->count * da->item_size);
	memcpy(target, value_p, da->item_size);
	if (aliased) free(value_p);
	da->count++;
	return true;
}

void *da_get(tDynamicArray *da, size_t index)
{
	*(_DAERROR) = SUCCESS;
	if (!da) { *(_DAERROR) = NULL_ARRAY_ERROR; return NULL; }
	if (index >= da->count) { *(_DAERROR) = INDEX_OOB_ERROR; return NULL; }
	return (uint8_t *)da->array + (index * da->item_size);
}

bool da_set(tDynamicArray *da, size_t index, void *value)
{
	*(_DAERROR) = SUCCESS;
	if (!da)    { *(_DAERROR) = NULL_ARRAY_ERROR; return false; }
	if (!value) { *(_DAERROR) = NULL_VALUE_ERROR; return false; }
	if (index >= da->count) { *(_DAERROR) = INDEX_OOB_ERROR; return false; }
	void *target = (char *)da->array + (index * da->item_size);
	if (target == value) return true;
	memcpy(target, value, da->item_size);
	return true;
}

bool da_insert(tDynamicArray *da, size_t index, void *value)
{
	*(_DAERROR) = SUCCESS;
	if (!da)    { *(_DAERROR) = NULL_ARRAY_ERROR; return false; }
	if (!value) { *(_DAERROR) = NULL_VALUE_ERROR; return false; }

	void  *new_array;
	void  *target;
	void  *src;
	void  *dst;
	void  *value_p;
	size_t elements_to_move;
	bool  aliased;

	aliased = (da->array &&
		(uintptr_t)value >= (uintptr_t)da->array &&
		(uintptr_t)value <  (uintptr_t)da->array + da->count * da->item_size);
	if (aliased)
	{
		value_p = malloc(da->item_size);
		if (!value_p) { *(_DAERROR) = RESIZE_ERROR; return false; }
		memcpy(value_p, value, da->item_size);
	}
	else value_p = value;

	if (index > da->count) {
		if (aliased) free(value_p);
		*(_DAERROR) = INDEX_OOB_ERROR;
		return false;
	}
	if (da->count >= da->capacity)
	{
		size_t cap_tmp = da->capacity;
		if (cap_tmp == 0) cap_tmp = 256;
		else if (cap_tmp > SIZE_MAX / 2) {
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return false;
		}
		else cap_tmp *= 2;
		if (cap_tmp > SIZE_MAX / da->item_size) {
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return false;
		}
		new_array = realloc(da->array, cap_tmp * da->item_size);
		if (!new_array) {
			if (aliased) free(value_p);
			*(_DAERROR) = RESIZE_ERROR;
			return false;
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
	return true;
}

bool da_pop(tDynamicArray *da, size_t index, void *buf)
{
	*(_DAERROR) = SUCCESS;
	if (!da) { *(_DAERROR) = NULL_ARRAY_ERROR; return false; }
	if (index >= da->count) { *(_DAERROR) = INDEX_OOB_ERROR; return false; }
	if (buf) memcpy(buf, (uint8_t *)da->array + (index * da->item_size), da->item_size);
	size_t elements_to_move = da->count - index - 1;
	void *src = (uint8_t *)da->array + ((index + 1) * da->item_size);
	void *dst = (uint8_t *)da->array + (index * da->item_size);
	if (elements_to_move > 0)
		memmove(dst, src, elements_to_move * da->item_size);
	da->count--;
	return true;
}

void da_print(tDynamicArray *da)
{
	*(_DAERROR) = SUCCESS;
	if (!da) { *(_DAERROR) = NULL_ARRAY_ERROR; return; }
	if (da->count == 0) { printf("[]\n"); return; }

	/* Unprintable: DA_UNKNOWN with no print_fn. Not an error — the caller
	 * just didn't ask for formatted output. */
	if (!da->print_fn && da->type == DA_UNKNOWN) {
		printf("[<unprintable: %zu items of %zu bytes>]\n",
		       da->count, da->item_size);
		return;
	}

	printf("[");
	for (size_t i = 0; i < da->count; i++)
	{
		void *item = (uint8_t *)da->array + i * da->item_size;

		if (da->print_fn) {
			da->print_fn(item);
		} else {
			switch (da->type)
			{
				case DA_INT:    printf("%d", *(int *)item); break;
				case DA_UINT:   printf("%u", *(unsigned int *)item); break;
				case DA_BOOL:   printf("'%s'", *(bool *)item ? "True" : "False"); break;
				case DA_CHAR:   printf("'%c'", *(char *)item); break;
				case DA_STR: {
					char *s = *(char **)item;
					printf("\"%s\"", s ? s : "(null)");
					break;
				}
				case DA_FLOAT:  printf("%f", *(float *)item); break;
				case DA_DOUBLE: printf("%f", *(double *)item); break;
				case DA_SIZE:   printf("%zu", *(size_t *)item); break;
				case DA_LONG:   printf("%ld", *(long *)item); break;
				case DA_ULONG:  printf("%lu", *(unsigned long *)item); break;
				case DA_LLONG:  printf("%lld", *(long long *)item); break;
				case DA_ULLONG: printf("%llu", *(unsigned long long *)item); break;
				case DA_SHORT:  printf("%hd", *(short *)item); break;
				case DA_USHORT: printf("%hu", *(unsigned short *)item); break;
				case DA_PTR:    printf("%p", *(void **)item); break;
				case DA_I8:     printf("%" PRId8,  *(int8_t *)item);  break;
				case DA_I16:    printf("%" PRId16, *(int16_t *)item); break;
				case DA_I32:    printf("%" PRId32, *(int32_t *)item); break;
				case DA_I64:    printf("%" PRId64, *(int64_t *)item); break;
				case DA_U8:     printf("%" PRIu8,  *(uint8_t *)item);  break;
				case DA_U16:    printf("%" PRIu16, *(uint16_t *)item); break;
				case DA_U32:    printf("%" PRIu32, *(uint32_t *)item); break;
				case DA_U64:    printf("%" PRIu64, *(uint64_t *)item); break;
				case DA_TYPE:
				case DA_UNKNOWN:
				default:
					*(_DAERROR) = UNKNOWN_TYPE_ERROR;
					goto done;
			}
		}
		if (i < da->count - 1) printf(", ");
	}
done:
	printf("]\n");
}

const char* get_error_msg(void)
{
	int8_t *error = _DAERROR;
	switch (*error)
	{
		case SUCCESS: return "Success";
		case INDEX_OOB_ERROR: return "Accessing index outside of array bounds";
		case CREATION_ERROR: return "Failed to create dynamic array";
		case RESIZE_ERROR: return "Failed to resize dynamic array";
		case NULL_ARRAY_ERROR: return "The array passed is NULL";
		case NULL_VALUE_ERROR: return "The value passed is NULL";
		case UNKNOWN_TYPE_ERROR: return "Type for array items is unknown";
		default: return "Unknown internal system error";
	}
}

void da_set_print_fn(tDynamicArray *da, tDAPrintFn fn)
{
    *(_DAERROR) = SUCCESS;
    if (!da) { *(_DAERROR) = NULL_ARRAY_ERROR; return; }
    da->print_fn = fn;
}
#endif

#endif
