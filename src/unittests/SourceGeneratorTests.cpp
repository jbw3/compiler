#include "SourceGenerator.h"
#include "SourceGeneratorTests.h"

SourceGeneratorTests::SourceGeneratorTests()
{
    AddTest(TestSourceGenerator);
}

bool SourceGeneratorTests::TestSourceGenerator()
{
    SourceGenerator generator;

    return true;
}
