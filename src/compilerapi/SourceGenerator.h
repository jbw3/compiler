#ifndef SOURCE_GENERATOR_H_
#define SOURCE_GENERATOR_H_

#include <string>

class SourceGenerator
{
public:
    SourceGenerator(const std::string& outFilename);

    ~SourceGenerator();

    void Flush();

private:
    std::ostream* os;
};

#endif // SOURCE_GENERATOR_H_
