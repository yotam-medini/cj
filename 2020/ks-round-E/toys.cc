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
typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Toy
{
 public:
    Toy(u_t _e=0, u_t _r=0) : e(_e), r(_r) {}
    u_t e, r;
};
typedef vector<Toy> vtoy_t;

static const u_t infi(-1);

class Toys
{
 public:
    Toys(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t time(const vtoy_t& subtoys) const;
    u_t n;
    vtoy_t toys;
    u_t y, z;
};

Toys::Toys(istream& fi) : y(0), z(0)
{
    fi >> n;
    toys.reserve(n);
    for (u_t i = 0; i < n; ++i)
    {
        u_t e, r;
	fi >> e >> r;
	toys.push_back(Toy(e, r));
    }
}

void Toys::solve_naive()
{
    y = 0;
    z = time(toys);
    for (u_t r = 1; (r < n) && (z != infi); ++r)
    {
        for (u_t mask = 0; (mask < (1u << n)) && (z != infi); ++mask)
	{
            vtoy_t subtoys;
	    u_t n_bits = 0;
	    for (u_t bi = 0; bi < n; ++bi)
	    {
	        if (mask & (1u << bi))
		{
		    ++n_bits;
		}
		else
		{
		    subtoys.push_back(toys[bi]);
		}
	    }
	    if (n_bits == r)
	    {
	        u_t t = time(subtoys);
		if ((t == infi) || (z < t))
		{
		    y = n_bits;
		    z = t;		    
		}
	    }
	}
    }
}

u_t Toys::time(const vtoy_t& subtoys) const
{
    u_t t = infi;
    u_t etotal = 0;
    for (const Toy& toy: subtoys)
    {
        etotal += toy.e;
    }
    bool will_cry = false;
    for (const Toy& toy: subtoys)
    {
        will_cry = will_cry || (etotal - toy.e < toy.r);
    }
    if (will_cry)
    {
        // simulater
	t = 0;
	bool cry = false;
	vu_t tleft(subtoys.size(), 0);
	u_t i = 0;
	while (!cry)
	{
	    const Toy& toy = subtoys[i];
            cry = (tleft[i] > 0) && (t - tleft[i] < toy.r);
	    if (!cry)
	    {
	        t += toy.e;
		tleft[i] = t;
	    }
	    i = (i + 1) % (subtoys.size());
	}
    }
    return t;
}

void Toys::solve()
{
    solve_naive();
}

void Toys::print_solution(ostream &fo) const
{
    fo << ' ' << y << ' ';
    if (z == infi)
    {
        fo << "INDEFINITELY";
    }
    else
    {
        fo << z;
    }
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

    void (Toys::*psolve)() =
        (naive ? &Toys::solve_naive : &Toys::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Toys toys(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (toys.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        toys.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
