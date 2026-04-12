#ifndef TOKEN_H_
#define TOKEN_H_

#include "ROString.h"
#include <stdint.h>

class Token
{
public:
    static const Token* None;

    // main types
    static constexpr uint16_t IdentifierType        = 0x1000;
    static constexpr uint16_t BuiltInIdentifierType = 0x2000;
    static constexpr uint16_t KeywordType           = 0x3000;
    static constexpr uint16_t BoolLiteralType       = 0x4000;
    static constexpr uint16_t IntLiteralType        = 0x5000;
    static constexpr uint16_t FloatLiteralType      = 0x6000;
    static constexpr uint16_t StrLiteralType        = 0x7000;
    static constexpr uint16_t SymbolType            = 0x8000;

    // symbol flags
    static constexpr uint16_t UnaryOp  = 0x0800;
    static constexpr uint16_t BinaryOp = 0x0400;

    // type name flag
    static constexpr uint16_t TypeName = 0x0800;

    // --- types ---

    static constexpr uint16_t Invalid      = 0;

    // identifier
    static constexpr uint16_t Identifier   = IdentifierType;

    // built-in identifier
    static constexpr uint16_t BuiltInIdentifier = BuiltInIdentifierType;

    // keyword
    static constexpr uint16_t Bool         = KeywordType | TypeName |  0;
    static constexpr uint16_t Break        = KeywordType |  1;
    static constexpr uint16_t Const        = KeywordType |  2;
    static constexpr uint16_t Continue     = KeywordType |  3;
    static constexpr uint16_t Elif         = KeywordType |  4;
    static constexpr uint16_t Else         = KeywordType |  5;
    static constexpr uint16_t Extern       = KeywordType |  6;
    static constexpr uint16_t For          = KeywordType |  7;
    static constexpr uint16_t Fun          = KeywordType |  8;
    static constexpr uint16_t If           = KeywordType |  9;
    static constexpr uint16_t I8           = KeywordType | TypeName | 10;
    static constexpr uint16_t I16          = KeywordType | TypeName | 11;
    static constexpr uint16_t I32          = KeywordType | TypeName | 12;
    static constexpr uint16_t I64          = KeywordType | TypeName | 13;
    static constexpr uint16_t In           = KeywordType | 14;
    static constexpr uint16_t ISize        = KeywordType | TypeName | 15;
    static constexpr uint16_t Return       = KeywordType | 16;
    static constexpr uint16_t Str          = KeywordType | TypeName | 17;
    static constexpr uint16_t Struct       = KeywordType | 18;
    static constexpr uint16_t Type         = KeywordType | TypeName | 19;
    static constexpr uint16_t U8           = KeywordType | TypeName | 20;
    static constexpr uint16_t U16          = KeywordType | TypeName | 21;
    static constexpr uint16_t U32          = KeywordType | TypeName | 22;
    static constexpr uint16_t U64          = KeywordType | TypeName | 23;
    static constexpr uint16_t Unchecked    = KeywordType | 24;
    static constexpr uint16_t USize        = KeywordType | TypeName | 25;
    static constexpr uint16_t Var          = KeywordType | 26;
    static constexpr uint16_t While        = KeywordType | 27;

    // bool literal
    static constexpr uint16_t FalseLit     = BoolLiteralType | 0;
    static constexpr uint16_t TrueLit      = BoolLiteralType | 1;

    // int literal
    static constexpr uint16_t BinIntLit    = IntLiteralType | 0;
    static constexpr uint16_t OctIntLit    = IntLiteralType | 1;
    static constexpr uint16_t DecIntLit    = IntLiteralType | 2;
    static constexpr uint16_t HexIntLit    = IntLiteralType | 3;

    // float literal
    static constexpr uint16_t FloatLit     = FloatLiteralType;

    // string literal
    static constexpr uint16_t StrLit       = StrLiteralType;

