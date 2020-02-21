// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<char> vc_t;
typedef vector<string> vs_t;


static unsigned dbg_flags;

class Scrambled
{
 public:
    Scrambled(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void generate_s();
    bool word_is_legal(const string& w) const;
    u_t L, N;
    vs_t lang;
    char c1, c2;
    ull_t A, B, C, D;
    string s;
    u_t solution;
};

Scrambled::Scrambled(istream& fi) : solution(0)
{
    string s1, s2;
    fi >> L;
    lang.reserve(L);
    for (u_t li = 0; li < L; ++li)
    {
        string w;
        fi >> w;
        lang.push_back(w);
    }
    fi >> s1 >> s2;
    c1 = s1[0];
    c2 = s2[0];
    fi >> N >> A >> B >> C >> D;
}

void Scrambled::solve_naive()
{
    generate_s();
    for (const string& w: lang)
    {
        if (word_is_legal(w))
        {
            ++solution;
        }
    }
}

void Scrambled::solve()
{
    solve_naive();
}

void Scrambled::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

void Scrambled::generate_s()
{
    // xi = ( A * xi-1 + B * xi-2 + C ) modulo D.
    // Si = char(97 + ( xi modulo 26 )) 
    s.reserve(N);
    s.push_back(c1);
    s.push_back(c2);
    ull_t x0 = c1, x1 = c2;
    for (u_t ci = 2; ci < N; ++ci)
    {
        ull_t xi = (A*x0 + B*x1 + C) % D;
        char c = 'a' + (xi % 26);
        s.push_back(c);
        x0 = x1;
        x1 = xi;
    }
}

bool Scrambled::word_is_legal(const string& w) const
{
    bool legal = false;
    const size_t sz = w.size();
    if (sz == 1)
    {
        legal = (s.find(w[0]) != string::npos);
    }
    else
    {
        for (u_t si = 0; (si + sz < N) && !legal; ++si)
        {
            legal = (s[si] == w[0]) && (s[si + sz - 1] == w[sz - 1]);
            if (legal)
            {
                vc_t s_internal, w_internal;
                for (u_t ci = 1; ci < sz - 1; ++ci)
                {
                    s_internal.push_back(s[si + ci]);
                    w_internal.push_back(w[ci]);
                }
                sort(s_internal.begin(), s_internal.end());
                sort(w_internal.begin(), w_internal.end());
                legal = (s_internal == w_internal);
            }
        }
    }
    return legal;
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

    void (Scrambled::*psolve)() =
        (naive ? &Scrambled::solve_naive : &Scrambled::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Scrambled scrambled(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (scrambled.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        scrambled.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
