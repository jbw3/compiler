#ifndef TOKEN_H_
#define TOKEN_H_

#include <string>

class Token
{
public:
    static const Token* None;

    // types
    enum EMainType : uint16_t
    {
        eIdentifierType  = 0x1000,
        eKeywordType     = 0x2000,
        eBoolLiteralType = 0x3000,
        eIntLiteralType  = 0x4000,
        eStrLiteralType  = 0x5000,
        eSymbolType      = 0x6000,
    };

    enum ESymbolFlags : uint16_t
    {
        eUnaryOp  = 0x0800,
        eBinaryOp = 0x0400,
    };

    enum EType : uint16_t
    {
        eInvalid      = 0,

        // identifier
        eIdentifier   = eIdentifierType,

        // keyword
        eBool         = eKeywordType |  0,
        eBreak        = eKeywordType |  1,
        eContinue     = eKeywordType |  2,
        eElif         = eKeywordType |  3,
        eElse         = eKeywordType |  4,
        eExtern       = eKeywordType |  5,
        eFor          = eKeywordType |  6,
        eFun          = eKeywordType |  7,
        eIf           = eKeywordType |  8,
        eI8           = eKeywordType |  9,
        eI16          = eKeywordType | 10,
        eI32          = eKeywordType | 11,
        eI64          = eKeywordType | 12,
        eIn           = eKeywordType | 13,
        eISize        = eKeywordType | 14,
        eReturn       = eKeywordType | 15,
        eStr          = eKeywordType | 16,
        eStruct       = eKeywordType | 17,
        eU8           = eKeywordType | 18,
        eU16          = eKeywordType | 19,
        eU32          = eKeywordType | 20,
        eU64          = eKeywordType | 21,
        eUSize        = eKeywordType | 22,
        eVar          = eKeywordType | 23,
        eWhile        = eKeywordType | 24,

        // bool literal
        eFalseLit     = eBoolLiteralType | 0,
        eTrueLit      = eBoolLiteralType | 1,

        // int literal
        eBinIntLit    = eIntLiteralType | 0,
        eOctIntLit    = eIntLiteralType | 1,
        eDecIntLit    = eIntLiteralType | 2,
        eHexIntLit    = eIntLiteralType | 3,

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

    static constexpr uint16_t MAIN_TYPE_MASK = 0xff00;

    static constexpr EMainType GetMainType(EType type)
    {
        EMainType mainType = static_cast<EMainType>(MAIN_TYPE_MASK & type);
        return mainType;
    }

    static constexpr bool IsUnaryOp(EType type)
    {
        constexpr uint16_t mask = eSymbolType | eUnaryOp;
        bool value = (type & mask) == mask;
        return value;
    }

    static constexpr bool IsBinaryOp(EType type)
    {
        constexpr uint16_t mask = eSymbolType | eBinaryOp;
        bool value = (type & mask) == mask;
        return value;
    }

    Token(const std::string& value, const std::string& filename, unsigned line, unsigned column, EType type);

    std::string value;
    std::string filename;
    unsigned line;
    unsigned column;
    EType type;
};

#endif // TOKEN_H_
