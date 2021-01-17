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
        eIdentifierType  = 1 << 8,
        eKeywordType     = 2 << 8,
        eBoolLiteralType = 3 << 8,
        eIntLiteralType  = 4 << 8,
        eStrLiteralType  = 5 << 8,
        eSymbolType      = 6 << 8,
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
        eEqual                      = eSymbolType |  0,
        eEqualEqual                 = eSymbolType |  1,
        eLess                       = eSymbolType |  2,
        eLessEqual                  = eSymbolType |  3,
        eLessLess                   = eSymbolType |  4,
        eLessLessEqual              = eSymbolType |  5,
        eGreater                    = eSymbolType |  6,
        eGreaterEqual               = eSymbolType |  7,
        eGreaterGreater             = eSymbolType |  8,
        eGreaterGreaterEqual        = eSymbolType |  9,
        eGreaterGreaterGreater      = eSymbolType | 10,
        eGreaterGreaterGreaterEqual = eSymbolType | 11,
        ePlus                       = eSymbolType | 12,
        ePlusEqual                  = eSymbolType | 13,
        eMinus                      = eSymbolType | 14,
        eMinusEqual                 = eSymbolType | 15,
        eTimes                      = eSymbolType | 16,
        eTimesEqual                 = eSymbolType | 17,
        eDivide                     = eSymbolType | 18,
        eDivideEqual                = eSymbolType | 19,
        eRemainder                  = eSymbolType | 20,
        eRemainderEqual             = eSymbolType | 21,
        eExclaim                    = eSymbolType | 22,
        eExclaimEqual               = eSymbolType | 23,
        eAmpersand                  = eSymbolType | 24,
        eAmpersandEqual             = eSymbolType | 25,
        eAmpersandAmpersand         = eSymbolType | 26,
        eBar                        = eSymbolType | 27,
        eBarEqual                   = eSymbolType | 28,
        eBarBar                     = eSymbolType | 29,
        eCaret                      = eSymbolType | 30,
        eCaretEqual                 = eSymbolType | 31,
        eOpenPar                    = eSymbolType | 32,
        eClosePar                   = eSymbolType | 33,
        eOpenBracket                = eSymbolType | 34,
        eCloseBracket               = eSymbolType | 35,
        eOpenBrace                  = eSymbolType | 36,
        eCloseBrace                 = eSymbolType | 37,
        eComma                      = eSymbolType | 38,
        ePeriod                     = eSymbolType | 39,
        ePeriodPeriod               = eSymbolType | 40,
        ePeriodPeriodLess           = eSymbolType | 41,
        eSemiColon                  = eSymbolType | 42,
        eColon                      = eSymbolType | 43,
    };

    static constexpr uint16_t MAIN_TYPE_MASK = 0xff00;

    static constexpr EMainType GetMainType(EType type)
    {
        EMainType mainType = static_cast<EMainType>(MAIN_TYPE_MASK & type);
        return mainType;
    }

    Token(const std::string& value, const std::string& filename, unsigned line, unsigned column, EType type);

    std::string value;
    std::string filename;
    unsigned line;
    unsigned column;
    EType type;
};

#endif // TOKEN_H_
