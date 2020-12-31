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
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Bakery
{
 public:
    Bakery(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t n, m;
    vu_t a;
    vu_t k;
    vu_t solution;
};

Bakery::Bakery(istream& fi)
{
    fi >> n >> m;
    copy_n(istream_iterator<u_t>(fi), n, back_inserter(a));
    copy_n(istream_iterator<u_t>(fi), m, back_inserter(k));
}

void Bakery::solve_naive()
{
    for (u_t ki = 0; ki < m; ++ki)
    {
        u_t uglyness = 0;
	u_t stock = 0;
        vu_t day_breads;
	for (u_t d = 0; d < n; ++d)
	{
	    const u_t bake = a[d];
	    stock += bake;
	    u_t ugly = 0;
	    day_breads.push_back(bake);
	    u_t demand = (d < n - 1 ? k[ki] : stock);
	    for (size_t di = day_breads.size(); (di > 0) && (demand > 0);)
	    {
	        --di;
		u_t sell = min(demand, day_breads[di]);
		if (sell > 0)
		{
		    demand -= sell;
		    day_breads[di] -= sell;
		    ugly = d - di;
		}
	    }
	    size_t ioldest;
	    for (ioldest = 0; (ioldest <= d) && (day_breads[0] == 0); ++ioldest)
	    {
	    }
	    if (uglyness < ugly)
	    {
	        uglyness = ugly;
	    }
	    
	}
	solution.push_back(uglyness);
    }
}

void Bakery::solve()
{
    solve_naive();
}

void Bakery::print_solution(ostream &fo) const
{
   const char* sep = "";
   for (u_t x: solution)
   {
       fo << sep << x;
       sep = " ";
   }
   fo << '\n';
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
    unsigned n_cases = 1;
    
    void (Bakery::*psolve)() =
        (naive ? &Bakery::solve_naive : &Bakery::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Bakery bakery(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (bakery.*psolve)();
        bakery.print_solution(fout);
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
