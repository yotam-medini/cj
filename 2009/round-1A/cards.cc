// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <gmpxx.h>

using namespace std;

typedef mpz_class mpzc_t;
typedef mpq_class mpqc_t;
typedef pair<unsigned, unsigned> uu_t;
typedef map<uu_t, mpzc_t> uu2mpz_t;
typedef map<uu_t, mpqc_t> uu2mpq_t;
typedef uu2mpz_t::value_type uu2mpzv_t;
typedef vector<mpqc_t> vmpq_t;

typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

enum { MaxCards = 40 };

uu2mpz_t choose_data;

static const mpzc_t& choose_mn(const uu2mpz_t& cd, unsigned m, unsigned n)
{
    n = min(n, m - n);
    auto w = cd.find(uu_t(m, n));
    if (w == cd.end())
    {
        cerr << __func__ << " software error\n";
        exit(1);
    }
    const mpzc_t& ret = w->second;
    return ret;
}

static void compute_choose_data(uu2mpz_t& cd)
{
    cd.insert(cd.end(), uu2mpzv_t(uu_t(1, 0), 1));
    for (unsigned m = 2; m <= MaxCards; ++m)
    {
        unsigned mm1 = m - 1;
        unsigned half = m/2;
        for (unsigned n = 0; n <= half; ++n)
        {
            mpzc_t v = (choose_mn(cd, mm1, n) * m) / (m - n);
            cd.insert(cd.end(), uu2mpzv_t(uu_t(m, n), v));
        }
    }
}

class Cards
{
 public:
    Cards(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void compute_move_prob();
    unsigned c, n;
    uu2mpq_t move_prob;
    mpqc_t solution;
};

Cards::Cards(istream& fi) : c(0), n(0)
{
    fi >> c >> n;
}

void Cards::solve_naive()
{
}

void Cards::solve()
{
    const mpqc_t one(1);
    compute_move_prob();
    vmpq_t expects(c + 1, mpqc_t());
    expects[c] = 0;
    for (int s = c; s >= 0; --s)
    {
        mpqc_t sexp(0);
        for (unsigned t = s + 1, te = min(s+n, c); t <= te; ++t)
        {
            auto w = move_prob.find(uu_t(s, t));
            if (w == move_prob.end())
            {
                cerr << "software error " << __func__ << "\n";
            }
            const mpqc_t& pst = (*w).second;
            sexp += pst * (one + expects[t]);
        }
        auto w = move_prob.find(uu_t(s, s));
        if (w == move_prob.end())
        {
            cerr << "software error " << __func__ << "\n";
        }
        const mpqc_t& pss = (*w).second;
        expects[s] = sexp / (one - pss);
    }
    solution = expects[0];
}

void Cards::compute_move_prob()
{
    mpzc_t all = choose_mn(choose_data, c, n);
cerr << "all="<<all.get_ui() << "\n";
    for (unsigned s = 0; s <= c; ++s)
    {
        const unsigned te = min(s + n, c);
        for (unsigned t = s; t <= te; ++t)
        {
            unsigned add = t - s;
            mpzc_t cadd = choose_mn(choose_data, c, add);
            mpzc_t cstay = choose_mn(choose_data, c - add, s);
            mpzc_t numerator = cadd * cstay;
            mpqc_t p(numerator, all);
cerr << "s="<<s << ", t="<<t<< 
 ", add="<<add <<", cadd="<<cadd.get_ui() << ", cstay="<<cstay.get_ui() <<
 ", num="<<numerator.get_ui() << "\n";
cerr << "p("<<s<<"->"<<t<<")=" << p.get_d() << "\n";
            uu2mpq_t::value_type v(uu_t(s, t), p);
            move_prob.insert(move_prob.end(), v);
        }
    }
}

void Cards::print_solution(ostream &fo) const
{
    double d = solution.get_d();
    fo << " " << fixed << setprecision(7) << d;
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

    void (Cards::*psolve)() =
        (naive ? &Cards::solve_naive : &Cards::solve);

    ostream &fout = *pfo;
    compute_choose_data(choose_data);
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        Cards problem(*pfi);
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
