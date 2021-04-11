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
#include <cstdio>
#include <cstring>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

class Asort
{
 public:
    Asort(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    u_t N;
    vs_t x;
    u_t solution;
};

Asort::Asort(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<string>(fi), N, back_inserter(x));
}

void Asort::solve_naive()
{
    vs_t sx(x);
    for (u_t i = 1; i < N; ++i)
    {
        const string& psx = sx[i - 1];
        string& csx = sx[i];
        const u_t sz0 = psx.size();
        const u_t sz1 = csx.size();
        if (dbg_flags & 0x8) { cerr << "sz0="<<sz0 << ", sz1="<<sz1 << '\n'; }
        if ((sz0 > sz1) || ((sz0 == sz1) && (psx >= csx)))
        {
	    if (sz0 == sz1)
	    {
	        csx.push_back('0');
		++solution;
	    }
	    else
	    {
	        const string head(psx, 0, sz1);
		if (dbg_flags & 0x2) { cerr << "head=" << head << '\n'; }
		if (head != csx)
		{
		    u_t nz = sz0 - sz1 + (head < csx ? 0 : 1);
		    csx += string(nz, '0');
		    solution += nz;
		}
		else
		{
		    bool all9 = true;
		    for (u_t ci = sz1; all9 && (ci < sz0); ++ci)
		    {
			all9 = psx[ci] == '9';
		    }
		    if (dbg_flags & 0x4) { cerr << "all9?="<< all9 << '\n'; }
		    if (all9)
		    {
			size_t nz = sz0 + 1 - sz1;
			csx += string(nz, '0');
			solution += nz;
		    }
		    else
		    {
			string tail(psx, sz1);
			if (dbg_flags & 0x4) { cerr << "tail?="<<tail << '\n'; }
			size_t j = tail.size() - 1;
			while (tail[j] == '9')
			{
			    tail[j] = '0';
			    --j;
			}
			tail[j] += 1;
			csx += tail;
			solution += tail.size();
		    }
		}
	    }
	}
    }
    if (dbg_flags & 0x1) 
    {
        for (const string& y: x) { cerr << ' ' << y; } cerr << '\n';
        for (const string& y: sx) { cerr << ' ' << y; } cerr << '\n';
    }
}

void Asort::solve()
{
    solve_naive();
}

void Asort::print_solution(ostream &fo) const
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

    void (Asort::*psolve)() =
        (naive ? &Asort::solve_naive : &Asort::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Asort asort(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (asort.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        asort.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
