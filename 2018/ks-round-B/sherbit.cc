// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <functional>
#include <array>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;

static unsigned dbg_flags;

string u2str(ull_t x)
{
    string s = "";
    while (x > 0)
    {
        s.push_back("01"[x % 2]);
        x /= 2;
    }
    if (s.empty()) { s = string("0"); }
    string rs(s.rbegin(), s.rend());
    return rs;
}

class ABC
{
 public:
    ABC(u_t _a=0, u_t _b=0, u_t _c=0) : a(_a), b(_b), c(_c) {}
    bool Nlegal(u_t N, const ull_t n) const
    {
        u_t n1 = 0;
	for (u_t bi = a; bi <= b; ++bi)
	{
	    if ((n & (ull_t(1) << (N - bi))) != 0)
	    {
	        ++n1;
	    }
	}
	bool ret = (n1 == c);
	return ret;
    }
    u_t a, b, c;
};
bool operator<(const ABC& v0, const ABC& v1)
{
    bool lt = (v0.a < v1.a) || ((v0.a == v1.a) && (v0.b < v1.b));
    return lt;
}
typedef vector<ABC> vabc_t;

class Constraint
{
 public:
    Constraint(const au2_t& _be={0, 0}, u_t _nb=0) : be(_be), nbits(_nb) {}
    Constraint(const ABC& abc) : be{abc.a - 1, abc.b}, nbits(abc.c) {}
    u_t size() const { return be[1] - be[0]; }
    au2_t be;
    u_t nbits;
};
bool operator<(const Constraint& c0, const Constraint& c1)
{
    bool lt = (c0.be[1] < c1.be[1]);
         // || ((c0.be[1] || c1.be[1]) && (c0.be[0] < c1.be[0]));
    return lt;
}
typedef vector<Constraint> vconstraint_t;

struct CompConstraintVal
{
    bool operator()(const Constraint& c, u_t v) const
    {
        return c.be[1] < v;
    }
    bool operator()(u_t v, const Constraint& c) const
    {
        return v < c.be[1];
    }
};

class XL
{
 public:
    XL(u_t _x=0, u_t _last=0) : x(_x), last(_last) {}
    u_t x;
    u_t last;
};
bool operator==(const XL& xl0, const XL& xl1)
{
    bool eq = (xl0.x == xl1.x) && (xl0.last == xl1.last);
    return eq;
}

class HashXL {
 public:
  size_t operator()(const XL& xl) const noexcept {
    u_t u = (xl.x << 16) | xl.last;
    return hash<int>()(u);
  }
};
typedef unordered_map<XL, ull_t, HashXL> xl2ull_t;

class SherBit
{
 public:
    SherBit(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static const ull_t MAXP = 1000000000000000000 + 1; // 10^18 + 1
    bool legal(const ull_t n) const;
    void set_sconstraints();
    ull_t comp_pfx_nlegals(u_t x, u_t last);
    bool legal_segment(u_t x, u_t last) const;
    void build_solution();
    u_t N;
    u_t K;
    ull_t P;
    vabc_t constraints;
    vconstraint_t sconstraints;
    string solution;
    xl2ull_t pfx2_nlegals;
};

SherBit::SherBit(istream& fi)
{
    fi >> N >> K >> P;
    constraints.reserve(K);
    for (u_t ri = 0; ri != K; ++ri)
    {
        u_t a, b, c;
	fi >> a >> b >> c;
        constraints.push_back(ABC{a, b, c});
    }
}

void SherBit::solve_naive()
{
    for (ull_t n = 0, n_legal = 0; (n_legal < P) && (n < (1u << N)); ++n)
    {
        if (legal(n))
        {
            ++n_legal;
            if (n_legal == P)
            {
                solution = n;
                string s = u2str(n);
                size_t nz = N - s.size();
                string zs(nz, '0');
                solution = zs + s;
            }
        }
    }
    if (solution.empty())
    {
        cerr << "Illegal case\n";
        exit(1);
    }
}

bool SherBit::legal(const ull_t n) const
{
    bool ret = true;
    for (vabc_t::const_iterator i = constraints.begin(), e = constraints.end();
        ret && (i != e); ++i)
    {
        const ABC& constraint = *i;
	ret = constraint.Nlegal(N, n);
    }
    return ret;
}

void SherBit::solve()
{
    set_sconstraints();
    if (comp_pfx_nlegals(0, 0) < P)
    {
        comp_pfx_nlegals(0, 1);
    }
    build_solution();
}

void SherBit::set_sconstraints()
{
    sconstraints = vconstraint_t{constraints.begin(), constraints.end()};
    sort(sconstraints.begin(), sconstraints.end());
}

// How many starting upto NOT including x, and last is bitwise preceding
ull_t SherBit::comp_pfx_nlegals(u_t x, u_t last)
{
    ull_t ret = 0;
    XL key(x, last);
    xl2ull_t::iterator iter = pfx2_nlegals.find(key);
    if (iter == pfx2_nlegals.end())
    {
        if (legal_segment(x, last))
        {
            if (x == N - 1)
            {
                ret = 1;
            }
            else
            {
                const u_t sub_last0 = (x < 15 ? last : last >> 1);
                for (u_t bit = 0; (bit < 2) && (ret < P); ++bit)
                {
                    u_t sub_last = sub_last0 | (bit << min<u_t>(x + 1, 15));
                    ret += comp_pfx_nlegals(x + 1, sub_last);
                }
            }
        }
        xl2ull_t::value_type v(key, ret);
        pfx2_nlegals.insert(iter, v);
    }
    else
    {
        ret = iter->second;
    }
    return ret;
}

// upto including x, last covers including upto it 
bool SherBit::legal_segment(u_t x, u_t last) const
{
    bool legal = true;
    const u_t xmin = (x > 15 ? x - 15 : 0);
    auto er = equal_range(sconstraints.begin(), sconstraints.end(), x + 1,
        CompConstraintVal());
    for (vconstraint_t::const_iterator iter = er.first;
        legal && (iter != er.second); ++iter)
    {
        const Constraint& sc = *iter;
        if (xmin <= sc.be[0])
        {
            u_t nbits = 0;
            for (u_t bi = sc.be[0] - xmin, bie = sc.be[1] - xmin; bi < bie;
                ++bi)
            {
                nbits += (last & (1u << bi)) ? 1 : 0;
            }
            legal = (nbits == sc.nbits);
        }
    }
    return legal;
}

void SherBit::build_solution()
{
    u_t last = 0;
    ull_t pending_legal = P;
    for (u_t bi = 0; bi < N; ++bi)
    {
        u_t bit = 0;
        xl2ull_t::const_iterator iter = pfx2_nlegals.find(XL(bi, last));
        ull_t n_legals = ((iter == pfx2_nlegals.end()) ? 0 : iter->second);
        if (n_legals < pending_legal)
        {
            bit = 1;
            pending_legal -= n_legals;
        }
        solution.push_back("01"[bit]);
        last |= (bit << min<u_t>(bi, 15));
        if (bi >= 15)
        {
            last >>= 1;
        }
    }
}

void SherBit::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (SherBit::*psolve)() =
        (naive ? &SherBit::solve_naive : &SherBit::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        SherBit sherbit(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (sherbit.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        sherbit.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
