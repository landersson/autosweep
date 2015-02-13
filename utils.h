#ifndef UTILS_H_
#define UTILS_H_

#include <string>

namespace utils
{

// return values for the str2int() function
enum class Str2IntResult 
{ 
    S2I_SUCCESS, 
    S2I_OVERFLOW, 
    S2I_UNDERFLOW, 
    S2I_INCONVERTIBLE 
};


 // return random integer in the range [a-b] (including b)
int rand_int(int a, int b);

// return string representation of given integer
std::string int2str(int n);

double factorial(int n);

double binomialCoefficientApprox(double n, double k);
double binomialCoefficient(double n, double k);

std::string int2str(int n);

Str2IntResult str2int(int &i, const std::string & str, int base = 0);

} // namespace utils

#endif // UTILS_H_

