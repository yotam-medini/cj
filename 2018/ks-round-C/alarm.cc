// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
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
typedef vector<ull_t> vull_t;
typedef vector<vull_t> vvull_t;

static unsigned dbg_flags;

class Alarm
{
 public:
    Alarm(istream& fi);
    void solve_naive();
    void solve1();
    void solve();
    void print_solution(ostream&) const;
    void gen_xya();
 private:
    enum {Nmax = 1000000, Kmax = 10000};
    static const ull_t MOD;
    ull_t n_powers_sum_1_to_k(ull_t n) const;
    ull_t invmod(ull_t n) const;
    ull_t power_mod(ull_t n, ull_t p) const;
    u_t N, K;
    ull_t x1, y1, C, D, E1, E2, F;
    ull_t solution;
    vull_t x, y, a;
};
const ull_t Alarm::MOD = 1000000007;

Alarm::Alarm(istream& fi) : solution(0)
{
    fi >> N >> K >> x1 >> y1 >> C >> D >> E1 >> E2 >> F;
}

void Alarm::solve_naive()
{
    gen_xya();
    ull_t s = 0;
    for (u_t p = 1; p <= K; ++p)
    {
        ull_t power = 0;
        for (u_t b = 1; b <= N; ++b)
        {
            for (u_t e = b; e <= N; ++e)
            {
                ull_t be_sum = 0;
                for (u_t j = b; j <= e; ++j)
                {
                    const u_t i = (j - b) + 1;
                    ull_t i_pwr = 1;
                    for (u_t pp = 0; pp < p; ++pp)
                    {
                        i_pwr = (i_pwr * i) % MOD;
                    }
                    ull_t apwr = (a[j] * i_pwr) % MOD;
                    if (dbg_flags & 0x2) { cerr << "apwr=" << apwr << '\n'; }
                    be_sum = (be_sum + apwr) % MOD;
                }
                if (dbg_flags & 0x2) { 
                   cerr << "b="<<b << ", e="<<e << ", be_sum="<<be_sum << '\n'; }
                power = (power + be_sum) % MOD;
            }
        }
        if (dbg_flags & 0x1) { cerr << "p="<<p << ", power="<<power << '\n'; }
        s = (s + power) % MOD;
    }
    solution = s;
}

void Alarm::solve1()
{
     gen_xya();
    vull_t ipower(size_t(N + 1), 1);
    ipower[0] = 0;
    vull_t ipower_sums(size_t(N + 1), 0);
    for (ull_t i = 1; i <= K; ++i)
    {
        for (ull_t n = 1; n <= N; ++n)
        {
            ipower[n] = (ipower[n] * n) % MOD;
            ipower_sums[n] = (ipower_sums[n - 1] + ipower[n]) % MOD;
        }
        ull_t ip = 0;
        for (ull_t j = 1, mult = N; j <= N; ++j, --mult)
        {
            ull_t add = (a[j] * ipower_sums[j]) % MOD;
            add = (mult * add) % MOD;
            ip = (ip + add) % MOD;
        }
        if (dbg_flags & 0x1) { cerr << "iPower(i=" << i << ")=" << ip << '\n'; }
        solution = (solution + ip) % MOD;
    }
}

void Alarm::solve()
{
    // S_{i=1}^K a^i = (a^(K+1} - a)/(a - 1) = a^K - 1
    vull_t ipower_sums(size_t(N + 1), 0);
    ipower_sums[1] = K;
    for (ull_t n = 2; n <= N; ++n)
    {
        ull_t sk = n_powers_sum_1_to_k(n);
        ipower_sums[n] = (ipower_sums[n - 1] + sk) % MOD;
    }
    gen_xya();
    for (ull_t j = 1, mult = N; j <= N; ++j, --mult)
    {
        ull_t add = (a[j] * ipower_sums[j]) % MOD;
        add = (mult * add) % MOD;
        solution = (solution + add) % MOD;
    }
}

void Alarm::gen_xya()
{
   x.reserve(N + 1);
   y.reserve(N + 1);
   a.reserve(N + 1);
   x.push_back(0); // dummy
   y.push_back(0); // dummy
   a.push_back(0); // dummy
   x.push_back(x1);
   y.push_back(y1);
   // xi = ( C × xi-1 + D × yi-1 + E1 ) modulo F.
   // yi = ( D × xi-1 + C × yi-1 + E2 ) modulo F.
   // We define Ai = ( xi + yi ) modulo F, for all i = 1 to N.
   for (u_t i = 2; i <= N; ++i)
   {
       ull_t xi = (C * x[i - 1] + D * y[i - 1] + E1) % F;
       ull_t yi = (D * x[i - 1] + C * y[i - 1] + E2) % F;
       x.push_back(xi);
       y.push_back(yi);
   }
   for (u_t i = 1; i <= N; ++i)
   {
       ull_t ai = (x[i] + y[i]) % F;
       a.push_back(ai);
   }
}

// S_{i=1}^K n^i = (n^{K+1} - n)/(n - 1)
ull_t Alarm::n_powers_sum_1_to_k(ull_t n) const
{
    ull_t ret = K;
    if (n > 1)
    {
        ull_t np = power_mod(n, K + 1);
        ull_t numerator = (np + (MOD - n)) % MOD;
        ull_t denom = invmod(n - 1);
        ret = (numerator * denom) % MOD;
    }
    return ret;
}

ull_t Alarm::invmod(ull_t n) const
{
    static vull_t _invmod;
    if (_invmod.empty())
    {   
        const ull_t MOD_m1 = MOD - 1;
        const ull_t Nmax_p1 = Nmax + 1;
        _invmod.insert(_invmod.end(), size_t(Nmax_p1), 0);
        _invmod[1] = 1;
        ull_t g = 5, p = 1;
        while (g != 1)
        {
            if ((dbg_flags & 0x1) && ((p & (p - 1)) == 0))
            {
                cerr << "1st-pass: p="<<p << '\n';
            }
            if (g <= Nmax_p1)
            {
                ull_t pinv = MOD_m1 - p;
                ull_t n_inv = power_mod(5, pinv);
                _invmod[g] = n_inv;
            }
            ++p;
            g = (ull_t(5) * g) % MOD;
        }
    }
    ull_t ret = _invmod[n];
    return ret;
}

ull_t Alarm::power_mod(ull_t n, ull_t p) const
{
    ull_t ret = 1;
    ull_t p2 = n;
    while (p)
    {
        if (p & 1)
        {
            ret = (ret * p2) % MOD;
        }
        p2 = (p2 * p2) % MOD;
        p = p / ull_t(2);
    }
    return ret;
}

void Alarm::print_solution(ostream &fo) const
{
   fo << ' ' << solution;
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

    void (Alarm::*psolve)() =
        (naive ? &Alarm::solve_naive : &Alarm::solve);
    if (solve_ver == 1) { psolve = &Alarm::solve1; }
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Alarm alarm(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (alarm.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        alarm.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
