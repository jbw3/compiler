#ifndef TOKEN_H_
#define TOKEN_H_

#include "ROString.h"
#include <stdint.h>

class Token
{
public:
    static const Token* None;

    // types
    enum EMainType : uint16_t
    {
        eIdentifierType        = 0x1000,
        eBuiltInIdentifierType = 0x2000,
        eKeywordType           = 0x3000,
        eBoolLiteralType       = 0x4000,
        eIntLiteralType        = 0x5000,
        eFloatLiteralType      = 0x6000,
        eStrLiteralType        = 0x7000,
        eSymbolType            = 0x8000,
    };

    enum ESymbolFlags : uint16_t
    {
        eUnaryOp  = 0x0800,
        eBinaryOp = 0x0400,
    };

    enum ETypeNameFlag : uint16_t
    {
        eTypeName = 0x0800,
    };

    enum EType : uint16_t
    {
        eInvalid      = 0,

        // identifier
        eIdentifier   = eIdentifierType,

        // built-in identifier
        eBuiltInIdentifier = eBuiltInIdentifierType,

        // keyword
        eBool         = eKeywordType | eTypeName |  0,
        eBreak        = eKeywordType |  1,
        eConst        = eKeywordType |  2,
        eContinue     = eKeywordType |  3,
        eElif         = eKeywordType |  4,
        eElse         = eKeywordType |  5,
        eExtern       = eKeywordType |  6,
        eFor          = eKeywordType |  7,
        eFun          = eKeywordType |  8,
        eIf           = eKeywordType |  9,
        eI8           = eKeywordType | eTypeName | 10,
        eI16          = eKeywordType | eTypeName | 11,
        eI32          = eKeywordType | eTypeName | 12,
        eI64          = eKeywordType | eTypeName | 13,
        eIn           = eKeywordType | 14,
        eISize        = eKeywordType | eTypeName | 15,
        eReturn       = eKeywordType | 16,
        eStr          = eKeywordType | eTypeName | 17,
        eStruct       = eKeywordType | 18,
        eType         = eKeywordType | eTypeName | 19,
        eU8           = eKeywordType | eTypeName | 20,
        eU16          = eKeywordType | eTypeName | 21,
        eU32          = eKeywordType | eTypeName | 22,
        eU64          = eKeywordType | eTypeName | 23,
        eUnchecked    = eKeywordType | 24,
        eUSize        = eKeywordType | eTypeName | 25,
        eVar          = eKeywordType | 26,
        eWhile        = eKeywordType | 27,

        // bool literal
        eFalseLit     = eBoolLiteralType | 0,
        eTrueLit      = eBoolLiteralType | 1,

        // int literal
        eBinIntLit    = eIntLiteralType | 0,
        eOctIntLit    = eIntLiteralType | 1,
        eDecIntLit    = eIntLiteralType | 2,
        eHexIntLit    = eIntLiteralType | 3,

        // float literal
        eFloatLit     = eFloatLiteralType,

        // string literal
        eStrLit       = eStrLiteralType,

        // symbol
        eEqual                      = eSymbolType | eBinaryOp |  0,
        eEqualEqual                 = eSymbolType | eBinaryOp |  1,
        eLess                       = eSymbolType | eBinaryOp |  2,
        eLessEqual                  = eSymbolType | eBinaryOp |  3,
        eLessLess                   = eSymbolType | eBinaryOp |  4,
        eLessLessEqual              = eSymbolType | eBinaryOp |  5,
        eGreater                    = eSymbolType | eBinaryOp |  6,
        eGreaterEqual               = eSymbolType | eBinaryOp |  7,
        eGreaterGreater             = eSymbolType | eBinaryOp |  8,
        eGreaterGreaterEqual        = eSymbolType | eBinaryOp |  9,
        eGreaterGreaterGreater      = eSymbolType | eBinaryOp | 10,
        eGreaterGreaterGreaterEqual = eSymbolType | eBinaryOp | 11,
        ePlus                       = eSymbolType | eBinaryOp | 12,
        ePlusEqual                  = eSymbolType | eBinaryOp | 13,
        eMinus                      = eSymbolType | eUnaryOp | eBinaryOp | 14,
        eMinusEqual                 = eSymbolType | eBinaryOp | 15,
        eTimes                      = eSymbolType | eUnaryOp | eBinaryOp | 16,
        eTimesEqual                 = eSymbolType | eBinaryOp | 17,
        eDivide                     = eSymbolType | eBinaryOp | 18,
        eDivideEqual                = eSymbolType | eBinaryOp | 19,
        eRemainder                  = eSymbolType | eBinaryOp | 20,
        eRemainderEqual             = eSymbolType | eBinaryOp | 21,
        eExclaim                    = eSymbolType | eUnaryOp | 22,
        eExclaimEqual               = eSymbolType | eBinaryOp | 23,
        eAmpersand                  = eSymbolType | eUnaryOp | eBinaryOp | 24,
        eAmpersandEqual             = eSymbolType | eBinaryOp | 25,
        eAmpersandAmpersand         = eSymbolType | eBinaryOp | 26,
        eBar                        = eSymbolType | eBinaryOp | 27,
        eBarEqual                   = eSymbolType | eBinaryOp | 28,
        eBarBar                     = eSymbolType | eBinaryOp | 29,
        eCaret                      = eSymbolType | eBinaryOp | 30,
        eCaretEqual                 = eSymbolType | eBinaryOp | 31,
        eOpenPar                    = eSymbolType | 32,
        eClosePar                   = eSymbolType | 33,
        eOpenBracket                = eSymbolType | 34,
        eCloseBracket               = eSymbolType | 35,
        eOpenBrace                  = eSymbolType | 36,
        eCloseBrace                 = eSymbolType | 37,
        eComma                      = eSymbolType | 38,
        ePeriod                     = eSymbolType | 39,
        ePeriodPeriod               = eSymbolType | eBinaryOp | 40,
        ePeriodPeriodLess           = eSymbolType | eBinaryOp | 41,
        eSemiColon                  = eSymbolType | 42,
        eColon                      = eSymbolType | 43,
    };

    static constexpr uint16_t MAIN_TYPE_MASK = 0xf000;

    static constexpr EMainType GetMainType(EType type)
    {
        EMainType mainType = static_cast<EMainType>(MAIN_TYPE_MASK & type);
        return mainType;
    }

    static constexpr bool IsUnaryOp(EType type)
    {
        bool isSymbol = GetMainType(type) == eSymbolType;
        bool isUnaryOp = (type & eUnaryOp) == eUnaryOp;
        return isSymbol & isUnaryOp;
    }

    static constexpr bool IsBinaryOp(EType type)
    {
        bool isSymbol = GetMainType(type) == eSymbolType;
        bool isBinaryOp = (type & eBinaryOp) == eBinaryOp;
        return isSymbol & isBinaryOp;
    }

    static constexpr bool IsTypeName(EType type)
    {
        bool isKeyword = GetMainType(type) == eKeywordType;
        bool isTypeName = (type & eTypeName) == eTypeName;
        return isKeyword & isTypeName;
    }

    Token(ROString value, unsigned filenameId, unsigned line, unsigned column, EType type);

    ROString value;
    unsigned filenameId;
    unsigned line;
    unsigned column;
    EType type;
};

#endif // TOKEN_H_
