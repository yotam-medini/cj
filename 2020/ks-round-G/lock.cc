// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<ull_t> vull_t;

static unsigned dbg_flags;

class Lock
{
 public:
    Lock(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    typedef vull_t::const_iterator citer_t;
    ull_t price(ull_t x) const;
    u_t w;
    ull_t n;
    vull_t wheels;
    ull_t solution;

    vull_t swheels;
    vull_t wheel_sums;
};

Lock::Lock(istream& fi) : solution(0)
{
    fi >> w >> n;
    wheels.reserve(n);
    copy_n(istream_iterator<ull_t>(fi), w, back_inserter(wheels));
}

void minby(ull_t& v, ull_t x)
{
    if (v > x)
    {
        v = x;
    }
}

void Lock::solve_naive()
{
    solution = n*w;
    for (u_t candid = 0; candid < n; ++candid)
    {
        ull_t changes = 0;
        for (u_t wi = 0; wi < w; ++wi)
        {
            ull_t x0 = candid;
            ull_t x1 = wheels[wi] - 1;
            if (x1 < x0)
            {
                swap(x0, x1);
            }
            ull_t delta = min(x1 - x0, x0 + n - x1);
            changes += delta;
        }
        minby(solution, changes);
    }
}

void Lock::solve()
{
    swheels.reserve(w);
    for (ull_t x: wheels)
    {
        swheels.push_back(x - 1); // to be 0-based
    }
    sort(swheels.begin(), swheels.end());
    wheel_sums.reserve(2*w + 1);
    wheel_sums.push_back(0);
    for (u_t loop = 0; loop < 2; ++loop)
    {
        for (ull_t sw: swheels)
        {
            wheel_sums.push_back(wheel_sums.back() + sw);
        }
    }
    ull_t half = n / 2, half1 = (n + 1) / 2;
    solution = n*w;
    for (ull_t x: swheels)
    {
        ull_t p = price(x);
        minby(solution, p);
        p = price((x + half) %  n);
        minby(solution, p);
        if (half != half1)
        {
            p = price((x + half1) % n);
            minby(solution, p);
        }
    }
}

ull_t Lock::price(ull_t x) const
{
    ull_t total = 0;
    const ull_t half1 = (n + 1) / 2;
    const ull_t xhalf = (x + half1) % n;
    const citer_t xiter = lower_bound(swheels.begin(), swheels.end(), x);
    const size_t ix = xiter - swheels.begin();
    const citer_t ziter = lower_bound(swheels.begin(), swheels.end(), 0);
    const size_t iz = ziter - swheels.begin();
    // ull_t fwd = 0, bwd = 0;
    const auto er = equal_range(swheels.begin(), swheels.end(), xhalf);
    const size_t h0 = er.first - swheels.begin();
    const size_t h1 = (n % 2 == 0 ? er.second - swheels.begin() : h0);
    if (dbg_flags & 0x1) { cerr << "x="<<x; }
    if (n % 2 == 0)
    {
        size_t nh = er.second - er.first; // == h1 - h0
        total += nh * half1;
        if (dbg_flags & 0x1) { cerr << ", (H)="<<total; }
    }
    if (x < xhalf) //    [0, x), [x, h0), [h1, n)
    {
        const ull_t z2x = ix*x - wheel_sums[ix];
        const ull_t x2h = (wheel_sums[h0] - wheel_sums[ix]) - (h0 - ix)*x;
        const ull_t h2n = (w - h1)*(n + x) - (wheel_sums[w] - wheel_sums[h1]);
        total += z2x + x2h + h2n;
        if (dbg_flags & 0x1) { 
           cerr << ", z2x="<<z2x << ", x2h="<<x2h << ", h2n="<<h2n; }
    }
    else // (xhalf < x): [x, n), [0, h0), [h1, x)
    {
        const ull_t x2n = (wheel_sums[w] - wheel_sums[ix]) - (w - ix)*x;
        const ull_t z2h = (wheel_sums[h0] - wheel_sums[iz]) + (h0 - iz)*(n - x);
        const ull_t h2p = (ix - h1)*x - (wheel_sums[ix] - wheel_sums[h1]);
        total += x2n + z2h + h2p;
        if (dbg_flags & 0x1) { 
           cerr << ", x2n="<<x2n << ", z2h="<<z2h << ", h2p="<<h2p; }
    }
    if (dbg_flags & 0x1) { cerr << ", total=" << total << '\n'; }
    return total;
}

void Lock::print_solution(ostream &fo) const
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

    void (Lock::*psolve)() =
        (naive ? &Lock::solve_naive : &Lock::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Lock lock(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (lock.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        lock.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
