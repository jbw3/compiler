#ifndef KEYWORDS_H_
#define KEYWORDS_H_

#include <string>
#include <unordered_set>

const std::string BOOL_KEYWORD = "bool";

const std::string BREAK_KEYWORD = "break";

const std::string CONTINUE_KEYWORD = "continue";

const std::string ELIF_KEYWORD = "elif";

const std::string ELSE_KEYWORD = "else";

const std::string EXTERN_KEYWORD = "extern";

const std::string FALSE_KEYWORD = "false";

const std::string FOR_KEYWORD = "for";

const std::string FUNCTION_KEYWORD = "fun";

const std::string IF_KEYWORD = "if";

const std::string INT8_KEYWORD = "i8";

const std::string INT16_KEYWORD = "i16";

const std::string INT32_KEYWORD = "i32";

const std::string INT64_KEYWORD = "i64";

const std::string IN_KEYWORD = "in";

const std::string INT_SIZE_KEYWORD = "isize";

const std::string RETURN_KEYWORD = "return";

const std::string STR_KEYWORD = "str";

const std::string STRUCT_KEYWORD = "struct";

const std::string TRUE_KEYWORD = "true";

const std::string UINT8_KEYWORD = "u8";

const std::string UINT16_KEYWORD = "u16";

const std::string UINT32_KEYWORD = "u32";

const std::string UINT64_KEYWORD = "u64";

const std::string UINT_SIZE_KEYWORD = "usize";

const std::string VARIABLE_KEYWORD = "var";

const std::string WHILE_KEYWORD = "while";

const std::unordered_set<std::string> KEYWORDS =
{
    BOOL_KEYWORD,
    BREAK_KEYWORD,
    CONTINUE_KEYWORD,
    ELIF_KEYWORD,
    ELSE_KEYWORD,
    EXTERN_KEYWORD,
    FOR_KEYWORD,
    FUNCTION_KEYWORD,
    IF_KEYWORD,
    INT8_KEYWORD,
    INT16_KEYWORD,
    INT32_KEYWORD,
    INT64_KEYWORD,
    IN_KEYWORD,
    INT_SIZE_KEYWORD,
    RETURN_KEYWORD,
    STR_KEYWORD,
    STRUCT_KEYWORD,
    UINT8_KEYWORD,
    UINT16_KEYWORD,
    UINT32_KEYWORD,
    UINT64_KEYWORD,
    UINT_SIZE_KEYWORD,
    VARIABLE_KEYWORD,
    WHILE_KEYWORD,
};

#endif // KEYWORDS_H_
