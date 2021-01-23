// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
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

typedef unordered_map<u_t, ull_t> u2ull_t;
typedef vector<u2ull_t> vu2ull_t;

class SherBit
{
 public:
    SherBit(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static const ull_t MAXP = 1000000000000000000 + 1; // 10^18 + 1
    enum {BMA_MAX = 15}; // max(B - a)
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
    vu2ull_t xpfx_nlegals;
    vu_t b_calls;
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
    xpfx_nlegals = vu2ull_t(size_t(N + 1), u2ull_t());
    b_calls = vu_t(size_t(N + 1), 0);
    comp_pfx_nlegals(0, 0);
    if (dbg_flags & 0x1) {
        for (u_t x = 0; x < N + 1; ++x) {
            cerr << "#H(" << x << ")=" << xpfx_nlegals[x].size() << 
               ", calls=" << b_calls[x] << '\n';
            if (dbg_flags & 0x2) {
                for (const u2ull_t::value_type& xl: xpfx_nlegals[x]) {
                    cerr << "  " << xl.first << " = b" << u2str(xl.first) <<
                        ": " << xl.second << '\n';
                }
            }
        }
    }
    build_solution();
}

void SherBit::set_sconstraints()
{
    sconstraints = vconstraint_t{constraints.begin(), constraints.end()};
    sort(sconstraints.begin(), sconstraints.end());
}

// How many starting atg x, and last is bitwise preceding upto EXcluding x
// last bit-"size" = BMA_MAX
ull_t SherBit::comp_pfx_nlegals(u_t x, u_t last)
{
    ull_t ret = 0;
    ++b_calls[x];
    u2ull_t::iterator iter = xpfx_nlegals[x].find(last);
    if (iter == xpfx_nlegals[x].end())
    {
        for (u_t bit = 0; (bit < 2) && (ret < P); ++bit)
        {
            const u_t sub_last_bit = last | (bit << min<u_t>(x, BMA_MAX));
            if (legal_segment(x, sub_last_bit))
            {
                    const u_t sub_last =
                        (x < BMA_MAX ? sub_last_bit : sub_last_bit >> 1);
                if (x == N - 1)
                {
                    ret += 1;
                    u2ull_t& xpfxN = xpfx_nlegals[N];
                    xpfxN.insert(xpfxN.end(), u2ull_t::value_type(sub_last, 1));
                }
                else
                {
                    ret += comp_pfx_nlegals(x + 1, sub_last);
                }
            }
        }
        xpfx_nlegals[x].insert(iter, u2ull_t::value_type(last, ret));
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
    const u_t xmin = (x > BMA_MAX ? x - BMA_MAX : 0);
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
    for (u_t bi = 0, bi1 = 1; bi < N; bi = bi1++)
    {
        u_t bit = 0;
        u2ull_t::const_iterator iter = xpfx_nlegals[bi1].find(last);
        ull_t n_legals = ((iter == xpfx_nlegals[bi1].end()) ? 0 : iter->second);
        if (n_legals < pending_legal)
        {
            bit = 1;
            pending_legal -= n_legals;
        }
        solution.push_back("01"[bit]);
        last |= (bit << min<u_t>(bi, BMA_MAX - 1));
        if (bi >= BMA_MAX - 1)
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
