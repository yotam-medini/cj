// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
// #include <set>
// #include <map>
#include <vector>
#include <utility>

#include <cstdlib>
#include <unistd.h>
// #include <gmpxx.h>

using namespace std;

// typedef mpz_class mpzc_t;
typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
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
            write(x);
        }
        return *this;
    }
    template <class T> void write(const string& m, const T &c)
    {
        if (_f)
        {
            write(m);
            for (auto x: c)
            {
                write(' '); write(x);
            }
            write('\n');
            flush();
        }
    }
    void flush() { if (_f) { _f->flush(); } }
    bool active() const { return (_f != 0); }
 private:
    template <class T> void write(const T &x)
    {
        cerr << x;
        (*_f) << x;
    }
    ofstream *_f;
};

static const ull_t TENP9 = 1000000000;

class Blind
{
 public:
    Blind(ErrLog &el) : errlog(el) {}
    void solve_naive(istream& fi, ostream &fo);
    void solve(istream& fi, ostream &fo);
    void print_solution(ostream&) const;
 private:
    ErrLog &errlog;
};

void Blind::solve_naive(istream& fi, ostream &fo)
{
    u_t T;
    ll_t A, B;
    fi >> T >> A >> B;
    errlog << "T="<<T << ", A="<<A << ", B=" << B << '\n';
    errlog.flush();
    if ((A == B) && (A == TENP9 - 5))
    {
        for (u_t ti = 0; ti < T; ++ti)
        {
            errlog << "ti="<<ti << '\n'; errlog.flush();
            bool found = false;
            for (int x = -5; (!found) && (x <= 5); ++x)
            {
                for (int y = -5; (!found) && (y <= 5); ++y)
                {
                    fo << x << ' ' << y << '\n'; fo.flush();
                    string answer;
                    fi >> answer;
                    errlog << "x="<<x << ", y="<<y << 
                        ", answer="<<answer << '\n';
                    errlog.flush();
                    found = (answer == string("CENTER"));
                    if (answer == string("WRONG"))
                    {
                        exit(1);
                    }
                }
            }
        }
    }
    else
    {
        errlog << "Not Test Set 0\n"; errlog.flush();
        exit(1);
    }
}

void Blind::solve(istream& fi, ostream &fo)
{
    solve_naive(fi, fo);
}

void Blind::print_solution(ostream &fo) const
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
    if (dbg_flags & 0x2) 
    {
        errlog << "pid = " << getpid() << "\n"; errlog.flush();
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    string ignore;
    Blind problem(errlog);
    if (naive)
    {
         problem.solve_naive(*pfi, *pfo);
    }
    else
    {
         problem.solve_naive(*pfi, *pfo);
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
