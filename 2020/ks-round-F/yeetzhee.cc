// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iterator>
#include <map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

static const string vu_to_str(const vu_t& a)
{
    ostringstream os;
    os << '{';
    const char* sep = "";
    for (u_t x: a)
    {
        os << sep << x; sep = " ";
    }
    os << '}';
    string ret = os.str();
    return ret;
}

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

typedef map<vu_t, Frac> vu2frac_t;
typedef map<vu_t, double> vu2dbl_t;

class Yeetzhee
{
 public:
    Yeetzhee(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void grow(vu_t& gsub, const vu_t& sub, u_t side) const;
    bool possible_grow(vu_t& gsub, const vu_t& sub, u_t grow) const;
    // bool row(vu_t& gsub, const vu_t& sub, u_t grow) const;
    bool possible(const vu_t& sub) const;
    void frac_expectation(Frac& e, const vu_t& sub);
    double dbl_expectation(const vu_t& sub);
    u_t n, m, k;
    vu_t a;
    double solution;
    vu2frac_t memo;
    vu2dbl_t dmemo;
};

Yeetzhee::Yeetzhee(istream& fi) : solution(-1)
{
    fi >> n >> m >> k;
    copy_n(istream_iterator<u_t>(fi), k, back_inserter(a));
}

void Yeetzhee::solve_naive()
{
    Frac e;
    frac_expectation(e, vu_t());
    solution = e.dbl();
    if (dbg_flags & 0x2) { 
       cerr << "nmk=(" << n << ", " << m << ", " << k << "), a=" << 
       vu_to_str(a) << ", e=" << e.str() << '\n'; }
}

void Yeetzhee::frac_expectation(Frac& e, const vu_t& sub)
{
    auto er = memo.equal_range(sub);
    vu2frac_t::iterator iter = er.first;
    if (er.first == er.second)
    {
        e = Frac::zero;
        u_t total = accumulate(sub.begin(), sub.end(), 0);
        if (total < n)
        {
            u_t n_good = 0;
            const Frac mfrac(m);
            Frac etotal(Frac::zero);
            Frac eside;
            vu_t gsub;
            const u_t sub_sz = sub.size();
            for (u_t side = 0; side < sub_sz; ++side)
            {
                if (possible_grow(gsub, sub, side))
                {
                    frac_expectation(eside, gsub);
                    ++n_good;
                    Frac::add(etotal, etotal, eside);            
                }
            }
            if (sub_sz < k)
            {
                u_t n_new = m - sub_sz;
                grow(gsub, sub, sub_sz);
                frac_expectation(eside, gsub);
                Frac::mult(eside, Frac(n_new), eside);            
                Frac::add(etotal, etotal, eside);            
                n_good += n_new;
            }
            Frac::div(etotal, etotal, Frac(n_good));
            Frac exp_next(m, n_good);
            Frac::add(e, exp_next, etotal);
        }
        vu2frac_t::value_type v{sub, e};
        memo.insert(iter, v);
        if (dbg_flags & 0x1) { cerr<<vu_to_str(sub)<<", exp="<<e.dbl()<<'\n'; } 
    }
    else
    {
        e = iter->second;
    }
}

void Yeetzhee::grow(vu_t& gsub, const vu_t& sub, u_t side) const
{
    gsub = sub;
    if (side < gsub.size())
    {
        ++gsub[side];
    }
    else
    {
        gsub.push_back(1);
    }
    sort(gsub.begin(), gsub.end());
}

bool Yeetzhee::possible_grow(vu_t& gsub, const vu_t& sub, u_t side) const
{
    grow(gsub, sub, side);
    return possible(gsub);
}

bool Yeetzhee::possible(const vu_t& sub) const
{
    bool p = true;
    for (u_t si = 0, ai = 0; p && (si < sub.size()) && (ai < a.size());
        ++ai, ++si)
    {
        for ( ; (ai < a.size()) && (sub[si] > a[ai]); ++ai)
        {}
        p = (ai < a.size());
    }
    return p;
}

void Yeetzhee::solve()
{
    // solve_naive();
    solution = dbl_expectation(vu_t());
}

double Yeetzhee::dbl_expectation(const vu_t& sub)
{
    double e = 0.;
    auto er = dmemo.equal_range(sub);
    vu2dbl_t::iterator iter = er.first;
    if (er.first == er.second)
    {
        u_t total = accumulate(sub.begin(), sub.end(), 0);
        if (total < n)
        {
            u_t n_good = 0;
            double etotal = 0.;
            double eside;
            vu_t gsub;
            const u_t sub_sz = sub.size();
            for (u_t side = 0; side < sub_sz; ++side)
            {
                if (possible_grow(gsub, sub, side))
                {
                    eside = dbl_expectation(gsub);
                    ++n_good;
                    etotal += eside;
                }
            }
            if (sub_sz < k)
            {
                u_t n_new = m - sub_sz;
                grow(gsub, sub, sub_sz);
                eside = dbl_expectation(gsub);
                eside *= n_new;
                etotal += eside;
                n_good += n_new;
            }
            etotal /= n_good;
            double exp_next = double(m) / double(n_good);
            e = exp_next + etotal;
        }
        vu2dbl_t::value_type v{sub, e};
        dmemo.insert(iter, v);
        if (dbg_flags & 0x1) { cerr<<vu_to_str(sub)<<", exp="<<e<<'\n'; } 
    }
    else
    {
        e = iter->second;
    }
    return e;
}

void Yeetzhee::print_solution(ostream &fo) const
{
    ostringstream os;
    os.precision(8);
    os.setf(ios::fixed);
    os << solution;
    string s = os.str();
    while (s.back() == '0')
    {
        s.pop_back();
    }
    fo << ' ' << s;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
        }
        else
        {
            cerr << "Bad option: " << opt << "\n";
            return 1;
        }
    }

    int ai_in = ai;
    int ai_out = ai + 1;
    istream *pfi = (argc <= ai_in || (string(argv[ai_in]) == dash))
         ? &cin
         : new ifstream(argv[ai_in]);
    ostream *pfo = (argc <= ai_out || (string(argv[ai_out]) == dash))
         ? &cout
         : new ofstream(argv[ai_out]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Yeetzhee::*psolve)() =
        (naive ? &Yeetzhee::solve_naive : &Yeetzhee::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Yeetzhee yeetzhee(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (yeetzhee.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        yeetzhee.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
