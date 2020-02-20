#include <string>
#include <string>
#include <cstdio>
// #include <sstream>

using namespace std;
typedef unsigned u_t;

string dtos(const double x, u_t precision=6)
{
    char buf[0x20];
    snprintf(buf, sizeof(buf), "%.*f", precision, x);
    string ret(buf);
    return ret;
}

#if 0
string dtos(const double x, u_t precision=6)
{
    stringstream os;
    os.precision(precision);
    os << x;
    string ret = os.str();
    size_t sz = ret.size();
    size_t p = ret.find('.');
    if (p == string::npos)
    {
        ret.push_back('.');
        p = sz++;
    }
    size_t nz = sz - p - 1; // current
    nz = precision - nz; // to add
    ret.insert(sz, nz, '0');
    return ret;
}
#endif

#include <iostream>
int main(int argc, char** argv)
{
    for (int ai = 1; ai < argc; ++ai)
    {
        const char *a = argv[ai];
        double x = stod(a);
        cout << "dtos(" << a << ") = " << dtos(x) << '\n';
    }
    return 0;
}
