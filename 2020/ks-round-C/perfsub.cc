// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>
#include <deque>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<int> vi_t;

static unsigned dbg_flags;

vi_t squares;
void init_squares()
{
    int sq = 0;
    squares.reserve(10000);
    for (int k = 0; sq < 10000000; ++k)
    {
        sq = k*k;
        squares.push_back(sq);
    }
}

int iroot(int n)
{
    int ret = 0;
    if (n > 0)
    {
        int high = n + 1;
        while (ret + 1 < high)
        {
            int mid = (ret + high)/2;
            int sq = mid*mid;
            if (sq <= n)
            {
                ret = mid;
            }
            else
            {
                high = mid;
            }
        }
    }
    return ret;
}

bool is_square(int n)
{
    int r = iroot(n);
    bool ret = (r*r == n);
    return ret;
}

class PerfectSubArray
{
 public:
    PerfectSubArray(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n;
    vi_t a;
    ull_t solution;
};

PerfectSubArray::PerfectSubArray(istream& fi) : solution(0)
{
    fi >> n;
    copy_n(istream_iterator<int>(fi), n, back_inserter(a));
}

void PerfectSubArray::solve_naive()
{
    for (u_t b = 0; b < n; ++b)
    {
        for (u_t e = b + 1; e <= n; ++e)
        {
            int s = accumulate(a.begin() + b, a.begin() + e, 0);
            if (is_square(s))
            {
                ++solution;
            }
        }
    }
}

void PerfectSubArray::solve()
{
    static const ll_t MIN = -100, MAX = 100;
    vector<ll_t> next2count(n * (MAX + 1 - MIN), 0);
    const ll_t asz = next2count.size();
    ll_t pzero = n * (-MIN);
    for (ll_t r = 0, sq = r*r; sq + pzero < asz; ++r, sq = r*r)
    {
        next2count[sq + pzero] = 1;
    }
    for (u_t i = 0; i != n; ++i)
    {
        const int x = a[i];
        const ull_t c = next2count[x + pzero];
        solution += c;
        pzero += x;
        ll_t ilimit = min(asz, MAX*MAX*(n - 1) + pzero);
        for (ll_t r = 0, sq = r*r; sq + pzero < ilimit; ++r, sq = r*r)
        {
            ++(next2count[sq + pzero]);
        }
    }
}

#if 0
void PerfectSubArray::solve()
{
    static ll_t MIN = -100, MAX = 100;
    deque<ull_t> next2count(size_t(MAX + 1 - MIN), 0);
    ll_t pzero = -MIN;
    for (u_t r = 0, sq = r*r; int(sq) <= MAX; ++r, sq = r*r)
    {
        next2count[pzero + sq] = 1;
    }
    for (u_t i = 0; i != n; ++i)
    {
        int x = a[i];
        const ull_t c = next2count[x + pzero];
        solution += c;
        pzero += x;
        if (pzero + MIN < 0)
        {
            size_t add = -(pzero + MIN);
            next2count.insert(next2count.begin(), add, 0);
            pzero = -MIN;
        }
        if (next2count.size() <= size_t(pzero + MAX))
        {
            size_t add = pzero + MAX + 1 - next2count.size();
            next2count.insert(next2count.end(), add, 0);
        }
        const ll_t n2csz = next2count.size();
        for (ll_t r = 0, sq = 0; sq + pzero < n2csz; ++r, sq = r*r)
        {
            ++next2count[sq + pzero];
        }
    }
}
#endif

void PerfectSubArray::print_solution(ostream &fo) const
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

    void (PerfectSubArray::*psolve)() =
        (naive ? &PerfectSubArray::solve_naive : &PerfectSubArray::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    init_squares();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        PerfectSubArray perfsub(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (perfsub.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        perfsub.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
