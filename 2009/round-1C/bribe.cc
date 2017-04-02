// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
// typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef set<ul_t> setu_t;
typedef vector<ul_t> vu_t;

static unsigned dbg_flags;

class Bribe
{
 public:
    Bribe(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ul_t coins(unsigned cellb, unsigned celle);
    ul_t p;
    vu_t q;
    setu_t q_in;
    setu_t q_out;
    ul_t solution;
};

Bribe::Bribe(istream& fi) : solution(0)
{
    fi >> p;
    unsigned nq;
    fi >> nq;
    q = vu_t(vu_t::size_type(nq), 0);
    for (unsigned i = 0; i < nq; ++i)
    {
        unsigned v;
        fi >> v;
        q[i] = v - 1; // Dijkstra style
    }
    sort(q.begin(), q.end());
}

void Bribe::solve_naive()
{
}

void Bribe::solve()
{
    setu_t released;
    for (unsigned qi : q)
    {
        q_in.insert(q_in.end(), qi);
    }
    solution = coins(0, p);
}

ul_t Bribe::coins(unsigned cellb, unsigned celle)
{
    static ul_t ul_infty = ul_t(-1); // infinity;
    static unsigned u_infty = unsigned(-1); // infinity;
    ul_t ret = ul_infty;
    setu_t::iterator qb = q_in.upper_bound(cellb);
    setu_t::iterator qe = q_in.upper_bound(celle);
    for (setu_t::iterator qi = qb, qi_next = qi; qi != qe; qi = qi_next)
    {
        ++qi_next;
        unsigned qn = *qi;
        auto eqr = q_out.equal_range(qn);
        setu_t::iterator pred = eqr.first;
        setu_t::iterator succ = eqr.second;
        if (pred != q_out.begin()) { --pred; }
        if (succ != q_out.end()) { ++succ; }
        unsigned pred_n = (pred != q_out.end() ? *pred : u_infty);
        unsigned succ_n = (succ != q_out.begin() ? *succ : u_infty);
        ul_t coins_low = (pred_n == u_infty ? qn : qn - pred_n - 1);
        ul_t coins_high = (succ_n == u_infty ? p - qn - 1 : succ_n - qn);
        ul_t ret_candid = coins_low + coins_high;

        q_in.erase(qi);
        setu_t::iterator qo = q_out.insert(q_out.end(), qn);
        if (pred_n != u_infty)
        {
            ret_candid += coins(cellb, qn);
        }
        if (succ_n != u_infty)
        {
            ret_candid += coins(qn + 1, celle);
        }
        q_out.erase(qo);
        q_in.insert(q_in.end(), qn);

        if (ret > ret_candid)
        {
            ret = ret_candid;
        }
    }
    return ret;
}

void Bribe::print_solution(ostream &fo) const
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

    void (Bribe::*psolve)() =
        (naive ? &Bribe::solve_naive : &Bribe::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        Bribe problem(*pfi);
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
