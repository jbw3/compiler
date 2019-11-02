#ifndef KEYWORDS_H_
#define KEYWORDS_H_

#include <string>
#include <unordered_set>

const std::string BOOL_KEYWORD = "bool";

const std::string ELSE_KEYWORD = "else";

const std::string FUNCTION_KEYWORD = "fun";

const std::string IF_KEYWORD = "if";

const std::string INT8_KEYWORD = "i8";

const std::string INT16_KEYWORD = "i16";

const std::string INT32_KEYWORD = "i32";

const std::string INT64_KEYWORD = "i64";

const std::string UINT8_KEYWORD = "u8";

const std::string UINT16_KEYWORD = "u16";

const std::string UINT32_KEYWORD = "u32";

const std::string UINT64_KEYWORD = "u64";

const std::string VARIABLE_KEYWORD = "var";

const std::string WHILE_KEYWORD = "while";

const std::unordered_set<std::string> KEYWORDS =
{
    BOOL_KEYWORD,
    INT8_KEYWORD,
    INT16_KEYWORD,
    INT32_KEYWORD,
    INT64_KEYWORD,
    UINT8_KEYWORD,
    UINT16_KEYWORD,
    UINT32_KEYWORD,
    UINT64_KEYWORD,
    ELSE_KEYWORD,
    FUNCTION_KEYWORD,
    IF_KEYWORD,
    VARIABLE_KEYWORD,
    WHILE_KEYWORD,
};

const std::unordered_set<std::string> RESERVED_KEYWORDS =
{
    "as",
    "async",
    "await",
    "break",
    "const",
    "continue",
    "default",
    "elif",
    "enum",
    "extern",
    "for",
    "import",
    "in",
    "match",
    "namespace",
    "return",
    "struct",
    "yield",
};

#endif // KEYWORDS_H_
