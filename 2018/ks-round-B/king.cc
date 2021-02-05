// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
// #include <map>
// #include <set>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class BIT
{
 public:
    BIT(u_t _max_idx) : max_idx(_max_idx), tree(_max_idx + 1, 0) {}
    void update(u_t idx, u_t delta)
    {
        if (idx > 0)
        {
            while (idx <= max_idx)
            {
                tree[idx] += delta;
                idx += (idx & -idx);
            }
        }
    }
    u_t query(int idx) const
    {
        u_t n = 0;
        while (idx > 0)
        {
            n += tree[idx];
            idx -= (idx & -idx); // removing low bit
        }
        return n;
    }
 private:
    u_t max_idx;
    vu_t tree;
};

class King
{
 public:
    King(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void gen_vh();
    ull_t n_monotone(bool increase) const;
    u_t N;
    ull_t V1, H1, A, B, C, D, E, F, M;
    vu_t v, h;
    vau2_t vh;
    u_t vmax, hmax;
    ull_t solution;
};

King::King(istream& fi) : solution(0)
{
    fi >> N >> V1 >> H1 >> A >> B >> C >> D >> E >> F >> M;
}

void King::solve_naive()
{
    gen_vh();
    for (u_t i = 0; i < N; ++i)
    {
        const au2_t& vh0 = vh[i];
        for (u_t j = i + 1; j < N; ++j)
        {
            const au2_t& vh1 = vh[j];
            for (u_t k = j + 1; k < N; ++k)
            {
                const au2_t& vh2 = vh[k];
                bool can = true;
                if ((vh0[0] < vh1[0]) && (vh1[0] < vh2[0]))
                {
                    bool hinc = ((vh0[1] < vh1[1]) && (vh1[1] < vh2[1]));
                    bool hdec = ((vh0[1] > vh1[1]) && (vh1[1] > vh2[1]));
                    if (hinc || hdec)
                    {
                        can = false;
                    }
                }
                if (can)
                {
                    ++solution;
                }
            }
        }
    }
}

void King::solve()
{
    gen_vh();
    const ull_t Nll = N;
    const ull_t choose3 = (Nll*(Nll - 1)*(Nll - 2)) / (1*2*3);
    const ull_t n_mono_inc = n_monotone(true);
    const ull_t n_mono_dec = n_monotone(false);
    const ull_t n_mono = n_mono_inc + n_mono_dec;
    solution = choose3 - n_mono;
}

void King::gen_vh()
{
    v.reserve(N);
    h.reserve(N);
    v.push_back(V1);
    h.push_back(H1);
    for (u_t i = 1; i < N; ++i)
    {
        // Vi = (A × Vi-1 + B × Hi-1 + C) modulo M
        // Hi = (D × Vi-1 + E × Hi-1 + F) modulo M
        u_t vi = (A * v[i - 1] + B * h[i - 1] + C) % M;
        u_t hi = (D * v[i - 1] + E * h[i - 1] + F) % M;
        v.push_back(vi);
        h.push_back(hi);
    }
    vh.reserve(N);
    hmax = 0;
    for (u_t i = 0; i < N; ++i)
    {
        vh.push_back(au2_t{v[i], h[i]});
        if (hmax < h[i]) { hmax = h[i]; }
    }
    sort(vh.begin(), vh.end());
    vmax = vh[N - 1][0];
}

ull_t King::n_monotone(bool increase) const
{
   ull_t n_mono = 0;
   BIT bit1(hmax + 1), bit2(hmax + 1);
   int ib = 0, ie = N, step = 1;
   if (!increase)
   {
       ib = N - 1; ie = -1, step = -1;
   }
   for (int i = ib; i != ie; i += step)
   {
       u_t n_below1 = bit1.query(vh[i][1]);
       u_t n_below2 = bit2.query(vh[i][1]);
       n_mono += n_below2;
       bit1.update(vh[i][1] + 1, 1); // we don't want zero
       bit2.update(vh[i][1] + 1, n_below1); // we don't want zero
   }
   return n_mono;
}

void King::print_solution(ostream &fo) const
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

    void (King::*psolve)() =
        (naive ? &King::solve_naive : &King::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        King king(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (king.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        king.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
