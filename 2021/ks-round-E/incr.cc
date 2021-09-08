// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <numeric>

#include <cstdlib>
#include <cstdio>
#include <cmath>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<double> vd_t;

static unsigned dbg_flags;

string dtos(const double x, u_t precision=6)
{
    char buf[0x20];
    snprintf(buf, sizeof(buf), "%.*f", precision, x);
    string ret(buf);
    return ret;
}

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class Incr
{
 public:
    Incr(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    double get_expection(u_t n);
    ull_t N;
    double solution;
    static vd_t expects;
};
vd_t Incr::expects;

Incr::Incr(istream& fi)
{
    fi >> N;
}

void Incr::solve_naive()
{
    ull_t total = 0;
    ull_t np = 0;
    vu_t p;
    permutation_first(p, N);
    
    for (bool more = true; more; more = permutation_next(p))
    {
        ull_t inc_size = 1;
        u_t last = p[0];
        for (u_t i = 1; i < N; ++i)
        {
            if (last < p[i])
            {
                last = p[i];
                ++inc_size;
            }
        }
        total += inc_size;
        ++np;
    }
    if (dbg_flags & 0x1) {
        cerr << "N="<<N << ", np="<<np << ", total="<<total << '\n'; }
    solution = double(total) / double(np);
}

/*********************************
E(0) = 0
E(n) = 1 + (1/n)\cdot\sum_{k=1}^n E(n-k)
     = 1 + (1/n)\cdot\sum_{k=0}^{n-1} E(k)
\rightarrow
E(n+1) = (n(E(n)-1) + E(n))/(n + 1)
\rightarrow
E(n) = E(n-1) + 1/n = \sum_{k=1}^n = 1/n = \log(n) 

H(n) = \sum_{k=1}^n = log(n) + \gamma + \epsilon_k 
where  \epsilon_k \sim 1/(2k)
*********************************/

void Incr::solve()
{
    if (N < 1000000)
    {
        solution = get_expection(N);
    }
    else
    {
        static const double  euler_mascheroni_gamma = 0.577215664901532;
        solution = log(N) + euler_mascheroni_gamma;
    }
}

double Incr::get_expection(u_t n)
{
    double ret = 0.;
    if (n >= expects.size())
    {
        if (expects.empty())
        {
            expects.push_back(0.);
        }
        expects.reserve(n + 1);
        for (u_t m = expects.size(); m <= n; ++m)
        {
#if 0
            double total = 0.;
            for (u_t k = 1; k < m; ++k)
            {
                total += expects[k];
            }
            total = 1. + total / double(m);
#endif
            double total = expects.back() + 1./double(m);
            expects.push_back(total);
        }
    }    
    ret = expects[n];
    return ret;
}

void Incr::print_solution(ostream &fo) const
{
    fo << ' ' << dtos(solution, 8);
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
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

    void (Incr::*psolve)() =
        (naive ? &Incr::solve_naive : &Incr::solve);
    if (solve_ver == 1) { psolve = &Incr::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Incr incr(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (incr.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        incr.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
