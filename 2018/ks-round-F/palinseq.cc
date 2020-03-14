// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

class PalindromeSequence
{
 public:
    PalindromeSequence(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    static const char* az;
    u_t L, N, K;
    u_t solution;
};
const char* PalindromeSequence::az = "abcdefghijklmnopqrstuvwxyz";

PalindromeSequence::PalindromeSequence(istream& fi) : solution(u_t(-1))
{
    fi >> L >> N >> K;
}

void PalindromeSequence::solve_naive()
{
    vs_t lang;
    for (u_t n = 1; n <= 2; ++n)
    {
        for (u_t i = (N >= n ? 0 : L) ; i != L; ++i)
        {
            string s(string(size_t(n), 'a' + i));
            lang.push_back(s);
        }     
    }
    for (u_t n = 3; n <= N; ++n)
    {
        for (u_t si = 0, se = lang.size(); si != se; ++si)
        {
            const string ss = lang[si]; // not ref, since lang grows
            if (ss.size() == n - 2) // not efficient, but naive
            {
                for (u_t azi = 0; azi != L; ++azi)
                {
                    string sc(string(size_t(1), az[azi]));
                    string s = sc + ss + sc;
                    lang.push_back(s);
                }
            }
        }
    }
    solution = 0;
    if (K < lang.size())
    {
        sort(lang.begin(), lang.end());
        const string& target = lang[K - 1];
        solution = target.size();
    }
}

void PalindromeSequence::solve()
{
}

void PalindromeSequence::print_solution(ostream &fo) const
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

    void (PalindromeSequence::*psolve)() =
        (naive ? &PalindromeSequence::solve_naive : &PalindromeSequence::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PalindromeSequence palinseq(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (palinseq.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        palinseq.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
