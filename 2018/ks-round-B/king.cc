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

class King
{
 public:
    King(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void gen_vh();
    u_t N;
    ull_t V1, H1, A, B, C, D, E, F, M;
    vu_t v, h;
    ull_t solution;
};

King::King(istream& fi) : solution(0)
{
    fi >> N >> V1 >> H1 >> A >> B >> C >> D >> E >> F >> M;
}

void King::solve_naive()
{
    gen_vh();
    vau2_t vh;
    vh.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
        vh.push_back(au2_t{v[i], h[i]});
    }
    sort(vh.begin(), vh.end());
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
    solve_naive();
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
