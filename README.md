# Dynamic Array

A generic, type-aware dynamic array library written in C. It stores elements in contiguous memory and grows its capacity automatically as elements are appended or inserted.

## Features

- Supports standard C scalar types, fixed-width integer types, `size_t`, and pointers.
- Supports user-defined types through an explicit element size.
- Provides append, insert, indexed access, update, and removal operations.
- Uses `realloc` to grow storage, starting at 256 elements and doubling capacity when needed.
- Supports built-in printing for recognized types and custom printing callbacks.
- Reports errors through a thread-local error state when supported by the compiler.
- Exposes both regular functions and convenience macros through a function table.

## Files

- `dynamic_array.h` — library interface and implementation.
- `example.c` — usage examples for built-in types, custom structures, and array operations.

## Requirements

- A C compiler with support for C11 is recommended.
- Standard C library headers.

## Integration

This is a single-header library. Define `DA_IMPLEMENTATION` in **one** C translation unit before including the header. Do not define it in multiple translation units, or you may get multiple-definition linker errors.

```c
#define DA_IMPLEMENTATION
#include "dynamic_array.h"
```

Alternatively, compile the header implementation separately and link the resulting object file:

```sh
cc -std=c11 -x c dynamic_array.h -DDA_IMPLEMENTATION -c -o dynamic_array.o
```

Then include `dynamic_array.h` in the source files that use the library, without defining `DA_IMPLEMENTATION` there.

## Quick start

```c
#define DA_IMPLEMENTATION
#include "dynamic_array.h"

#include <stdio.h>

int main(void)
{
    tDynamicArray *numbers = da_create(DA_INT, 0);
    if (!numbers) {
        fprintf(stderr, "Could not create array: %s\n", get_error_msg());
        return 1;
    }

    int first = 42;
    int second = 7;

    if (!DA_APPEND(numbers, &first) ||
        !DA_APPEND(numbers, &second)) {
        fprintf(stderr, "Append failed: %s\n", get_error_msg());
        da_free(numbers);
        return 1;
    }

    DA_PRINT(numbers);

    int *value = DA_GET(numbers, 0);
    if (value)
        printf("First element: %d\n", *value);

    da_free(numbers);
    return 0;
}
```

Compile:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic example.c -o example
```

Run:

```sh
./example
```

## API

Create an array with `da_create(type, custom_size)`:

- For a built-in type, pass its `DA_*` type and `0` as `custom_size`.
- For a custom type, pass `DA_UNKNOWN` and `sizeof(your_type)`.

| Function / macro | Purpose |
|---|---|
| `da_create(type, custom_size)` | Creates an empty array. Returns `NULL` on failure. |
| `DA_APPEND(array, value)` / `da_append` | Adds an element to the end. |
| `DA_INSERT(array, index, value)` / `da_insert` | Inserts an element at an index; `index == count` appends. |
| `DA_GET(array, index)` / `da_get` | Returns a pointer to the element at an index, or `NULL` on failure. |
| `DA_SET(array, index, value)` / `da_set` | Replaces an existing element. |
| `DA_POP(array, index, buffer)` / `da_pop` | Removes an element. If `buffer` is non-`NULL`, copies the removed element into it. |
| `DA_PRINT(array)` / `da_print` | Prints the array. |
| `da_set_print_fn(array, fn)` | Sets a custom element-printing callback. |
| `da_free(array)` | Frees the array and its internal storage. |
| `get_error_msg()` | Returns a message for the most recent public operation's error state. |

The macros check whether the array pointer is `NULL` before dispatching through the array's function table. The corresponding functions are also available directly.

## Supported built-in types

The `tDAType` enum includes:

- `DA_INT`, `DA_UINT`, `DA_BOOL`, `DA_CHAR`, `DA_STR`
- `DA_FLOAT`, `DA_DOUBLE`, `DA_SIZE`
- `DA_LONG`, `DA_ULONG`, `DA_LLONG`, `DA_ULLONG`
- `DA_SHORT`, `DA_USHORT`, `DA_PTR`
- `DA_I8`, `DA_I16`, `DA_I32`, `DA_I64`
- `DA_U8`, `DA_U16`, `DA_U32`, `DA_U64`

`DA_STR` stores `char *` values: the array copies the pointer value, not the pointed-to string. The caller remains responsible for the lifetime of the string data.

## Custom types and printing

For a custom type, create the array with `DA_UNKNOWN` and the size of the element. Set a callback if you want `DA_PRINT` to display the elements meaningfully.

```c
typedef struct {
    float x;
    float y;
} vec2;

static void print_vec2(void *item)
{
    vec2 *point = item;
    printf("(%f, %f)", point->x, point->y);
}

/* ... */
tDynamicArray *points = da_create(DA_UNKNOWN, sizeof(vec2));
if (points) {
    da_set_print_fn(points, print_vec2);

    vec2 point = {1.0f, 2.0f};
    DA_APPEND(points, &point);
    DA_PRINT(points);

    da_free(points);
}
```

The callback signature must be exactly `void (*)(void *)`. A custom callback overrides built-in type-based printing.

## Important behavior and ownership

- Elements are copied into the array as raw bytes of `item_size`; the library does not perform deep copies or manage resources referenced by pointers.
- `DA_GET` returns a pointer into the array's internal storage. Do not assume that pointer remains valid after an operation that may resize or move the array, such as an append or insert that grows capacity.
- `DA_POP` optionally copies the removed element into the caller-provided buffer. It does not free resources referenced by the element.
- `da_free` frees the array's backing allocation and array structure. It does not free memory referenced by stored pointers.
- The implementation does not expose a shrink-to-fit operation.

## Error handling

After a failed operation, call `get_error_msg()` to retrieve the message associated with the most recent public `da_*` call. The error state is reset at the beginning of each public operation.

The implementation uses thread-local error storage when supported by the compiler. On toolchains without thread-local storage support, the fallback error state is process-global and is not thread-safe.

## License

This project is licensed under the MIT License.
See the [LICENSE](LICENSE) file for details.
