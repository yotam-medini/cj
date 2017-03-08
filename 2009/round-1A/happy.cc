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

static ul_t happy_iterate(ul_t n, unsigned base)
{
    ul_t next_val = 0;
    while (n)
    {
        ul_t d = n % base;
        n /= base;
        next_val += d*d;
    }
    return next_val;
}

static bool is_happy(ul_t n, unsigned base)
{
    ul_t slow = n;
    bool ret = false;
    do
    {
        n = happy_iterate(n, base);
        ret = (n == 1);
        n = happy_iterate(n, base);
        ret = ret || (n == 1);
        slow = happy_iterate(slow, base);
    } while ((slow != n) && !ret);
    return ret;
}


static void happy_bases_compute()
{
    bool all_happy = false;
    ul_t n;
    unsigned n_max_happy_bases = 0,  n_happy_bases = 0;
    for (n = 2; !all_happy; ++n)
    {
        if ((n & (n-1)) == 0) { cerr << "n="<<n<<"\n"; }
        all_happy = true;
        n_happy_bases = 0;
        int curr_happy_bases[9];
        for (unsigned base = 2; base <= 10; ++base)
        {
            bool bhappy = is_happy(n, base);
            if (bhappy)
            {
                 // happy_bases[base].insert(n);
                 curr_happy_bases[n_happy_bases] = base;
                 ++n_happy_bases;
            }
            else
            {
                all_happy = false;
            }
        }
        if (n_happy_bases > 1 || happy_bases[curr_happy_bases[0]].empty())
        {
            for (unsigned b = 0; b < n_happy_bases; ++b)
            {
                happy_bases[curr_happy_bases[b]].insert(n);
            }
        }
        if (n_max_happy_bases < n_happy_bases)
        {
            n_max_happy_bases = n_happy_bases;
        }
        if (n_happy_bases >= 8)
        {
            cerr << "n="<<n << ", #(happy bases)="<<n_happy_bases << " :";
            for (unsigned b = 0; b < n_happy_bases; ++b)
            {
                cerr << " " << curr_happy_bases[b];
            }
            cerr << "\n";
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
