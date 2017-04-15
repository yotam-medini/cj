// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
#include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
// typedef unsigned long ul_t;
// typedef unsigned long long ull_t;
// typedef vector<char> vc_t;
// typedef set<char> set_c_t;

static unsigned dbg_flags;

enum { MaxGrid = 25 };

class Kid
{
 public:
    Kid(unsigned vi=0, unsigned vj=0): 
        i(vi), j(vj), 
        rmin(vi), rmax(vi), 
        cmin(vj), cmax(vj)
    {}
    unsigned i, j; 
    unsigned rmin, rmax, cmin, cmax;
};
typedef map<char, Kid> c2kid_t;

class ABCake
{
 public:
    ABCake(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void gready();
    bool is_empty(unsigned rb, unsigned re, unsigned cb, unsigned ce) const;
    void print_grid(ostream &fo) const;
    unsigned r, c;
    char grid[MaxGrid][MaxGrid];
    c2kid_t kids;
};

ABCake::ABCake(istream& fi)
{
    for (unsigned ri = 0; ri < MaxGrid; ++ri)
    {
        for (unsigned ci = 0; ci < MaxGrid; ++ci)
        {
            grid[ri][ci] = '\0';
        }
    }
    fi >> r >> c;
    for (unsigned ri = 0; ri < r; ++ri)
    {
        string s;
        fi >> s;
        for (unsigned ci = 0; ci < c; ++ci)
        {
            grid[ri][ci] = s[ci];
        }
    }
}

void ABCake:: print_grid(ostream& fo) const
{
    for (unsigned ri = 0; ri < r; ++ri)
    {
        for (unsigned ci = 0; ci < c; ++ci)
        {
            fo << grid[ri][ci];
        }
        fo << "\n";
    }
}

void ABCake::solve_naive()
{
    for (unsigned ri = 0; ri < r; ++ri)
    {
        for (unsigned ci = 0; ci < c; ++ci)
        {
            char ckid = grid[ri][ci];
            if (ckid != '?')
            {
                c2kid_t::value_type v(ckid, Kid(ri, ci));
                kids.insert(kids.end(), v);
            }
        }
    }
    gready();
}

void ABCake::gready()
{
    for (c2kid_t::iterator i = kids.begin(), e = kids.end(); i != e; ++i)
    {
         char ckid = (*i).first;
         Kid& kid = (*i).second;
         for (unsigned ri = kid.i; 
              ri > 0 && is_empty(ri - 1, ri, kid.j, kid.j + 1);
              --ri)
         {
             kid.rmin = ri - 1;
             grid[ri - 1][kid.j] = ckid;
         }
         for (unsigned ri = kid.i + 1; 
              ri < r && is_empty(ri, ri + 1, kid.j, kid.j + 1);
              ++ri)
         {
             kid.rmax = ri;
             grid[ri][kid.j] = ckid;
         }
         for (unsigned ci = kid.j; 
              ci > 0 && is_empty(kid.rmin, kid.rmax + 1, ci - 1, ci);
              --ci)
         {
             for (unsigned ri = kid.rmin; ri <= kid.rmax; ++ri)
             {
                 grid[ri][ci - 1] = ckid;
             }
         }
         for (unsigned ci = kid.j + 1; 
              ci < c && is_empty(kid.rmin, kid.rmax + 1, ci, ci + 1);
              ++ci)
         {
             for (unsigned ri = kid.rmin; ri <= kid.rmax; ++ri)
             {
                 grid[ri][ci] = ckid;
             }
         }
         if (dbg_flags & 0x2)
         {
             cerr << "kid: " << ckid << " done:\n";
             print_grid(cerr);
         }
    }
}

bool ABCake::is_empty(unsigned rb, unsigned re, unsigned cb, unsigned ce) const
{
    bool ret = true;
    for (unsigned ri = rb; ret && (ri < re); ++ri)
    {
        for (unsigned ci = cb; ret && (ci < ce); ++ci)
        {
            ret = grid[ri][ci] == '?';
        }
    }
    return ret;
}

void ABCake::solve()
{
    if (dbg_flags & 0x1) { print_grid(cerr); }
    solve_naive();
}

void ABCake::print_solution(ostream &fo) const
{
    fo << "\n";
    print_grid(fo);
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

    void (ABCake::*psolve)() =
        (naive ? &ABCake::solve_naive : &ABCake::solve);

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        if (getenv("TELLG"))
        {
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg="<<pfi->tellg() << "\n";
        }
        ABCake problem(*pfi);
        (problem.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        problem.print_solution(fout);
        fout.flush();

    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
