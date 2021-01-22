#include "CompilerContext.h"
#include <stdlib.h>

using namespace std;

CompilerContext::CompilerContext()
{
}

unsigned CompilerContext::AddFilename(const string& filename)
{
    filenames.push_back(filename);
    return static_cast<unsigned>(filenames.size());
}
