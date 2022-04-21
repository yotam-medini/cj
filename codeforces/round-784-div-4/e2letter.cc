// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
// #include <map>
// #include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

class E2letter
{
 public:
    E2letter(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N;
    vs_t S2s;
    ull_t solution;
};

E2letter::E2letter(istream& fi) : solution(0)
{
    fi >> N;
     copy_n(istream_iterator<string>(fi), N, back_inserter(S2s));
}

void E2letter::solve_naive()
{
    for (u_t i = 0; i < N; ++i)
    {
        const string& si = S2s[i];
        for (u_t j = i + 1; j < N; ++j)
        {
            const string& sj = S2s[j];
            if ((si[0] == sj[0]) != (si[1] == sj[1]))
            {
                ++solution;
            }
        }
    }
}

void E2letter::solve()
{
    const u_t NC = 'k' + 1 - 'a';
    ull_t count[NC][NC];
    for (u_t i = 0; i < NC; ++i)
    {
        for (u_t j = 0; j < NC; ++j)
        {
            count[i][j] = 0;
        }
    }
    for (const string& s: S2s)
    {
        ++count[s[0] - 'a'][s[1] - 'a'];
    }

    for (u_t i0 = 0; i0 < NC; ++i0)
    {
        for (u_t i1 = 0; i1 < NC; ++i1)
        {
            const ull_t count_i = count[i0][i1];
            for (u_t j0 = 0; j0 < NC; ++j0)
            {
                for (u_t j1 = 0; j1 < NC; ++j1)
                {
                    if ((i0 == j0) != (i1 == j1))
                    {
                        const ull_t count_j = count[j0][j1];
                        ull_t p = count_i * count_j;
                        solution += p;
                    }
                }
            }
        }
    }
    solution /= 2;
}

void E2letter::print_solution(ostream &fo) const
{
    fo << solution << '\n';
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

    void (E2letter::*psolve)() =
        (naive ? &E2letter::solve_naive : &E2letter::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        E2letter e2letter(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (e2letter.*psolve)();
        e2letter.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
