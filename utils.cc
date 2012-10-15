
#include "utils.h"

#include <cassert>
#include <cstdlib>
#include <sstream>

int rand_int(int a, int b)
{
    assert(a <= b);

    return rand() % (b - a) + a;
}

std::string int2str(int n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}
