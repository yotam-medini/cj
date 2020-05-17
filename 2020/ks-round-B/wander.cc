// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <vector>
#include <cstdlib>
#include <cmath>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

#define DP_MAX 2*100000
// dp[i] is going to store Log ( i !) in base 2 
double dp[DP_MAX]; 
  
void precompute() 
{ 
    // Preprocess all the logarithm value on base 2 
    for (int i=2; i < DP_MAX; ++i) 
        dp[i] = log2(i) + dp[i-1]; 
} 

double probability(int k, int n) 
{ 
    double ans = 0; // Initialize result 
  
    // Iterate from k heads to n heads 
    for (int i=k; i <= n; ++i) 
    { 
        double res = dp[n] - dp[i] - dp[n-i] - n; 
        ans += pow(2.0, res); 
    } 
  
    return ans; 
} 
  
class Wander
{
 public:
    Wander(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    double mn_from(u_t m, u_t n) const;
    u_t w, h, l, u, r, d;
    double solution;
};

Wander::Wander(istream& fi) : solution(0)
{
    fi >> w >> h >> l >> u >> r >> d;
}

void Wander::solve_naive()
{
    double solx = ((r < w) && (u > 1) ? mn_from(r + u - 2, r) : 0);
    double soly = ((d < h) && (l > 1) ? mn_from(d + l - 2, d) : 0);
    solution = solx + soly;
}

void Wander::solve()
{
    solve_naive();
}

double Wander::mn_from(u_t m, u_t n) const
{
    return probability(n, m);
}

#if 0
double Wander::mn_from(u_t m, u_t n) const // pascal
{
    vull_t row;
    row.push_back(1);
    for (u_t p = 1; p <= m; ++p)
    {
        vull_t next_row;
        next_row.reserve(p + 1);
        next_row.push_back(1);
        for (u_t k = 1; k < p; ++k)
        {
            ull_t pascal = row[k - 1] + row[k];
            next_row.push_back(pascal);
        }
        next_row.push_back(1);
        swap(row, next_row);
    }
    ull_t num = 0, denom = 0;
    for (u_t i = n; i < m + 1; ++i)
    {
        num += row[i];
    }
    for (u_t i = 0; i < m + 1; ++i)
    {
        denom += row[i];
    }
    double ret = double(num)/double(denom);
    return ret;
}
#endif

void Wander::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

int main(int argc, char ** argv)
{
    const string dash("-");

    precompute();

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

    void (Wander::*psolve)() =
        (naive ? &Wander::solve_naive : &Wander::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Wander wander(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (wander.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        wander.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
