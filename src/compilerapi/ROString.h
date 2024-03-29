#pragma once

#include <cstring>
#include <ostream>

class ROString
{
public:
    ROString()
    {
        ptr = nullptr;
        size = 0;
    }

    ROString(const char* cStr)
    {
        ptr = cStr;
        size = strlen(cStr);
    }

    ROString(const char* ptr, size_t size)
    {
        this->ptr = ptr;
        this->size = size;
    }

    const char* GetPtr() const
    {
        return ptr;
    }

    size_t GetSize() const
    {
        return size;
    }

    char operator [](size_t index) const
    {
        return ptr[index];
    }

    bool operator ==(const ROString& other) const
    {
        if (size != other.size)
        {
            return false;
        }
        return memcmp(ptr, other.ptr, size) == 0;
    }

    bool operator !=(const ROString& other) const
    {
        if (size != other.size)
        {
            return true;
        }
        return memcmp(ptr, other.ptr, size) != 0;
    }

private:
    const char* ptr;
    size_t size;
};

inline std::ostream& operator <<(std::ostream& os, const ROString& str)
{
    os.write(str.GetPtr(), str.GetSize());
    return os;
}

namespace std
{
    template<>
    struct hash<ROString>
    {
        std::size_t operator()(const ROString& str) const
        {
            size_t hash = 0;

            const char* strPtr = str.GetPtr();
            size_t strSize = str.GetSize();
            for (size_t i = 0; i < strSize; ++i)
            {
                hash ^= strPtr[i];
                hash *= 31;
            }

            return hash;
        }
    };
}
