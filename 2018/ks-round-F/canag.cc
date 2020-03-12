// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<char> vc_t;
typedef array<u_t, 26> au26_t;
typedef vector<au26_t> vau26_t;

static unsigned dbg_flags;

class CommonAnagram
{
 public:
    CommonAnagram(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    void ccount(vau26_t& ccs, const string& s) const;
    void delta(au26_t& cc_delta, const au26_t& high, const au26_t& low)
    {
        for (u_t ci = 0; ci < 26; ++ci)
        {
            cc_delta[ci] = high[ci] - low[ci];
        }
    }
    u_t L;
    string A, B;
    u_t solution;
};

CommonAnagram::CommonAnagram(istream& fi) : solution(0)
{
    fi >> L >> A >> B;
}

void CommonAnagram::solve_naive()
{
    for (u_t b = 0; b < L; ++b)
    {
        for (u_t e = b + 1; e <= L; ++e)
        {
            vc_t sa;
            for (u_t ci = b; ci < e; ++ci)
            {
                sa.push_back(A[ci]);
            }
            sort(sa.begin(), sa.end());

            bool match = false;
            for (u_t bb = 0; (bb < L) && !match; ++bb)
            {
                for (u_t be = bb + 1; (be <= L) && !match; ++be)
                {
                    vc_t sb;
                    for (u_t ci = bb; ci < be; ++ci)
                    {
                        sb.push_back(B[ci]);
                    }
                    sort(sb.begin(), sb.end());
                    match = (sa == sb);
                    if (match)
                    {
                        ++solution;
                    }
                }
            }
        }
    }
}

void CommonAnagram::solve()
{
    vau26_t ccsa, ccsb;
    ccount(ccsa, A);
    ccount(ccsb, B);

    for (u_t b = 0; b < L; ++b)
    {
        for (u_t e = b + 1; e <= L; ++e)
        {
            au26_t adelta;
            delta(adelta, ccsa[e], ccsa[b]);

            bool match = false;
            for (u_t bb = 0; (bb < L) && !match; ++bb)
            {
                for (u_t be = bb + 1; (be <= L) && !match; ++be)
                {
                    au26_t bdelta;
                    delta(bdelta, ccsb[be], ccsb[bb]);
                    match = (adelta == bdelta);
                    if (match)
                    {
                        ++solution;
                    }
                }
            }
        }
    }
}

void CommonAnagram::ccount(vau26_t& ccs, const string& s) const
{
    au26_t cc{0};
    ccs.push_back(cc);
    for (char c: s)
    {
        ++cc[c - 'A'];
        ccs.push_back(cc);
    } 
}

void CommonAnagram::print_solution(ostream &fo) const
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

    void (CommonAnagram::*psolve)() =
        (naive ? &CommonAnagram::solve_naive : &CommonAnagram::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        CommonAnagram canag(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (canag.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        canag.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
