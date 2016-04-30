// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ul_t> vul_t;
typedef vector<ull_t> vull_t;
typedef pair<unsigned, unsigned> uu_t;
typedef map<uu_t, ull_t> uu2ull_t;

enum { BIGMOD =  1000000007 };

static unsigned dbg_flags;


class IncSpeed
{
 public:
    IncSpeed(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    uu2ull_t seg_ninc(unsigned b, unsigned e);
    unsigned n, m;
    ull_t X, Y, Z;
    vul_t A;
    vul_t speeds;
    ull_t solution;
    uu2ull_t memo;
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

void IncSpeed::solve()
{
    speeds.reserve(n);
    for (unsigned i = 0; i < n; ++i)
    {
        ull_t speed = A[i % m];
        speeds.push_back(speed);
        A[i % m] = (X * A[i % m] + Y * (i + 1)) % Z;
    }
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

    istream *pfi = (argc < 2 || (string(argv[1]) == dash))
         ? &cin
         : new ifstream(argv[1]);
    ostream *pfo = (argc < 3 || (string(argv[2]) == dash))
         ? &cout
         : new ofstream(argv[2]);

    if ((!pfi) || (!pfo) || pfi->fail() || pfo->fail())
    {
        cerr << "Open file error\n";
        exit(1);
    }

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    // bool large = (argc > 4) && !strcmp(argv[4], "-large");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        IncSpeed problem(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        problem.solve();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout << "\n";
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

