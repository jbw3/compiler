#ifndef SRC_UNITTESTS_TESTFILES_C_HEADER_OUT_H_
#define SRC_UNITTESTS_TESTFILES_C_HEADER_OUT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct str
{
    size_t Size;
    char* Data;
};

struct Struct1;

struct array_bool
{
    size_t Size;
    bool* Data;
};

struct array_ptr_u64
{
    size_t Size;
    uint64_t** Data;
};

struct array_array_ptr_u64
{
    size_t Size;
    struct array_ptr_u64* Data;
};

struct Struct2
{
    struct str s;
    struct Struct1* s1;
    struct array_bool s2;
    struct array_array_ptr_u64 s3;
};

struct Struct1
{
    struct Struct2 s2;
    float num;
    bool abc;
};

typedef struct Struct1 Struct3;

struct EmptyStruct
{
};

struct FunStruct
{
    void (*f0)();
    struct FunStruct* (*f1)();
    uint8_t (*f2)(uint8_t a);
    int32_t** (*f3)(int32_t* a, int32_t** b, int32_t***** c);
    struct array_i32 (*f4)(struct array_i32 a);
};

bool f1(int32_t x, uint64_t abc, intptr_t s1, uintptr_t s2, float n1, double n2);

struct array_Struct1
{
    size_t Size;
    struct Struct1* Data;
};

void f2(struct str s1, struct array_Struct1 param_2, struct Struct1 param3, struct Struct1*** param4);

struct array_i32
{
    size_t Size;
    int32_t* Data;
};

struct array_i32 f3(struct array_i32 a1, struct array_Struct1 a2);

struct FunStruct* f4(struct FunStruct* a);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SRC_UNITTESTS_TESTFILES_C_HEADER_OUT_H_ */
