// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;
typedef vector<bool> vbool_t;

static unsigned dbg_flags;

class BathStalls
{
 public:
    BathStalls(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void select_stall();
    ull_t n, k;
    vbool_t occupied;
    ull_t space_max, space_min;
};

BathStalls::BathStalls(istream& fi) : space_max(0), space_min(0)
{
    fi >> n >> k;
}

void BathStalls::select_stall()
{
    ull_t select_free_b = n;
    ull_t select_free_e = 0;
    ull_t free_max = 0;
    for (ull_t si = 0;  si < n + 1; )
    {
        for (; (si < n) && occupied[si]; ++si) {}
        if (si < n + 1) // free found
        {
            ull_t free_b = si;
            for (; !occupied[si]; ++si) {}
            ull_t free_e = si;
            ull_t free_size = free_e - free_b;
            if (free_max < free_size)
            {
                free_max = free_size;
                select_free_b = free_b;
                select_free_e = free_e;
            }
        }
    }
    // [10,11]->10,  [10,12]->10,  [10,13]->11  [10,14]->11
    ull_t iselect = (select_free_b + select_free_e - 1) / 2;
    space_min = iselect - select_free_b;
    space_max = (select_free_e - 1) - iselect;
    occupied[iselect] = true;
}

void BathStalls::solve_naive()
{
    occupied = vbool_t(vbool_t::size_type(n + 2), false);
    occupied[0] = true;
    occupied[n + 1] = true;
    // simulate
    for (ull_t p = 0; p < k; ++p)
    {
        select_stall();
    }
}

void BathStalls::solve()
{
    ull_t p2 = 1, p2_next = 2;
    while (p2_next <= k)
    {
        p2 = p2_next;
        p2_next *= 2; 
    }
    ull_t p2m1 = p2 - 1;
    ull_t n_level_segs = p2m1 + 1;
    ull_t level_sz = (n - p2m1);
    if (n_level_segs > level_sz)
    {
        n_level_segs = level_sz;
    }
    ull_t level_seg_sz = level_sz/n_level_segs;
    ull_t level_seg_residue = level_sz % n_level_segs;
    ull_t in_level = k - p2;
    ull_t seg_sz = level_seg_sz;
    if (in_level < level_seg_residue)
    {
        seg_sz += 1;
    }
    space_min = (seg_sz - 1)/2;
    space_max = (seg_sz - 1) - space_min;
}

void BathStalls::print_solution(ostream &fo) const
{
    fo << " " << space_max << ' ' << space_min;
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

    void (BathStalls::*psolve)() =
        (naive ? &BathStalls::solve_naive : &BathStalls::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BathStalls bathstalls(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bathstalls.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        bathstalls.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
