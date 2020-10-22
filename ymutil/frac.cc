#include <iostream>
#include <sstream>

using namespace std;

typedef unsigned u_t;
typedef long long ll_t;
typedef unsigned long long ull_t;

class Frac
{
 public:
    Frac(ull_t _n=0, ull_t _d=1, int _sign=1): n(_n), d(_d),
       sign(_sign == 1 ? 1 : -1) 
    {
        reduce();
    }
    double dbl() const 
    { 
        double ret = double(n) / double(d);
        if (sign != 1) { ret = -ret; }
        return ret;
    }
    static Frac& add(Frac& res, const Frac& q0, const Frac& q1)
    {
        ll_t rn = q0.snum() * q1.sdenom() + q1.snum() * q0.sdenom();
        int rs = 1;
        if (rn < 0)
        {
            rn = -rn;
            rs = -1;
        }
        res = Frac(rn, q0.d * q1.d, rs);
        return res;
    };
    static Frac& sub(Frac& res, const Frac& q0, const Frac& q1)
    {
        Frac minus_q1;
        mult(minus_q1, minus_one, q1);
        return add(res, q0, minus_q1);
    };
    static Frac& mult(Frac& res, const Frac& q0, const Frac& q1)
    {
        res = Frac(q0.n * q1.n, q0.d * q1.d, q0.sign * q1.sign);
        return res;
    };
    static Frac& div(Frac& res, const Frac& q0, const Frac& q1)
    {
        res = Frac(q0.n * q1.d, q0.d * q1.n, q0.sign * q1.sign);
        return res;
    };
    bool strset(const string& s);
    string str() const;
    static const Frac zero, one, minus_one;
 private:
    static ull_t gcd(ull_t x0, ull_t x1);
    ll_t snum() const
    {
        ll_t ret = n;
        if (sign != 1) { ret = -ret; }
        return ret;
    }
    ll_t sdenom() const { return d; }
    void reduce()
    {
        ull_t g = gcd(n, d);
        n /= g;
        d /= g;
    }
    ull_t n, d;
    int sign;
};

const Frac Frac::zero(0);
const Frac Frac::one(1);
const Frac Frac::minus_one(1, 1, -1);

ull_t Frac::gcd(ull_t x0, ull_t x1)
{
    while (x1 != 0)
    {
        ull_t r = x0 % x1;
        x0 = x1;
        x1 = r;
    }
    return x0;
}

bool Frac::strset(const string& s)
{
    bool ok = true;
    string sub(s);
    sign = 1;
    if (sub[0] == '-')
    {
        sign = -1;
        sub = s.substr(1);
    }
    size_t pnext;
    n = stoi(sub, &pnext);
    d = 1;
    if (sub[pnext] == '/')
    {
        ++pnext;
        sub = sub.substr(pnext);
        d = stoi(sub);
    }
    ok = (d > 0);
    if (ok) { reduce(); }
    return ok;
}

string Frac::str() const
{
    ostringstream os;
    os << '(' << (sign == 1 ? "" : "-") << n;
    if (d != 1)
    {
         os << '/' << d;
    }
    os << ')';
    string ret = os.str();
    return ret;
}

void usage(const char* prog)
{
    cerr << "usgae: " << prog << "<q1> <op> <q2>\n";
}

int main(int argc, char **argv)
{
    int rc = (argc == 4 ? 0 : 1);
    if (rc != 0)
    {
        usage(argv[0]);
    }
    else
    {
        Frac r, a, b;
        a.strset(argv[1]);
        const char op = argv[2][0];
        b.strset(argv[3]);
        switch (op)
        {
         case '+': Frac::add(r, a, b); break;
         case '-': Frac::sub(r, a, b); break;
         case '*': Frac::mult(r, a, b); break;
         case '/': Frac::div(r, a, b); break;
         default:
            cerr << "Bad op: " << argv[2] << '\n';
            usage(argv[0]);
            rc = 1;
        }
        if (rc == 0)
        {
            cout << a.str() << ' ' << op << ' ' << b.str() << 
                " = " << r.str() << "\n";
        }
    }
    return rc;
}
