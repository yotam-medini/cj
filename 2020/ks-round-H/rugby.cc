// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <array>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef array<ll_t, 2> ll2_t;
typedef vector<ll_t> vll_t;
typedef vector<ll2_t> vll2_t;

static unsigned dbg_flags;

class Rugby
{
 public:
    Rugby(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    ll_t move_price(const ll2_t& target) const;
    ll_t x_solve() const;
    ll_t y_solve() const;
    ll_t delta_sum(vll_t& a, ll_t x) const;
    u_t N;
    vll2_t players;
    ll_t solution;
    ll_t x_solution, y_solution;
};

Rugby::Rugby(istream& fi) : solution(0)
{
    fi >> N;
    players.reserve(N);
    while (players.size() < N)
    {
        ll_t x, y;
	fi >> x >> y;
	players.push_back(ll2_t{x, y});
    }
}

void Rugby::solve_naive()
{
    ll2_t xymin = players[0], xymax = players[0];
    for (const ll2_t& player: players)
    {
        for (u_t i: {0, 1})
	{
	    if (xymin[i] > player[i])
	    {
	        xymin[i] = player[i];
	    }
	    if (xymax[i] < player[i])
	    {
	        xymax[i] = player[i];
	    }
	}
    }
    const ll_t Nm1 = N - 1;
    solution = N * ((xymax[0] - xymin[0]) + N + (xymax[1] - xymin[1]));
    ll2_t target, best;
    for (target[0] = xymin[0] - Nm1; target[0] <= xymax[0] + Nm1; ++target[0])
    {
        for (target[1] = xymin[1]; target[1] <= xymax[1]; ++target[1])
	{
	    ll_t price = move_price(target);
	    if (solution > price)
	    {
	        solution = price;
                best = target;
	    }
	}
    }
    if (dbg_flags & 0x1) { cerr << "best=("<<best[0]<<", "<<best[1]<<")\n"; }
}

void Rugby::solve()
{
    solution = x_solve() + y_solve();
}

void minby(ll_t& v, ll_t x)
{
    if (v > x)
    {
        v = x;
    }
}

void maxby(ll_t& v, ll_t x)
{
    if (v < x)
    {
        v = x;
    }
}

ll_t Rugby::x_solve() const
{
    ll_t x_price = 0;
    vll_t xs; xs.reserve(N);
    for (const ll2_t& player: players)
    {
        xs.push_back(player[0]);
    }
    sort(xs.begin(), xs.end());
    ll_t low = xs[0];
    ll_t high = xs[N - 1];
    for (size_t i = 0, ie = xs.size(); i != ie; ++i)
    {
        const ll_t curr = xs[i];
        minby(low, curr - i);
        maxby(high, curr + (N - 1) - i);
    }
    high -= (N - 1); // we base delta_sum on xs
    if (high < low)
    {
        swap(low, high);
    }
    bool found = false;
    while (low + 2 <= high)
    {
        ll_t mid = (low + high)/2;
        ll_t prices[3];
        for (ll_t i = 0; i < 3; ++i)
        {
            prices[i] = delta_sum(xs, mid + (i - 1));
        }
        if ((prices[0] >= prices[1]) && (prices[1] <= prices[2]))
        {
            low = high = mid;
            x_price = prices[1];
            found = true;
        }
        else if ((prices[0] == prices[1]) || (prices[1] == prices[2]))
        {
            low = high = mid;
            x_price = prices[1];
            found = true;
        }
        else if (prices[0] < prices[1])
        {
            high = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }
    ll_t x_best = low;
    if (!found)
    {
        x_price = delta_sum(xs, low);
        if (low != high)
        {
            ll_t h_price = delta_sum(xs, high);
            if (x_price > h_price)
            {
                x_price = h_price;
                x_best = high;
            }
        }
    }
    if (dbg_flags & 0x1) {
        cerr << "x_price=" <<x_price << ", x_best=" << x_best << '\n'; }
    
    return x_price;
}

ll_t Rugby::delta_sum(vll_t& a, ll_t x) const
{
    ll_t delta = 0;
    for (size_t i = 0; i < N; ++i)
    {
        delta += llabs(x + i - a[i]);
    }
    return delta;
}

ll_t Rugby::y_solve() const
{
    vll_t ys; ys.reserve(N);
    for (const ll2_t& player: players)
    {
        ys.push_back(player[1]);
    }
    sort(ys.begin(), ys.end());
    ll_t ymed = ys[N / 2];
    ll_t y_price = 0;
    for (size_t i = 0; i < size_t(N / 2); ++i)
    {
        y_price += (ymed - ys[i]);
    }
    for (size_t i = size_t(N / 2) + 1; i < size_t(N); ++i)
    {
        y_price += ys[i] - ymed;
    }
    if (dbg_flags & 0x1) {
        cerr << "y_price="<<y_price << ", ymed=" << ymed << '\n'; }
    return y_price;
}

ll_t Rugby::move_price(const ll2_t& target) const
{
    vll2_t splayers(players);
    sort(splayers.begin(), splayers.end());
    ll_t price = 0;
    for (u_t i = 0; i < N; ++i)
    {
        const ll2_t& player = splayers[i];
	const ll_t xtarget = target[0] + i;
	ll_t xdelta = llabs(xtarget - player[0]);
	ll_t ydelta = llabs(target[1] - player[1]);
	ll_t delta = xdelta + ydelta;
	price += delta;
    }
    return price;
}

void Rugby::print_solution(ostream &fo) const
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

    void (Rugby::*psolve)() =
        (naive ? &Rugby::solve_naive : &Rugby::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Rugby rugby(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (rugby.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        rugby.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
