// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>

#include <cstdlib>
#include <unistd.h>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
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

class MinSort
{
 public:
    MinSort(istream& cfi, ostream& cfo, ErrLog &el) :
        fi(cfi), fo(cfo), errlog(el) {}
    void solve_naive();
    void solve();
 private:
    bool readline_ints(vi_t &v);
    istream& fi;
    ostream& fo;
    ErrLog &errlog;
};

bool MinSort::readline_ints(vi_t &v)
{
    v.clear();
    string line;
    getline(fi, line);
    istringstream  iss(line);
    while (!iss.eof())
    {
        int x;
        iss >> x;
        if (!iss.fail())
        {
            v.push_back(x);
        }
    }
    return fi.eof();
}

void MinSort::solve_naive()
{
    vi_t v;
    readline_ints(v);
    u_t T = v[0], N = v[1];
    errlog << "T=" << T << ", N=" << N << '\n'; errlog.flush();
    for (u_t ti = 0; ti < T; ++ti)
    {
        errlog << "ti=" << ti << '\n';
        for (u_t imin = 1; imin < N; ++imin)
        {
            fo << "M " << imin << ' ' << N << '\n'; fo.flush();
            u_t pos;
            fi >> pos;
            errlog << "ti="<<ti << ", imin="<<imin << ", pos=" << pos << '\n';
            if (imin != pos)
            {
                fo << "S " << imin << ' ' << pos << '\n'; fo.flush();
                int response;
                fi >> response;
                errlog << "response=" << response << '\n';
                if (response != 1)
                {
                    errlog << "response failure\n";
                    imin = N; ti = T;
                }
            }
        }
        errlog << "Sendin Done\n";
        fo << "D\n"; fo.flush();
        u_t verdict;
        fi >> verdict;
        errlog << "verdict=" << verdict << '\n';
        if (verdict != 1)
        {
            errlog << "Failure\n";
            ti = N;
        }
    }
}

void MinSort::solve()
{
     solve_naive();
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
    if (dbg_flags & 0x100) 
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
    MinSort minSort(*pfi, *pfo, errlog);
    if (naive)
    {
        minSort.solve_naive();
    }
    else
    {
        minSort.solve();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
