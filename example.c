/* example.c — one DA_APPEND per "type" string understood by da_print.
 *
 * Compile (single TU; implementation lives in this file):
 *   cc -std=c11 -Wall -Wextra -Werror example.c -o example
 *   ./example
 *
 * The helper `&(T){value}` is a C99 compound literal: an unnamed
 * temporary of type T initialized to `value`. Its address is stable
 * for the enclosing block, so the library's memcpy has a valid source
 * for the duration of the call. No named variables are needed for the
 * values themselves.
 */
#define DA_IMPLEMENTATION
#include "dynamic_array.h"

#include <limits.h>   /* SHRT_MIN/MAX, USHRT_MAX, UINT_MAX, LONG_MIN/MAX, ... */

int main(void)
{
    /* ---- int ---- */
    tDynamicArray *ints = da_create(DA_INT, "int");
    DA_APPEND(ints, &(int){42});
    DA_APPEND(ints, &(int){-7});
    DA_APPEND(ints, &(int){1000});
    DA_PRINT(ints);                         /* [42, -7, 1000] */
    da_free(ints);

    /* ---- unsigned int ---- */
    tDynamicArray *uints = da_create(DA_UINT, "unsigned int");
    DA_APPEND(uints, &(unsigned int){3u});
    DA_APPEND(uints, &(unsigned int){UINT_MAX});
    DA_PRINT(uints);
    da_free(uints);

    /* ---- bool ---- */
    tDynamicArray *bools = da_create(DA_BOOL, "bool");
    DA_APPEND(bools, &(bool){true});
    DA_APPEND(bools, &(bool){false});
    DA_PRINT(bools);                        /* ['True', 'False'] */
    da_free(bools);

    /* ---- char ---- */
    tDynamicArray *chars = da_create(DA_CHAR, "char");
    DA_APPEND(chars, &(char){'A'});
    DA_APPEND(chars, &(char){'z'});
    DA_APPEND(chars, &(char){'\n'});
    DA_PRINT(chars);
    da_free(chars);

    /* ---- str (char *) ---- */
    tDynamicArray *strs = da_create(DA_STR, "str");
    DA_APPEND(strs, &(char *){"hello"});
    DA_APPEND(strs, &(char *){"world"});
    DA_APPEND(strs, &(char *){NULL});       /* da_print shows "(null)" */
    DA_PRINT(strs);                         /* ["hello", "world", "(null)"] */
    da_free(strs);

    /* ---- float ---- */
    tDynamicArray *floats = da_create(DA_FLOAT, "float");
    DA_APPEND(floats, &(float){3.14f});
    DA_APPEND(floats, &(float){-0.5f});
    DA_PRINT(floats);
    da_free(floats);

    /* ---- double ---- */
    tDynamicArray *doubles = da_create(DA_DOUBLE, "double");
    DA_APPEND(doubles, &(double){2.718281828459045});
    DA_APPEND(doubles, &(double){-1.5});
    DA_PRINT(doubles);
    da_free(doubles);

    /* ---- size_t ---- */
    tDynamicArray *sizes = da_create(DA_SIZE, "size_t");
    DA_APPEND(sizes, &(size_t){0});
    DA_APPEND(sizes, &(size_t){SIZE_MAX});
    DA_PRINT(sizes);
    da_free(sizes);

    /* ---- long ---- */
    tDynamicArray *longs = da_create(DA_LONG, "long");
    DA_APPEND(longs, &(long){LONG_MIN});
    DA_APPEND(longs, &(long){LONG_MAX});
    DA_PRINT(longs);
    da_free(longs);

    /* ---- unsigned long ---- */
    tDynamicArray *ulongs = da_create(DA_ULONG, "unsigned long");
    DA_APPEND(ulongs, &(unsigned long){0UL});
    DA_APPEND(ulongs, &(unsigned long){ULONG_MAX});
    DA_PRINT(ulongs);
    da_free(ulongs);

    /* ---- long long ---- */
    tDynamicArray *llongs = da_create(DA_LLONG, "long long");
    DA_APPEND(llongs, &(long long){LLONG_MIN});
    DA_APPEND(llongs, &(long long){LLONG_MAX});
    DA_PRINT(llongs);
    da_free(llongs);

    /* ---- unsigned long long ---- */
    tDynamicArray *ullongs = da_create(DA_ULLONG, "unsigned long long");
    DA_APPEND(ullongs, &(unsigned long long){0ULL});
    DA_APPEND(ullongs, &(unsigned long long){ULLONG_MAX});
    DA_PRINT(ullongs);
    da_free(ullongs);

    /* ---- short ---- */
    tDynamicArray *shorts = da_create(DA_SHORT, "short");
    DA_APPEND(shorts, &(short){SHRT_MIN});
    DA_APPEND(shorts, &(short){SHRT_MAX});
    DA_PRINT(shorts);
    da_free(shorts);

    /* ---- unsigned short ---- */
    tDynamicArray *ushorts = da_create(DA_USHORT, "unsigned short");
    DA_APPEND(ushorts, &(unsigned short){0});
    DA_APPEND(ushorts, &(unsigned short){USHRT_MAX});
    DA_PRINT(ushorts);
    da_free(ushorts);

    /* ---- ptr (void *) ---- */
    int sentinel = 7;
    tDynamicArray *ptrs = da_create(DA_PTR, "ptr");
    DA_APPEND(ptrs, &(void *){&sentinel});
    DA_APPEND(ptrs, &(void *){NULL});
    DA_PRINT(ptrs);                         /* [0x..., (nil)] */
    da_free(ptrs);

    /* ---- int8_t ---- */
    tDynamicArray *i8 = da_create(DA_I8, "int8_t");
    DA_APPEND(i8, &(int8_t){INT8_MIN});
    DA_APPEND(i8, &(int8_t){INT8_MAX});
    DA_PRINT(i8);
    da_free(i8);

    /* ---- int16_t ---- */
    tDynamicArray *i16 = da_create(DA_I16, "int16_t");
    DA_APPEND(i16, &(int16_t){INT16_MIN});
    DA_APPEND(i16, &(int16_t){INT16_MAX});
    DA_PRINT(i16);
    da_free(i16);

    /* ---- int32_t ---- */
    tDynamicArray *i32 = da_create(DA_I32, "int32_t");
    DA_APPEND(i32, &(int32_t){INT32_MIN});
    DA_APPEND(i32, &(int32_t){INT32_MAX});
    DA_PRINT(i32);
    da_free(i32);

    /* ---- int64_t ---- */
    tDynamicArray *i64 = da_create(DA_I64, "int64_t");
    DA_APPEND(i64, &(int64_t){INT64_MIN});
    DA_APPEND(i64, &(int64_t){INT64_MAX});
    DA_PRINT(i64);
    da_free(i64);

    /* ---- uint8_t ---- */
    tDynamicArray *u8 = da_create(DA_U8, "uint8_t");
    DA_APPEND(u8, &(uint8_t){0});
    DA_APPEND(u8, &(uint8_t){UINT8_MAX});
    DA_PRINT(u8);
    da_free(u8);

    /* ---- uint16_t ---- */
    tDynamicArray *u16 = da_create(DA_U16, "uint16_t");
    DA_APPEND(u16, &(uint16_t){0});
    DA_APPEND(u16, &(uint16_t){UINT16_MAX});
    DA_PRINT(u16);
    da_free(u16);

    /* ---- uint32_t ---- */
    tDynamicArray *u32 = da_create(DA_U32, "uint32_t");
    DA_APPEND(u32, &(uint32_t){0});
    DA_APPEND(u32, &(uint32_t){UINT32_MAX});
    DA_PRINT(u32);
    da_free(u32);

    /* ---- uint64_t ---- */
    tDynamicArray *u64 = da_create(DA_U64, "uint64_t");
    DA_APPEND(u64, &(uint64_t){0});
    DA_APPEND(u64, &(uint64_t){UINT64_MAX});
    DA_PRINT(u64);
    da_free(u64);

    /* ---- other operations, same compound-literal style ---- */
    tDynamicArray *demo = da_create(DA_INT, "int");
    DA_APPEND(demo, &(int){10});            /* [10]       */
    DA_APPEND(demo, &(int){20});            /* [10, 20]   */
    DA_INSERT(demo, 1, &(int){15});         /* [10, 15, 20] */
    DA_SET   (demo, 2, &(int){99});         /* [10, 15, 99] */
    DA_PRINT (demo);

    int popped = 0;
    DA_POP(demo, 0, &popped);               /* popped = 10, [15, 99] */
    printf("popped = %d\n", popped);

    int *first = DA_GET(demo, 0);           /* *first == 15 */
    if (first) printf("first = %d\n", *first);

    da_free(demo);

    return 0;
}
