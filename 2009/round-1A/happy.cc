// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

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
typedef vector<ul_t> vul_t;
typedef set<ul_t> set_ul_t;

static unsigned dbg_flags;

static set_ul_t happy_bases[11];

static bool is_happy(ul_t n, unsigned base)
{
    bool min_seen = false;
    ul_t vmin = n;
    while (!min_seen)
    {
        ul_t next_val = 0;
        while (n)
        {
            ul_t d = n % base;
            n /= base;
            next_val += d*d;
        }
        min_seen = (next_val == vmin);
        if (vmin > next_val)
        {
            vmin = next_val;
        }
        n = next_val;
    }
    bool ret = (n == 1);
    return ret;
}


static void happy_bases_compute()
{
    bool all_happy = false;
    ul_t n;
    for (n = 2; !all_happy; ++n)
    {
        all_happy = true;
        for (unsigned base = 2; base <= 10; ++base)
        {
            bool bhappy = is_happy(n, base);
            if (bhappy)
            {
                 happy_bases[base].insert(n);
            }
            else
            {
                all_happy = false;
            }
        }
    }
    cerr << "All bases are happy with " << (n-1) << "\n";
}

class Happy
{
 public:
    Happy(istream& fi);
    void solve();
    void print_solution(ostream&) const;
 private:
    vul_t bases;
};

Happy::Happy(istream& fi)
{
    string line;
    getline(fi, line);
    const char *cline = line.c_str();
    bool more = true;
    while (more)
    {
        char *end;
        unsigned base = strtoul(cline, &end, 10);
        if ((more = (end != cline)))
        {
            bases.push_back(base);
            cline = end;
        }
    }
}

void Happy::solve()
{
}

void Happy::print_solution(ostream &fo) const
{
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

    happy_bases_compute();

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Happy problem(*pfi);
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
