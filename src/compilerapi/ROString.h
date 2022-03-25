#pragma once

#include <cstring>

class ROString
{
public:
    ROString()
    {
        ptr = nullptr;
        size = 0;
    }

    ROString(const char* ptr, size_t size)
    {
        this->ptr = ptr;
        this->size = size;
    }

    size_t GetSize() const
    {
        return size;
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
