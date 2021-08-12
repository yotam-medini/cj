// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
// #include <map>
// #include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<string> vs_t;
typedef vector<u_t> vu_t;;

static unsigned dbg_flags;

class BNode
{
 public:
    virtual ~BNode() {}
};

class INode: public BNode
{
 public:
    INode(ull_t _n) : n(_n) {}
    ull_t n;
};

class BinNode: public BNode
{
 public:
    char op;
    BNode *lr[2];
};

class BinOp
{
 public:
    BinOp(istream& fi);
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
 private:
     u_t N;
     vs_t expressions;
     vu_t solution;
};

BinOp::BinOp(istream& fi)
{
    fi >> N;
    expressions.reserve(N);
    copy_n(istream_iterator<string>(fi), N, back_inserter(expressions));
}

void BinOp::solve_naive()
{
}

void BinOp::solve()
{
     solve_naive();
}

void BinOp::print_solution(ostream &fo) const
{
    for (u_t i: solution)
    {
        fo << ' ' << (i + 1);
    }
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

    void (BinOp::*psolve)() =
        (naive ? &BinOp::solve_naive : &BinOp::solve);
    if (solve_ver == 1) { psolve = &BinOp::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BinOp binop(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (binop.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        binop.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
