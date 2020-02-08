// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef pair<u_t, u_t> uu_t;
typedef vector<u_t> vu_t;
typedef vector<uu_t> vuu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

void sum_combs(vvu_t& sums, u_t n)
{
    sums.clear();
    for (u_t first = 1; first < n; ++first)
    {
        vvu_t tails;
        sum_combs(tails, n - first);
        for (const vu_t& tail: tails)
        {
            vu_t s;
            s.push_back(first);
            s.insert(s.end(), tail.begin(), tail.end());
            sums.push_back(s);
        }
    }
    vu_t s;
    s.push_back(n);
    sums.push_back(s);
}

class CompPFrac
{
 public:
    CompPFrac(u_t _N) : N(_N) {}
    bool operator()(u_t x0, u_t x1) const
    {
        u_t m0 = (100 * x0 + N/2) % N;
        u_t m1 = (100 * x1 + N/2) % N;
        return m0 < m1;
    }
 private:
   u_t N;
};

class Rounding
{
 public:
    Rounding(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t sum_percents(const vu_t& csa) const;
    bool comp_pfrac(u_t x0, u_t x1) const;
    u_t N, L;
    vu_t cs;
    u_t solution;
};

Rounding::Rounding(istream& fi) : solution(0)
{
    fi >> N >> L;
    cs.reserve(L);
    for (u_t li = 0; li != L; ++li)
    {
        u_t c;
        fi >> c;
        cs.push_back(c);
    }
}

void Rounding::solve_naive()
{
    vvu_t sums;
    u_t voted = accumulate(cs.begin(), cs.end(), 0);
    sum_combs(sums, N - voted);
    if (dbg_flags & 0x1) {
      cerr << "#sum_combs(" << N << ")=" << sums.size() << '\n';
      for (const vu_t& s: sums) {
         for (u_t x: s) {
           cerr << ' ' << x;
         }
         cerr << '\n';
      }
    }
    for (u_t shift = 0; shift <= L; ++shift)
    {
        for (const vu_t& add: sums) {
            vu_t cs_add(cs);
            for (u_t ai = 0, ci = shift, ae = add.size(); ai != ae;
                 ++ai, ++ci)
            {
                if (ci < L)
                {
                    cs_add[ci] += add[ai];
                }
                else
                {
                    cs_add.push_back(add[ai]);
                }
            }
            u_t value = sum_percents(cs_add);
            if (solution < value)
            {
                solution = value;
            }
        }
    }
}

void Rounding::solve()
{
   if (100 % N == 0)
   {
       solution = 100;
   }
   else
   {
       const u_t frac_unit = 100 % N;
       vu_t vcs(cs);
       vcs.push_back(0);
       CompPFrac cmp{N};
       make_heap(vcs.begin(), vcs.end(), cmp);
       u_t voted = accumulate(cs.begin(), cs.end(), 0);
       u_t pending = N - voted;
       while (pending > 0)
       {
           u_t c = vcs.front();
           if (c != 0)
           {
               pop_heap(vcs.begin(), vcs.end(), cmp);
               vcs.pop_back();
           }
           u_t cmod = (100*c) % N;
           u_t missing = (N + (N + 1)/2 - cmod) % N;
           u_t add = min((missing + frac_unit - 1) / frac_unit, pending);
           vcs.push_back(c + add);
           push_heap(vcs.begin(), vcs.end(), cmp);
           pending -= add;
       }
       solution = sum_percents(vcs);
   }
}

u_t Rounding::sum_percents(const vu_t& csa) const
{
    u_t sp = 0;
    for (u_t x: csa)
    {
        u_t np = (100 * x) / N;
        u_t mod = (100 * x) % N;
        if (2*mod >= N)
        {
            ++np;
        }
        sp += np;
    }
    return sp;
}

bool Rounding::comp_pfrac(u_t x0, u_t x1) const
{
    u_t m0 = (100 * x0 + N/2) % N;
    u_t m1 = (100 * x1 + N/2) % N;
    return m0 < m1;
}

void Rounding::print_solution(ostream &fo) const
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

    void (Rounding::*psolve)() =
        (naive ? &Rounding::solve_naive : &Rounding::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Rounding rounding(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rounding.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rounding.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
