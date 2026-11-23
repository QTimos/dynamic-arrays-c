/* example.c
 * You can work with #define DA_IMPLEMENTATION
 * Or you can precompile the header file to an object file
 * and link with it in your project
 *
 * you can compile it like this
 * cc -std=c11 -x c dynamic_array.h -DDA_IMPLEMENTATION -c -o dynamic_array.o
 */

#define DA_IMPLEMENTATION
#include "dynamic_array.h"
#include <limits.h>

typedef struct sVec2 { float x, y; } vec2;
static void print_point(void *item)
{
	vec2 *p = item;
	printf("(%f, %f)", p->x, p->y);
}

int main(void)
{ 
	/* ---- int ---- */
	tDynamicArray *ints = da_create(DA_INT, 0);
	da_append(ints, &(int){42});
	da_append(ints, &(int){-7});
	da_append(ints, &(int){1000});
	da_print(ints);
	da_free(ints);

	/* ---- unsigned int ---- */
	tDynamicArray *uints = da_create(DA_UINT, 0);
	da_append(uints, &(unsigned int){3u});
	da_append(uints, &(unsigned int){UINT_MAX});
	da_print(uints);
	da_free(uints);

	/* ---- bool ---- */
	tDynamicArray *bools = da_create(DA_BOOL, 0);
	da_append(bools, &(bool){true});
	da_append(bools, &(bool){false});
	da_print(bools);
	da_free(bools);

	/* ---- char ---- */
	tDynamicArray *chars = da_create(DA_CHAR, 0);
	da_append(chars, &(char){'a'});
	da_append(chars, &(char){'z'});
	da_append(chars, &(char){'\n'});
	da_print(chars);
	da_free(chars);

	/* ---- str (char *) ---- */
	tDynamicArray *strs = da_create(DA_STR, 0);
	da_append(strs, &(char *){"hello"});
	da_append(strs, &(char *){"world"});
	da_append(strs, &(char *){NULL});
	da_print(strs);
	da_free(strs);

	/* ---- float ---- */
	tDynamicArray *floats = da_create(DA_FLOAT, 0);
	da_append(floats, &(float){3.14f});
	da_append(floats, &(float){-0.5f});
	da_print(floats);
	da_free(floats);

	/* ---- double ---- */
	tDynamicArray *doubles = da_create(DA_DOUBLE, 0);
	da_append(doubles, &(double){2.718281828459045});
	da_append(doubles, &(double){-1.5});
	da_print(doubles);
	da_free(doubles);

	/* ---- size_t ---- */
	tDynamicArray *sizes = da_create(DA_SIZE, 0);
	da_append(sizes, &(size_t){0});
	da_append(sizes, &(size_t){SIZE_MAX});
	da_print(sizes);
	da_free(sizes);

	/* ---- long ---- */
	tDynamicArray *longs = da_create(DA_LONG, 0);
	da_append(longs, &(long){LONG_MIN});
	da_append(longs, &(long){LONG_MAX});
	da_print(longs);
	da_free(longs);

	/* ---- unsigned long ---- */
	tDynamicArray *ulongs = da_create(DA_ULONG, 0);
	da_append(ulongs, &(unsigned long){0ul});
	da_append(ulongs, &(unsigned long){ULONG_MAX});
	da_print(ulongs);
	da_free(ulongs);

	/* ---- long long ---- */
	tDynamicArray *llongs = da_create(DA_LLONG, 0);
	da_append(llongs, &(long long){LLONG_MIN});
	da_append(llongs, &(long long){LLONG_MAX});
	da_print(llongs);
	da_free(llongs);

	/* ---- unsigned long long ---- */
	tDynamicArray *ullongs = da_create(DA_ULLONG, 0);
	da_append(ullongs, &(unsigned long long){0ULL});
	da_append(ullongs, &(unsigned long long){ULLONG_MAX});
	da_print(ullongs);
	da_free(ullongs);

	/* ---- short ---- */
	tDynamicArray *shorts = da_create(DA_SHORT, 0);
	da_append(shorts, &(short){SHRT_MIN});
	da_append(shorts, &(short){SHRT_MAX});
	da_print(shorts);
	da_free(shorts);

	/* ---- unsigned short ---- */
	tDynamicArray *ushorts = da_create(DA_USHORT, 0);
	da_append(ushorts, &(unsigned short){0});
	da_append(ushorts, &(unsigned short){USHRT_MAX});
	da_print(ushorts);
	da_free(ushorts);

	/* ---- ptr (void *) ---- */
	int sentinel = 7;
	tDynamicArray *ptrs = da_create(DA_PTR, 0);
	da_append(ptrs, &(void *){&sentinel});
	da_append(ptrs, &(void *){NULL});
	da_print(ptrs);
	da_free(ptrs);

	/* ---- int8_t ---- */
	tDynamicArray *i8 = da_create(DA_I8, 0);
	da_append(i8, &(int8_t){INT8_MIN});
	da_append(i8, &(int8_t){INT8_MAX});
	da_print(i8);
	da_free(i8);

	/* ---- int16_t ---- */
	tDynamicArray *i16 = da_create(DA_I16, 0);
	da_append(i16, &(int16_t){INT16_MIN});
	da_append(i16, &(int16_t){INT16_MAX});
	da_print(i16);
	da_free(i16);

	/* ---- int32_t ---- */
	tDynamicArray *i32 = da_create(DA_I32, 0);
	da_append(i32, &(int32_t){INT32_MIN});
	da_append(i32, &(int32_t){INT32_MAX});
	da_print(i32);
	da_free(i32);

	/* ---- int64_t ---- */
	tDynamicArray *i64 = da_create(DA_I64, 0);
	da_append(i64, &(int64_t){INT64_MIN});
	da_append(i64, &(int64_t){INT64_MAX});
	da_print(i64);
	da_free(i64);

	/* ---- uint8_t ---- */
	tDynamicArray *u8 = da_create(DA_U8, 0);
	da_append(u8, &(uint8_t){0});
	da_append(u8, &(uint8_t){UINT8_MAX});
	da_print(u8);
	da_free(u8);

	/* ---- uint16_t ---- */
	tDynamicArray *u16 = da_create(DA_U16, 0);
	da_append(u16, &(uint16_t){0});
	da_append(u16, &(uint16_t){UINT16_MAX});
	da_print(u16);
	da_free(u16);

	/* ---- uint32_t ---- */
	tDynamicArray *u32 = da_create(DA_U32, 0);
	da_append(u32, &(uint32_t){0});
	da_append(u32, &(uint32_t){UINT32_MAX});
	da_print(u32);
	da_free(u32);

	/* ---- uint64_t ---- */
	tDynamicArray *u64 = da_create(DA_U64, 0);
	da_append(u64, &(uint64_t){0});
	da_append(u64, &(uint64_t){UINT64_MAX});
	da_print(u64);
	da_free(u64);

	/* ---- other operations, same compound-literal style ---- */
	tDynamicArray *demo = da_create(DA_INT, 0);
	da_append(demo, &(int){10});
	da_append(demo, &(int){20});
	da_insert(demo, 1, &(int){15});
	da_set   (demo, 2, &(int){99});
	da_print (demo);

	int popped = 0;
	da_pop(demo, 0, &popped);
	printf("popped = %d\n", popped);

	int *first = da_get(demo, 0);
	if (first) printf("first = %d\n", *first);
	da_free(demo);

	tDynamicArray *pts = da_create(DA_UNKNOWN, sizeof(vec2));
	if (!pts) {
		printf("create: %s\n", da_get_error_msg());
		return 1;
	}
	pts->print_fn = print_point;
	vec2 a = {1.23, 2.93};
	vec2 b = {3.22, 4.03};
	da_append(pts, &a);
	da_append(pts, &b);
	da_print(pts);
	da_free(pts);

	return 0;
}
