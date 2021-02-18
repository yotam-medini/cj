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
    u_t N;
    vll2_t players;
    ll_t solution;
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
    ll2_t target;
    for (target[0] = xymin[0] - Nm1; target[0] <= xymax[0] + Nm1; ++target[0])
    {
        for (target[1] = xymin[1]; target[1] <= xymax[1]; ++target[1])
	{
	    ll_t price = move_price(target);
	    if (solution > price)
	    {
	        solution = price;
	    }
	}
    }
}

void Rugby::solve()
{
    solve_naive();
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
