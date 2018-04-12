// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created: 2018/April/12

#include <iostream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
// #include <vector>
#include <utility>

#include <cstdlib>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
// typedef vector<ul_t> vul_t;

static unsigned dbg_flags;

class ErrLog
{
 public:
    ErrLog(const char *fn=0) : _f(fn ? new ofstream(fn) : 0) {}
    ~ErrLog() { delete _f; }
    template <class T>
    ErrLog& operator<<(const T &x)
    {
        if (_f)
        {
            cerr << x;
            (*_f) << x;
        }
        return *this;
    }
    void flush() { if (_f) { _f->flush(); } }
    bool active() const { return (_f != 0); }
 private:
    ofstream *_f;
};

class Problem
{
 public:
    Problem(istream& fi, ErrLog &el);
    void solve_naive(istream& fi, ostream &fo);
    void solve(istream& fi, ostream &fo);
    void print_solution(ostream&) const;
 private:
    ErrLog &errlog;
};

Problem::Problem(istream& fi, ErrLog &el) : errlog(el)
{
}

void Problem::solve_naive(istream& fi, ostream &fo)
{
}

void Problem::solve(istream& fi, ostream &fo)
{
}

void Problem::print_solution(ostream &fo) const
{
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
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

    ErrLog errlog(dbg_flags & 0x1 ? "/tmp/ymcj.log" : 0);
    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);
    errlog << "n_cases="<<n_cases << "\n";

    void (Problem::*psolve)(istream&, ostream&) =
        (naive ? &Problem::solve_naive : &Problem::solve);
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Problem problem(*pfi, errlog);
        (problem.*psolve)(*pfi, *pfo);
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
