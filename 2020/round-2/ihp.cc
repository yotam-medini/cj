// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class IHP
{
 public:
    IHP(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ull_t L, R;
    ull_t solN, solL, solR;
};

IHP::IHP(istream& fi) : solN(0), solL(0), solR(0)
{
    fi >> L >> R;
}

void IHP::solve_naive()
{
    solL = L;
    solR = R;
    ull_t i = 1;
    while ((solL >= i) || (solR >= i))
    {
        if (solL >= solR)
        {
            solL -= i;
        }
        else
        {
            solR -= i;
        }
        ++i;
    }
    solN = i - 1;
}


ull_t isqrt(ull_t n)
{
    ull_t low = 1, high = n + 1;
    while (low < high - 1)
    {
        ull_t mid = low + (high - low)/2;
        if (mid * mid <= n)
        {
            low = mid;
        }
        else
        {
            high = mid;
        }
    }
    return low;
}

void IHP::solve()
{
    ull_t big = max(L, R);
    ull_t small = min(L, R);
    ull_t delta = big - small;

    // Sum 1..N  = (N*(N+1))/2 <= delta
    // N^2 + N - 2*dela <= 0
    // x = (-1 + sqrt(1 +8*delta)) / 2
    ull_t N1 = isqrt(8*delta + 1);
    if (N1 > 0)
    {
        N1 = (N1 - 1)/2;
    }
    // ull_t s1 = (N1*(N1 + 1))/2;
    ull_t s1 = (N1 % 2 == 0 ? (N1 / 2)*(N1 + 1) : N1*((N1 + 1)/2));
    big -= s1;

    // Sum N1+1 + ... + N1+N2 = M2*(N1 + 1 + N1 + 1 + 2*(N2 - 1))/2 <= big
    // N2*(N2 + N1) - big = 
    // N2^2 + N1*N2 - big <= 0
    ull_t N2 = isqrt(N1*N1 + 4*big);
    N2 = (N2 >= N1 ? (N2 - N1)/2 : 0);
    // ull_t s2 = (N2*(2*N1 + 2 + 2*(N2 - 1)))/2;
    ull_t s2 = N2*(N1 + N2);
    big -= s2;

    ull_t N3 = 0, s3 = 0;
    if (N2 > 0)
    {
        N3 = N2 - 1;
        if (N3 > 0)
        {
            // s3 = (N3*(N1 + 2 + N1 + 2 + 2*(N3 - 1)))/2;
            s3 = N3*(N1 + N3 + 1);
        }
        small -= s3;
        ull_t N123 = N1 + N2 + N3;
        if (small >= N123 + 1)
        {
            ++N3;
            ++N123;
            s3 += N123;
            small -= N3;
        }
    }
    solN = N1 + N2 + N3;
    solL = L;
    solR = R;
    *((L >= R) ? &solL : &solR) -= s1;
    if (solL >= solR)
    {
        solL -= s2;
        solR -= s3;
    }
    else
    {
       solR -= s2;        
       solL -= s3;        
    }
    if (dbg_flags & 0x1) { cerr << "delta="<<delta <<
        ", s1="<<s1 << ", s2="<< s2 << ", s3="<<s3 <<
        ", N1="<<N1 << ", N2="<<N2 << ", N3="<<N3 << "\n"; }
}

void IHP::print_solution(ostream &fo) const
{
    fo << ' ' << solN << ' ' << solL << ' ' << solR;
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

    void (IHP::*psolve)() =
        (naive ? &IHP::solve_naive : &IHP::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        IHP ihp(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (ihp.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        ihp.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
