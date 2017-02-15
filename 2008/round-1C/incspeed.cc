// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
// #include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;

enum { BIGMOD =  1000000007 };

static unsigned dbg_flags;

class Boundary
{
 public:
    Boundary(ul_t vl=0, ul_t vr=0): l(vl), r(vr) {}
    ul_t l, r;
};
static bool lt_l(const Boundary &b0, const Boundary& b1) { return b0.l < b1.l; }
static bool lt_r(const Boundary &b0, const Boundary& b1) { return b0.r < b1.r; }
typedef vector<Boundary> vbdy_t;

class IncSpeed
{
 public:
    IncSpeed(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    unsigned n, m;
    ull_t X, Y, Z;
    vul_t A;
    vul_t speeds;
    ull_t solution;
    void set_speeds();
    void get_sibseq_bdys(vbdy_t &ssbdy, unsigned b, unsigned e) const;
};

IncSpeed::IncSpeed(istream& fi) : solution(0)
{
    fi >> n >> m >> X >> Y >> Z;
    A.reserve(m);
    for (unsigned i = 0; i < m; ++i)
    {
        ull_t a;
        fi >> a;
        A.push_back(a);
    }
}

void IncSpeed::set_speeds()
{
    speeds.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        ull_t speed = A[i % m];
        speeds.push_back(speed);
        A[i % m] = (X * A[i % m] + Y * (i + 1)) % Z;
    }
}

void IncSpeed::solve_naive()
{
    set_speeds();
    vull_t inc_from(vull_t::size_type(n), 1); // as singles
    for (int f = n - 1; f >= 0; --f)
    {
        for (unsigned s = f + 1; s < n; ++s)
        {
            if (speeds[f] < speeds[s])
            {
                inc_from[f] = (inc_from[f] + inc_from[s]) % BIGMOD;
            }
        }
        solution = (solution + inc_from[f]) % BIGMOD;
    }
}

void IncSpeed::solve()
{
    set_speeds();
    vbdy_t ssbdy;
    get_sibseq_bdys(ssbdy, 0, speeds.size());
    solution = ssbdy.size();
}

void IncSpeed::get_sibseq_bdys(vbdy_t& ssbdy, unsigned b, unsigned e) const
{
    if (b + 1 == e)
    {
        ssbdy.push_back(Boundary(speeds[b], speeds[b]));
    }
    else
    {
        vbdy_t ssl, ssr;
        unsigned m = (b + e)/2;

        get_sibseq_bdys(ssl, b, m);
        get_sibseq_bdys(ssr, m, e);
        vbdy_t sslr ;
        merge(ssl.begin(), ssl.end(), ssr.begin(), ssr.end(),
            back_inserter(sslr), lt_l);

        vbdy_t comb;
        sort(ssl.begin(), ssl.end(), lt_r);
        vbdy_t::const_iterator ri = ssr.begin(), re = ssr.end(), rit;
        for (vbdy_t::const_iterator li = ssl.begin(), le = ssl.end();
             li != le; ++li)
        {
            const Boundary& bdyl = *li;
            for (; (ri != re) && ri->l <= bdyl.r; ++ri);
            for (rit = ri; rit != re; ++rit)
            {
                const Boundary& bdyr = *rit;
                comb.push_back(Boundary(bdyl.l, bdyr.r));
            }
        }
        sort(comb.begin(), comb.end(), lt_l);
        merge(sslr.begin(), sslr.end(), comb.begin(), comb.end(),
              back_inserter(ssbdy), lt_l);
    }
    if (dbg_flags & 0x1) {
        cerr << "b="<<b << ", e="<<e << ", #="<<ssbdy.size() << "\n";
    }
}

#if 0
uu2ull_t IncSpeed::seg_ninc(unsigned b, unsigned e)
{
    uu2ull_t ret = 1;
    if (b + 1 < e)
    {
        uu2ull_t.key_type key(a, b);
        auto er = memo.equal_range(k);
        if (er.first != er.second)
        {
            ret = *(er.first);
        }
        else
        {
        }
    }
    return ret;
}
#endif

void IncSpeed::print_solution(ostream &fo) const
{
    fo << " " << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    unsigned n_opts = 0;
    bool naive = false;

    if ((argc > 1) && (string(argv[1]) == "-naive"))
    {
        naive = true;
        n_opts = 1;
    }
    int ai_in = n_opts + 1;
    int ai_out = ai_in + 1;
    int ai_dbg = ai_out + 1;
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

    if (ai_dbg < argc) { dbg_flags = strtoul(argv[ai_dbg], 0, 0); }

    unsigned n_cases;
    *pfi >> n_cases;

    void (IncSpeed::*psolve)() =
        (naive ? &IncSpeed::solve_naive : &IncSpeed::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        IncSpeed problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

