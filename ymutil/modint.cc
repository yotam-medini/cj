#include <iostream>

using namespace std;

typedef long long ll_t;
typedef unsigned long long ull_t;

template <ull_t mod>
class ModInt
{
 public:
    typedef ModInt<mod> self_t;
    ModInt(ll_t _n=0) : n((_n >= 0 ? _n : mod - (-_n) % mod) % mod) {}
    static constexpr std::uint32_t get_mod() { return mod; }
    ull_t get() const { return n; }
    static self_t add(const self_t& a, const self_t& b)
    {
        return self_t(a.n + b.n);
    }
    static self_t sub(const self_t& a, const self_t& b)
    {
        return self_t((a.n + mod) - b.n);
    }
    static self_t mul(const self_t& a, const self_t& b)
    {
        return self_t(a.n * b.n);
    }
    static self_t div(const self_t& a, const self_t& b)
    {
        return mul(a, b.inv());
    }
    static bool eq(const self_t& a, const self_t& b)
    {
        return a.n == b.n;
    }
    self_t pow(ull_t p) const
    {
        self_t ret = 1;
        self_t m = *this;;
        while (p)
        {
            if (p & 0x1)
            {
                ret = ret * m;
            }
            m = m * m;
            p /= 2;
        }
        return ret;
    }
    self_t inv() const
    {
        return pow(mod - 2); // since n^{mod - 1} == 1
    }
 private:
    ull_t n;
};

template <ull_t mod>
ostream& operator<<(ostream& os, const ModInt<mod>& a)
{
    return os << a.get();
}

template <ull_t mod> ModInt<mod>
operator+(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::add(a, b);
}

template <ull_t mod> ModInt<mod>
operator-(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::sub(a, b);
}

template <ull_t mod> ModInt<mod>
operator*(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::mul(a, b);
}

template <ull_t mod> ModInt<mod>
operator/(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::div(a, b);
}

template <ull_t mod> bool
operator==(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return ModInt<mod>::eq(a, b);
}

template <ull_t mod> bool
operator!=(const ModInt<mod>& a, const ModInt<mod>& b)
{
    return !(a == b);
}

typedef ModInt<1000000007> gmodint_t;

#include <iostream>
#include <cstdlib>

int main(int argc, char** argv)
{
    int rc = 0;
    ll_t _a = strtoul(argv[1], nullptr, 0);
    ll_t _b = strtoul(argv[2], nullptr, 0);
    gmodint_t a(_a);
    gmodint_t b(_b);
    cout << "a=" << a << ", b=" << b << '\n';
    cout << "a + b = " << a + b << '\n';
    cout << "a - b = " << a - b << '\n';
    cout << "b - a = " << b - a << '\n';
    cout << "a * b = " << a * b << '\n';
    cout << "a / b = " << a / b << '\n';
    cout << "b / a = " << b / a << '\n';
    cout << "(1/a) = " << a.inv() << '\n';
    cout << "(1/b) = " << b.inv() << '\n';
    cout << "a ** b = " << a.pow(_b) << '\n';
    cout << "b ** a = " << b.pow(_a) << '\n';
    for (_a = -10; (rc == 0) && (_a <= 10); ++_a)
    {
        a = gmodint_t(_a);
        for (_b = -10; (rc == 0) && (_b <= 10); ++_b)
        {
            b = gmodint_t(_b);
            if (a + b != b + a) { rc = 1; cerr<<"fail @ "<<__LINE__<<'\n'; }
            if ((a - b) + b != a) { rc = 1; cerr<<"fail @ "<<__LINE__<<'\n'; }
            if ((a * b) != b * a) { rc = 1; cerr<<"fail @ "<<__LINE__<<'\n'; }
            if (b != gmodint_t(0))
            {
                if ((a / b) * b !=  a)
                { 
                    rc = 1; cerr<<"fail @ "<<__LINE__<<'\n';
                }
            }
        }
    }
    return rc;
}
