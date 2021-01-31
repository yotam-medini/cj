// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
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
typedef vector<vconstraint_t> vvconstraint_t;

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
    // enum {BMA_MAX = 3}; // max(B - a)
    typedef array<ll_t, (1<<BMA_MAX)> aibma_t;
    typedef vector<aibma_t> vaibma_t;
    bool legal(const ull_t n) const;
    void set_sconstraints();
    ull_t comp_pfx_nlegals(u_t x, const u_t last);
    bool legal_segment(u_t x, u_t last) const;
    void build_solution();
    u_t N;
    u_t K;
    ull_t P;
    vabc_t constraints;
    vvconstraint_t sconstraints;
    string solution;
    vaibma_t xpfx_nlegals;
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
    if (BMA_MAX != 15) {
        cerr << "!!! Warning BMA_MAX="<< BMA_MAX << " != 15\n";
    }
    set_sconstraints();
    static aibma_t pfx_nlegals_undef;
    if (pfx_nlegals_undef[0] == 0)
    {
        if (dbg_flags & 0x1) { cerr << "pfx_nlegals_undef.size = " <<
            pfx_nlegals_undef.size() << '\n';
        }
        fill(pfx_nlegals_undef.begin(), pfx_nlegals_undef.end(), -1);
    }
    xpfx_nlegals = vaibma_t(size_t(N + 1), pfx_nlegals_undef);
    b_calls = vu_t(size_t(N + 1), 0);
    comp_pfx_nlegals(0, 0);
    if (dbg_flags & 0x1) {
        for (u_t x = 0; x < N + 1; ++x) {
            u_t sz = 0;
            for (ll_t v: xpfx_nlegals[x]) { if (v >= 0) { ++sz; } }
            cerr << "#H(" << x << ")=" << sz << 
                ", calls=" << b_calls[x] << '\n';
            if (dbg_flags & 0x2) {
                for (u_t last = 0; last < 1u << BMA_MAX; ++last) {
                    ll_t v = xpfx_nlegals[x][last];
                    if (v >= 0) {
                        cerr << "  " << last << " = b" << u2str(last) <<
                        ": " << v << '\n';
                    }
                }
            }
        }
    }
    build_solution();
}

void SherBit::set_sconstraints()
{
    sconstraints = vvconstraint_t(size_t(N), vconstraint_t());
    for (const ABC& abc: constraints)
    {
        const Constraint constraint(abc);
        sconstraints[constraint.be[1] - 1].push_back(constraint);
    }
}

// How many starting atg x, and last is bitwise preceding upto EXcluding x
// last bit-"size" = BMA_MAX
ull_t SherBit::comp_pfx_nlegals(u_t x, const u_t last)
{
    ++b_calls[x];
    ull_t ret = 0;
    ll_t sret = xpfx_nlegals[x][last];
    if (sret < 0)
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
                    // xpfx_nlegals[N][sub_last] = 1;
                }
                else
                {
                    ret += comp_pfx_nlegals(x + 1, sub_last);
                }
            }
        }
        xpfx_nlegals[x][last] = ret;
    }
    else
    {
        ret = sret;
    }
    return ret;
}

// upto including x, last covers including upto it (BMA_MAX + 1) bits
bool SherBit::legal_segment(u_t x, u_t last) const
{
    bool legal = true;
    const u_t xmin = (x > BMA_MAX ? x - BMA_MAX : 0);
    for (vconstraint_t::const_iterator 
        iter = sconstraints[x].begin(), eiter = sconstraints[x].end();
        legal && (iter != eiter); ++iter)
    {
        const Constraint& sc = *iter;
        u_t nbits = 0;
        for (u_t bi = sc.be[0] - xmin, bie = sc.be[1] - xmin; bi < bie;
            ++bi)
        {
            nbits += (last & (1u << bi)) ? 1 : 0;
        }
        legal = (nbits == sc.nbits);
    }
    return legal;
}

void SherBit::build_solution()
{
    u_t last = 0; // should hold upto including BMA_MAX-1 bits
    u_t last_full = 0; // should hold upto including BMA_MAX
    ull_t pending_legal = P;
    for (u_t bi = 0, bi1 = 1; bi1 < N; bi = bi1++)
    {
        if (dbg_flags & 0x8) { cerr << "bi="<<bi << ", bi1="<<bi1 <<
            ", last=" << last << ", pending=" << pending_legal << '\n'; }
        static const u_t low_mask = (1u << (BMA_MAX - 1)) - 1;
        const u_t last_low = last & low_mask;
        if (last != (last & last_low)) { cerr << "last=" << last << "=" << 
            u2str(last) << " bits above " << (BMA_MAX - 1) << '\n'; exit(1); }
        u_t bit = 0;
        if (!legal_segment(bi, last_full))
        {
            bit = 1;
        }
        else
        {
            ll_t sn_legals = xpfx_nlegals[bi1][last]; // like adding high 0-bit
            ull_t n_legals = (sn_legals >= 0 ? sn_legals : 0);
            if (n_legals < pending_legal)
            {
                bit = 1;
                pending_legal -= n_legals;
            }
        }
        solution.push_back("01"[bit]);
        last |= (bit << min<u_t>(bi, BMA_MAX - 1));
        last_full |= (bit << min<u_t>(bi, BMA_MAX));
        if (bi + 1 > BMA_MAX - 1)
        {
            last >>= 1;
        }
        if (bi + 1 > BMA_MAX)
        {
            last_full >>= 1;
        }
    }
    if (pending_legal > 1)
    {
        solution.push_back('1');
    }
    else
    {
        bool zero_legal = legal_segment(N - 1, last_full);
        solution.push_back(zero_legal ? '0' : '1');
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
