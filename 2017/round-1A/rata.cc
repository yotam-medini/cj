// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <utility>

#include <boost/tuple/tuple.hpp>
// #include <boost/tuple/tuple_comparison.hpp>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef vector<vul_t> vvul_t;
typedef set<ul_t> setul_t;
typedef multiset<ul_t> msetul_t;
typedef vector<setul_t> vsetul_t;
typedef vector<msetul_t> vmsetul_t;

typedef boost::tuple<ul_t, ul_t> tul2_t;
typedef vector<tul2_t> vtul2_t;

class Ingred
{
 public:
    Ingred(ul_t vg=0, const vul_t vp=vul_t()): gram(vg), p(vp) {}
    ul_t gram;
    vul_t p;
    vtul2_t kminmax;
    // setul_t q;
    // msetul_t mq;
};
typedef vector<Ingred> vingred_t;

static unsigned dbg_flags;

class Rata
{
 public:
    Rata(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ul_t naive_sub_solve(unsigned b0, unsigned b1);
    bool match(const tul2_t &mm0, const tul2_t &mm1) const
    {
        // mm0.get<1> givesL
        //  error: cannot resolve overloaded function ‘get’ based
        //  on conversion to type ‘ul_t {aka long unsigned int}’
        bool ret = 
            (get<1>(mm0) >= get<0>(mm1)) &&
            (get<1>(mm1) >= get<0>(mm0));
        return ret;
    }
    unsigned n, p;
    vingred_t ingreds;
    ul_t solution;
};

Rata::Rata(istream& fi) : solution(0)
{
    fi >> n >> p;
    const Ingred ingred0(0, vul_t(vul_t::size_type(p), 0));
    ingreds = vingred_t(vingred_t::size_type(n), ingred0);
    for (unsigned i = 0; i < n; ++i)
    {
        fi >> ingreds[i].gram;
    }
    for (unsigned i = 0; i < n; ++i)
    {
        Ingred &ing = ingreds[i];
        for (unsigned pi = 0; pi < p; ++pi)
        {
            fi >> ing.p[pi];
        }
    }
}

ul_t Rata::naive_sub_solve(unsigned b0, unsigned b1)
{
    const Ingred &ing0 = ingreds[0];
    const Ingred &ing1 = ingreds[1];
    unsigned add0 = match(ing0.kminmax[b0], ing1.kminmax[b1]) ? 1 : 0;
    unsigned n0 = ing0.kminmax.size();
    unsigned n1 = ing1.kminmax.size();
    unsigned best = add0;
    if ((b0 < n0 - 1) || (b1 < n1 - 1))
    {
        for (unsigned i0 = b0 + 1; i0 < n0; ++i0)
        {
            unsigned sub = naive_sub_solve(i0, b1);
            if (best < sub)
            {
                best = sub;
            }
        }        
        for (unsigned i1 = b1 + 1; i1 < n1; ++i1)
        {
            unsigned sub = naive_sub_solve(b0, 11);
            if (best < sub)
            {
                best = sub;
            }
        }
        if ((add0 > 0) && (b0 + 1 < n0) && (b1 + 1 < n1))
        {
            unsigned sub = naive_sub_solve(b0 + 1, b1 + 1);
            if (best < sub + add0)
            {
                best = sub + add0;
            }
        }
    }
    return best;
}

void Rata::solve_naive()
{
    for (unsigned i = 0; i < n; ++i)
    {
        Ingred &ing = ingreds[i];
        ul_t g = ing.gram;
        sort(ing.p.begin(), ing.p.end());
        ing.kminmax.reserve(p);
        for (unsigned pi = 0; pi < p; ++pi)
        {
            ul_t pg = ing.p[pi];
            // 0.9g <= pg / q <= 1.1g
            // (pg / g)/1.1  <=  q  <=  (pg / q)/0.9 
            // 10 * pg  <=  11*q*g   &    9*q*g <=  10*pg
            ul_t kmin = (10*pg + 11*g - 1) / (11 * g);
            ul_t kmax = (10*pg) / (9 * g);
            if (kmin <= kmax)
            {
                ing.kminmax.push_back(tul2_t(kmin, kmax));
            }
        }
    }
    if (n == 1)
    {
        solution = ingreds[0].kminmax.size();
    }
    else if (n == 2)
    {
        solution = naive_sub_solve(0, 0);
    }
    else
    {
        cerr << "naive cannot with n="<< n << "\n";
    }
}


void Rata::solve()
{
    solve_naive();
}

void Rata::print_solution(ostream &fo) const
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

    void (Rata::*psolve)() =
        (naive ? &Rata::solve_naive : &Rata::solve);

    bool tellg = getenv("TELLG");
    ul_t fpos_last = pfi->tellg();
    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Rata problem(*pfi);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << ", " << fpos << ")\n";
            fpos_last = fpos;
        }
        
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
