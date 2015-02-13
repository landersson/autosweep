
#include "utils.h"

#include <cassert>
#include <cerrno>
#include <cstdlib>
#include <sstream>

#include <stdint.h>
#include <limits.h>

namespace utils
{

int rand_int(int a, int b)
{
    assert(a <= b);

    return rand() % (b + 1 - a) + a;
}

Str2IntResult str2int (int &i, const std::string & str, int base)
{
    errno = 0;

    char *end;
    const char* s = str.c_str();

    long l = std::strtol(s, &end, base);
    if ((errno == ERANGE && l == LONG_MAX) || l > INT_MAX) {
        return Str2IntResult::S2I_OVERFLOW;
    }
    if ((errno == ERANGE && l == LONG_MIN) || l < INT_MIN) {
        return Str2IntResult::S2I_UNDERFLOW;
    }
    if (*s == '\0' || *end != '\0') {
        return Str2IntResult::S2I_INCONVERTIBLE;
    }
    i = l;
    return Str2IntResult::S2I_SUCCESS;
}

std::string int2str(int n)
{
    std::stringstream ss;
    ss << n;
    return ss.str();
}

double factorial(int n)
{
    if (n == 0) return 1.0;
    double f = n--;
    while(n > 1)
    {
        f *= n--;
    }
    return f;
}

double binomial(double n, double k)
{
    double r = 1;
    if ((n - k) < k)
    {
        for (int i = 1; i <= (n - k); i++)
        {
            r *= (1.0 + k/i);
        }
    }
    else
    {
        for (int i = 1; i <= k; i++)
        {
            r *= (1.0 + double(n - k) / i);
        }
    }
    return r;
}

double binomialCoefficient(double n, double k)
{
    if (k > n) return 0;
    if (k == 0) return 1;
    if (k == 1) return n;
    return factorial(n) / (factorial(k) / factorial(n - k));
}


} // namespace utils
