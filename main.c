#define DA_IMPLEMENTATION
#include "dynamic_array.h"

int     main(int argc, char *argv[])
{
	tDynamicArray *da = da_create(DA_INT, "int");

	for (int i = 3; i < 50; (i += 3))
		DA_APPEND(da, CAST_GEN(&i));
	DA_PRINT(da);
	int a;
	DA_POP(da, 2, &a);
	printf("value popped: %d\n", a);
	DA_PRINT(da);
	int val = 10;
	DA_INSERT(da, 2, CAST_GEN(&val));
	DA_PRINT(da);
	return 0;
}