    // symbol
    static constexpr uint16_t Equal                      = SymbolType | BinaryOp |  0;
    static constexpr uint16_t EqualEqual                 = SymbolType | BinaryOp |  1;
    static constexpr uint16_t Less                       = SymbolType | BinaryOp |  2;
    static constexpr uint16_t LessEqual                  = SymbolType | BinaryOp |  3;
    static constexpr uint16_t LessLess                   = SymbolType | BinaryOp |  4;
    static constexpr uint16_t LessLessEqual              = SymbolType | BinaryOp |  5;
    static constexpr uint16_t Greater                    = SymbolType | BinaryOp |  6;
    static constexpr uint16_t GreaterEqual               = SymbolType | BinaryOp |  7;
    static constexpr uint16_t GreaterGreater             = SymbolType | BinaryOp |  8;
    static constexpr uint16_t GreaterGreaterEqual        = SymbolType | BinaryOp |  9;
    static constexpr uint16_t GreaterGreaterGreater      = SymbolType | BinaryOp | 10;
    static constexpr uint16_t GreaterGreaterGreaterEqual = SymbolType | BinaryOp | 11;
    static constexpr uint16_t Plus                       = SymbolType | BinaryOp | 12;
    static constexpr uint16_t PlusEqual                  = SymbolType | BinaryOp | 13;
    static constexpr uint16_t Minus                      = SymbolType | UnaryOp | BinaryOp | 14;
    static constexpr uint16_t MinusEqual                 = SymbolType | BinaryOp | 15;
    static constexpr uint16_t Times                      = SymbolType | UnaryOp | BinaryOp | 16;
    static constexpr uint16_t TimesEqual                 = SymbolType | BinaryOp | 17;
    static constexpr uint16_t Divide                     = SymbolType | BinaryOp | 18;
    static constexpr uint16_t DivideEqual                = SymbolType | BinaryOp | 19;
    static constexpr uint16_t Remainder                  = SymbolType | BinaryOp | 20;
    static constexpr uint16_t RemainderEqual             = SymbolType | BinaryOp | 21;
    static constexpr uint16_t Exclaim                    = SymbolType | UnaryOp | 22;
    static constexpr uint16_t ExclaimEqual               = SymbolType | BinaryOp | 23;
    static constexpr uint16_t Ampersand                  = SymbolType | UnaryOp | BinaryOp | 24;
    static constexpr uint16_t AmpersandEqual             = SymbolType | BinaryOp | 25;
    static constexpr uint16_t AmpersandAmpersand         = SymbolType | BinaryOp | 26;
    static constexpr uint16_t Bar                        = SymbolType | BinaryOp | 27;
    static constexpr uint16_t BarEqual                   = SymbolType | BinaryOp | 28;
    static constexpr uint16_t BarBar                     = SymbolType | BinaryOp | 29;
    static constexpr uint16_t Caret                      = SymbolType | BinaryOp | 30;
    static constexpr uint16_t CaretEqual                 = SymbolType | BinaryOp | 31;
    static constexpr uint16_t OpenPar                    = SymbolType | 32;
    static constexpr uint16_t ClosePar                   = SymbolType | 33;
    static constexpr uint16_t OpenBracket                = SymbolType | 34;
    static constexpr uint16_t CloseBracket               = SymbolType | 35;
    static constexpr uint16_t OpenBrace                  = SymbolType | 36;
    static constexpr uint16_t CloseBrace                 = SymbolType | 37;
    static constexpr uint16_t Comma                      = SymbolType | 38;
    static constexpr uint16_t Period                     = SymbolType | 39;
    static constexpr uint16_t PeriodPeriod               = SymbolType | BinaryOp | 40;
    static constexpr uint16_t PeriodPeriodLess           = SymbolType | BinaryOp | 41;
    static constexpr uint16_t SemiColon                  = SymbolType | 42;
    static constexpr uint16_t Colon                      = SymbolType | 43;

    static constexpr uint16_t MAIN_TYPE_MASK = 0xf000;

    static constexpr uint16_t GetMainType(uint16_t type)
    {
        return MAIN_TYPE_MASK & type;
    }

    static constexpr bool IsUnaryOp(uint16_t type)
    {
        bool isSymbol = GetMainType(type) == SymbolType;
        bool isUnaryOp = (type & UnaryOp) == UnaryOp;
        return isSymbol & isUnaryOp;
    }

    static constexpr bool IsBinaryOp(uint16_t type)
    {
        bool isSymbol = GetMainType(type) == SymbolType;
        bool isBinaryOp = (type & BinaryOp) == BinaryOp;
        return isSymbol & isBinaryOp;
    }

    static constexpr bool IsTypeName(uint16_t type)
    {
        bool isKeyword = GetMainType(type) == KeywordType;
        bool isTypeName = (type & TypeName) == TypeName;
        return isKeyword & isTypeName;
    }

    Token(ROString value, unsigned filenameId, unsigned line, unsigned column, uint16_t type);

    ROString value;
    unsigned filenameId;
    unsigned line;
    unsigned column;
    uint16_t type;
};

#endif // TOKEN_H_
