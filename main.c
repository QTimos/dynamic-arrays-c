#define DA_IMPLEMENTATION
#include "dynamic_array.h"
#include <assert.h>
#include <stdio.h>

typedef struct {
    int id;
    double score;
    char tag[16];
} TestRecord;

void test_lifecycle_and_empty_state(void) {
    printf("Running: test_lifecycle_and_empty_state...\n");
    tDynamicArray *da = da_create(DA_INT, "int");
    assert(da != NULL);
    assert(da->vtable != NULL);

    // Operations on an empty array should fail safely without crashing
    int out = -1;
    DA_POP(da, 0, &out);
    assert(out == -1);

    assert(DA_GET(da, 0) == NULL);

    int val = 42;
    DA_SET(da, 0, CAST_GEN(&val));

    // Freeing NULL is standard C convention; keep if da_free guards against NULL
    da_free(NULL);
    da_free(da);
    printf("Passed!\n\n");
}

void test_append_get_set_and_reallocation(void) {
    printf("Running: test_append_get_set_and_reallocation...\n");
    tDynamicArray *da = da_create(DA_INT, "int");

    // Push enough items to force multiple internal reallocations
    const int count = 1000;
    for (int i = 0; i < count; i++) {
        DA_APPEND(da, CAST_GEN(&i));
    }

    // Verify DA_GET across all elements after reallocations
    for (int i = 0; i < count; i++) {
        int *ptr = (int *)DA_GET(da, i);
        assert(ptr != NULL);
        assert(*ptr == i);
    }

    // Verify DA_SET modifies elements in place
    int updated_val = 99999;
    DA_SET(da, 500, CAST_GEN(&updated_val));
    assert(*(int *)DA_GET(da, 500) == 99999);

    // Verify out-of-bounds DA_GET and DA_SET
    assert(DA_GET(da, count) == NULL);
    DA_SET(da, count, CAST_GEN(&updated_val));

    da_free(da);
    printf("Passed!\n\n");
}

void test_insert_boundaries(void) {
    printf("Running: test_insert_boundaries...\n");
    tDynamicArray *da = da_create(DA_INT, "int");

    int v1 = 10, v2 = 20, v3 = 30, v_front = 5, v_mid = 15, v_back = 40;

    // Populate base: [10, 20, 30]
    DA_APPEND(da, CAST_GEN(&v1));
    DA_APPEND(da, CAST_GEN(&v2));
    DA_APPEND(da, CAST_GEN(&v3));

    // Insert at front (index 0) -> [5, 10, 20, 30]
    DA_INSERT(da, 0, CAST_GEN(&v_front));
    assert(*(int *)DA_GET(da, 0) == 5);

    // Insert in middle (index 2) -> [5, 10, 15, 20, 30]
    DA_INSERT(da, 2, CAST_GEN(&v_mid));
    assert(*(int *)DA_GET(da, 2) == 15);

    // Insert at end (index 5) -> [5, 10, 15, 20, 30, 40]
    DA_INSERT(da, 5, CAST_GEN(&v_back));
    assert(*(int *)DA_GET(da, 5) == 40);

    // Out-of-bounds insert (should fail safely without modifying existing data)
    int v_oob = 999;
    DA_INSERT(da, 99, CAST_GEN(&v_oob));
    assert(DA_GET(da, 6) == NULL);

    da_free(da);
    printf("Passed!\n\n");
}

void test_pop_boundaries(void) {
    printf("Running: test_pop_boundaries...\n");
    tDynamicArray *da = da_create(DA_INT, "int");

    for (int i = 1; i <= 5; i++) {
        DA_APPEND(da, CAST_GEN(&i)); // [1, 2, 3, 4, 5]
    }

    int popped = 0;

    // Pop from front (index 0) -> leaves [2, 3, 4, 5]
    DA_POP(da, 0, &popped);
    assert(popped == 1);
    assert(*(int *)DA_GET(da, 0) == 2);

    // Pop from middle (index 1 -> value 3) -> leaves [2, 4, 5]
    DA_POP(da, 1, &popped);
    assert(popped == 3);
    assert(*(int *)DA_GET(da, 1) == 4);

    // Pop from end (index 2 -> value 5) -> leaves [2, 4]
    DA_POP(da, 2, &popped);
    assert(popped == 5);
    assert(DA_GET(da, 2) == NULL);

    // Out-of-bounds pop (should fail safely)
    popped = -1;
    DA_POP(da, 50, &popped);
    assert(popped == -1);

    // Pop remaining until empty
    DA_POP(da, 0, &popped);
    assert(popped == 2);
    DA_POP(da, 0, &popped);
    assert(popped == 4);
    assert(DA_GET(da, 0) == NULL);

    da_free(da);
    printf("Passed!\n\n");
}

void test_vtable_types_and_null_payloads(void) {
    printf("Running: test_vtable_types_and_null_payloads...\n");

    // 1. Verify Vtable bindings for Doubles (adjust enum if named differently)
    tDynamicArray *da_dbl = da_create(DA_DOUBLE, "double");
    assert(da_dbl != NULL);
    assert(da_dbl->vtable != NULL);

    double d1 = 3.14159, d2 = 2.71828, d_new = 1.61803;
    DA_APPEND(da_dbl, CAST_GEN(&d1));
    DA_APPEND(da_dbl, CAST_GEN(&d2));
    assert(*(double *)DA_GET(da_dbl, 0) == d1);

    DA_SET(da_dbl, 1, CAST_GEN(&d_new));
    assert(*(double *)DA_GET(da_dbl, 1) == d_new);

    double d_out = 0.0;
    DA_POP(da_dbl, 0, &d_out);
    assert(d_out == d1);
    da_free(da_dbl);

    // 2. Test NULL payloads passed into valid vtable functions
    tDynamicArray *da_null_args = da_create(DA_INT, "int");
    int val = 100;
    DA_APPEND(da_null_args, CAST_GEN(&val));

    DA_APPEND(da_null_args, NULL);
    DA_INSERT(da_null_args, 0, NULL);
    DA_SET(da_null_args, 0, NULL);
    DA_POP(da_null_args, 0, NULL); // Popping without capturing the output value

    da_free(da_null_args);
    printf("Passed!\n\n");
}

int main(void) {
    printf("=== Starting Dynamic Array Test Suite ===\n\n");

    test_lifecycle_and_empty_state();
    test_append_get_set_and_reallocation();
    test_insert_boundaries();
    test_pop_boundaries();
    test_vtable_types_and_null_payloads();

    printf("=== All Tests Completed Successfully ===\n");
    return 0;
}

