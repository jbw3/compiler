#include "SourceGenerator.h"
#include <fstream>
#include <iostream>

using namespace std;

SourceGenerator::SourceGenerator(const string& outFilename)
{
    os = outFilename.empty() ? &cout : new fstream(outFilename, ios_base::out);
}

SourceGenerator::~SourceGenerator()
{
    if (os != &cout)
    {
        delete os;
    }
}

void SourceGenerator::Flush()
{
    os->flush();
}
