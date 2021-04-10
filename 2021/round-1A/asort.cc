// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Asort
{
 public:
    Asort(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N;
    vull_t x;
    u_t solution;
};

Asort::Asort(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(x));
}

void mult_p10(ull_t& v, int nz)
{
    while (nz > 0)
    {
        v *= 10;
        --nz;
    } 
}

void Asort::solve_naive()
{
    vull_t sx(x);
    for (u_t i = 1; i < N; ++i)
    {
        if (sx[i] <= sx[i - 1])
        {
            char s0[0x20], s1[0x20];
            sprintf(s0, "%lld", sx[i - 1]);
            sprintf(s1, "%lld", sx[i]);
            u_t len0 = strlen(s0);
            u_t len1 = strlen(s1);
            if (len0 == len1)
            {
                sx[i] *= 10;
                ++solution;
            }
            else // len1 < len0
            {
                u_t ti = 0;
                for (ti = 0; (ti < len1) && (s0[ti] == s1[ti]); ++ti) { }
                if (ti == len1) // prefix
                {
                    ull_t tail = 0;
                    // tail all 9 ?
                    u_t j = ti;
                    for (j = ti; (j < len0) && (s0[j] == '9'); ++j) {}
                    if (j == len0) // all 9s
                    {
                        int nz = (len0 - len1) + 1;
                        mult_p10(sx[i], nz);
                        solution += nz;
                    }
                    else
                    {
                        for (j = ti; (j < len0); ++j)
                        {
                            tail = 10*tail + (s0[j] - '0');
                        }
                        int len_tail = len0 - len1;
                        mult_p10(sx[i], len_tail);
                        sx[i] += tail + 1;
                        solution += len_tail;
                    }
                }
                else // not a prefix
                {
                    int nz = (len0 - len1) + (s1[ti] > s0[ti] ? 0 : 1);
                    mult_p10(sx[i], nz);
                    solution += nz;                   
                }
            }
        }
    }
    if (dbg_flags & 0x1) 
    {
        for (ull_t y: x) { cerr << ' ' << y; } cerr << '\n';
        for (ull_t y: sx) { cerr << ' ' << y; } cerr << '\n';
    }
}

void Asort::solve()
{
    solve_naive();
}

void Asort::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
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

    void (Asort::*psolve)() =
        (naive ? &Asort::solve_naive : &Asort::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Asort asort(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (asort.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        asort.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
