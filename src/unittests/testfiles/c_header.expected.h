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

struct Struct2
{
    struct str s;
    struct Struct1* s1;
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

bool f1(int32_t x, uint64_t abc, float n1, double n2);

void f2(struct str s1);

struct array_i32
{
    size_t Size;
    int32_t* Data;
};

struct array_Struct1
{
    size_t Size;
    struct Struct1* Data;
};

void f3(struct array_i32 a1, struct array_Struct1 a2);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* SRC_UNITTESTS_TESTFILES_C_HEADER_OUT_H_ */
