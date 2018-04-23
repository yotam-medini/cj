// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <map>
#include <set>
#include <array>
#include <vector>
#include <utility>

// #include <boost/tuple/tuple.hpp>

#include <cstdlib>

using namespace std;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
// typedef vector<vul_t> vvul_t;
typedef map<ul_t, ul_t> ul2ul_t;
typedef set<ul_t> setul_t;
typedef multiset<ul_t> msetul_t;
typedef vector<setul_t> vsetul_t;
typedef vector<msetul_t> vmsetul_t;
// typedef vector<setul_t> vsetul_t;
// typedef vector<msetul_t> vmsetul_t;
typedef map<vul_t, ul_t> vu2u_t;

// typedef boost::tuple<ul_t, ul_t> tul2_t;
// typedef vector<tul2_t> vtul2_t;
typedef array<ul_t, 2> a2_t;
typedef vector<a2_t> va2_t;

class Ingred
{
 public:
    Ingred(ul_t vg=0, const vul_t vp=vul_t()): gram(vg), p(vp) {}
    ul_t gram;
    vul_t p;
    va2_t kminmax;
    vu2u_t memo;
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
    bool match(const a2_t &mm0, const a2_t &mm1) const
    {
        // mm0.get<1> givesL
        //  error: cannot resolve overloaded function ‘get’ based
        //  on conversion to type ‘ul_t {aka long unsigned int}’
        bool ret = (mm0[1] >= mm1[0]) && (mm1[1] >= mm0[0]);
        return ret;
    }
    void sort_packages();
    void kcompact();
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

void Rata::sort_packages()
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
                ing.kminmax.push_back(a2_t({kmin, kmax}));
            }
        }
    }
}

void Rata::solve_naive()
{
    sort_packages();
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
                ing.kminmax.push_back(a2_t({kmin, kmax}));
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

void Rata::kcompact()
{
    setul_t kset;
    for (unsigned i = 0; i < n; ++i)
    {
        Ingred &ing = ingreds[i];
        for (unsigned pi = 0; pi < p; ++pi)
        {
            const a2_t &mm = ing.kminmax[pi];
            kset.insert(mm[0]);
            kset.insert(mm[1]);
        }
    };
    ul2ul_t k2k;
    unsigned knew = 0;
    for (setul_t::const_iterator ksi = kset.begin(), kse = kset.end(); 
        ksi != kse; ++ksi, ++knew)
    {
        ul_t k = *ksi;
        k2k.insert(k2k.end(), ul2ul_t::value_type(k, knew));
    }
    const unsigned ksup = knew;
    // vul_t cover_count(vul_t::size_type(knew), 0);
    vsetul_t cover = vsetul_t(vsetul_t::size_type(ksup), setul_t());
    vmsetul_t cover_count = vmsetul_t(vmsetul_t::size_type(ksup), msetul_t());
    for (unsigned i = 0; i < n; ++i)
    {
        Ingred &ing = ingreds[i];
        for (unsigned pi = 0; pi < p; ++pi)
        {
            a2_t &mm = ing.kminmax[pi];
            
            for (unsigned ti = 0; ti < 2; ++ti)
            {
                ul_t kold = mm[ti];
                knew = k2k[kold];
                mm[ti] = knew;
            }
            for (unsigned k = mm[0]; k <= mm[1]; ++k)
            {
                cover[k].insert(cover[k].end(), i);
                cover_count[k].insert(cover_count[k].end(), i);
            }
        }
    };

    bool progress = true;
    unsigned k =0;
    while (progress)
    {
        progress = false;
        for (; (k <= ksup) && !progress; ++k)
        {
             progress = cover[k].size() == n;
             if (progress)
             {
                 unsigned nk = p; // infinity
                 for (unsigned i = 0; i < n; ++i)
                 {
                     unsigned icount = cover_count[k].count(k);
                     if (nk > icount)
                     {
                         nk = icount;
                     }
                 }
                 solution += nk;
                 cover[k] = setul_t();
             }
        }
    }    
}


void Rata::solve()
{
    sort_packages();
    kcompact();
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
