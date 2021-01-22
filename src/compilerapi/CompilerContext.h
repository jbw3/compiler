#ifndef COMPILER_CONTEXT_H_
#define COMPILER_CONTEXT_H_

#include <string>
#include <vector>

class CompilerContext
{
public:
    CompilerContext();

    unsigned AddFilename(const std::string& filename);

    const std::string& GetFilename(unsigned id) const
    {
        return filenames[id];
    }

private:
    std::vector<std::string> filenames;
};

#endif // COMPILER_CONTEXT_H_
