// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class ABC
{
 public:
    ABC(u_t _a=0, u_t _b=0, u_t _c=0) : a(_a), b(_b), c(_c) {}
    bool legal(const ull_t n) const
    {
        u_t n1 = 0;
	for (u_t bi = a - 1; bi <= b - 1; ++bi)
	{
	    if ((n & (ull_t(1) << bi)) != 0)
	    {
	        ++n1;
	    }
	}
	bool ret = (n1 == c);
	return ret;
    }
    u_t a, b, c;
};
typedef vector<ABC> vabc_t;

class SherBit
{
 public:
    SherBit(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
    bool legal(const ull_t n) const;
    u_t N;
    u_t K;
    ull_t P;
    vabc_t constraints;
    ull_t solution;
};

SherBit::SherBit(istream& fi) : solution(0)
{
    fi >> N >> K >> P;
    constraints.reserve(K);
    for (u_t ri = 0; ri != K; ++ri)
    {
        u_t a, b, c;
	fi >> a >> b >> c;
        constraints.push_back(ABC{a, b, c});
    }
}

void SherBit::solve_naive()
{
    for (ull_t n = 0, n_legal = 0; n_legal < P; ++n)
    {
         if (legal(n))
	 {
	     ++n_legal;
	     if (n_legal == P)
	     {
	         solution = n;
	     }
	 }
    }
}

void SherBit::solve()
{
    solve_naive();
}

bool SherBit::legal(const ull_t n) const
{
    bool ret = true;
    for (vabc_t::const_iterator i = constraints.begin(), e = constraints.end();
        ret && (i != e); ++i)
    {
        const ABC& constraint = *i;
	ret = constraint.legal(n);
    }
    return ret;
}



void SherBit::print_solution(ostream &fo) const
{
    vector<char> vc;
    ull_t n = solution;
    while (n > 0)
    {
        vc.push_back("01"[n % 2]);
	n /= 2;
    }
    vc.insert(vc.end(), N - vc.size(), '0');
    fo << ' ';
    for (vector<char>::const_reverse_iterator i = vc.rbegin(), e = vc.rend();
         i != e; ++i)
    {
        fo << *i;
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

    void (SherBit::*psolve)() =
        (naive ? &SherBit::solve_naive : &SherBit::solve);
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        SherBit sherbit(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (sherbit.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        sherbit.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
