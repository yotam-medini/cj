// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2013/April/20

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <utility>

#include <cstdlib>
#include <cstring>

#include <gmpxx.h>

using namespace std;

static unsigned dbg_flags;

typedef unsigned long ul_t;
typedef vector<ul_t> vul_t;
typedef map<unsigned, ul_t> u2ul_t;
typedef vector<bool> vb_t;

class Segment
{
 public:
    Segment(unsigned vend=0, unsigned vsize=0) : end(vend), size(vsize) {}
    unsigned end;
    ul_t size;
};

class State
{
 public:
     State(bool vm=false, ul_t vr=0, mpz_class vsum=0) :
         marked(vm), r(vr), sum(vsum) {}
     bool marked;
     ul_t r;
     mpz_class sum;
};

typedef vector<State> vstates_t;

typedef vector<Segment> vseg_t;

class Theme
{
 public:
    Theme(istream& fi);
    void solve();
    void solve_naive();
    void print_solution(ostream&);
 private:
    void limited_rides(ul_t &r, unsigned &gi, bool stop_at_cycle);
    ul_t R;
    ul_t k;
    unsigned n;
    vul_t g;
    mpz_class solution;
    vstates_t states;
};

Theme::Theme(istream& fi) : solution(0)
{
    fi >> R >> k >> n;
    g = vul_t(vul_t::size_type(n), vul_t::value_type(0));
    for (unsigned i = 0; i < n; ++i)
    {
        fi >> g[i];
    }
}

void Theme::solve()
{
    states = vstates_t(vstates_t::size_type(n), State());
    ul_t r = 0;
    unsigned gi = 0;
    limited_rides(r, gi, true);
    if (r < R)
    {
        ul_t r_cycle = r - states[gi].r;
        mpz_class s_cycle = solution - states[gi].sum;
        ul_t n_more_sycles = (R - r) / r_cycle;
        solution += n_more_sycles * s_cycle;
        r += n_more_sycles * r_cycle;

        limited_rides(r, gi, false);
    }

}

void Theme::limited_rides(ul_t &rr, unsigned &rgi, bool stop_at_cycle)
{
    ul_t r = rr;
    unsigned gi = rgi;
    for (; (r < R) && (!stop_at_cycle || !states[gi].marked); ++r)
    {
        states[gi] = State(true, r, solution);
        ul_t ride_size = 0;
        for (unsigned i = 0; i < n; ++i)
        {
            ul_t ride_size_next = ride_size + g[(gi + i) % n];
            if (ride_size_next <= k)
            {
                ride_size = ride_size_next;
            }
            else
            {
                 gi = (gi + i) % n;
                 i = n; // exit loop
            }
        }
        solution += ride_size;
    }
    rr = r; rgi = gi;
}

void Theme::solve_naive()
{
    unsigned gi = 0;
    for (unsigned r = 0; r < R; ++r)
    {
        ul_t ride_size = 0;
        for (unsigned i = 0; i < n; ++i)
        {
            ul_t ride_size_next = ride_size + g[(gi + i) % n];
            if (ride_size_next <= k)
            {
                ride_size = ride_size_next;
            }
            else
            {
                 gi = (gi + i) % n;
                 i = n; // exit loop
            }
        }
        solution += ride_size;
    }
}

void Theme::print_solution(ostream &fo)
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

    if (argc > 3) { dbg_flags = strtoul(argv[3], 0, 0); }
    bool naive = (argc > 4) && !strcmp(argv[4], "-naive");

    unsigned n_cases;
    *pfi >> n_cases;

    ostream &fout = *pfo;
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Theme  theme(*pfi);
        // cerr << "Case ci="<<ci << " (ci+1)="<<ci+1 << "\n";
        if (naive)
        {
            theme.solve_naive();
        }
        else
        {
            theme.solve();
        }
        fout << "Case #"<< ci+1 << ":";
        theme.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

