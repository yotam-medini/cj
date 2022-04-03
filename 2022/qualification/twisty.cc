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

class Problem
{
 public:
    Problem(istream& cfi, ostream& cfo, ErrLog &el) :
        fi(cfi), fo(cfo), errlog(el) {}
    void solve_naive();
    void solve();
 private:
    bool readline_ints(vi_t &v);
    istream& fi;
    ostream& fo;
    ErrLog &errlog;
};

bool Problem::readline_ints(vi_t &v)
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

void Problem::solve_naive()
{
    u_t T;
    fi >> T;
    errlog << "T=" << T << '\n';
    for (u_t ti = 0; ti < T; ++ti)
    {
        ull_t reported_passages = 0, estimated_passages = 0;
        u_t N, K;
        u_t room;
        ull_t passages;

        fi >> N >> K;
        errlog << "N=" << N << ", K=" << K << '\n';
        fi >> room >> passages;
        errlog << "room=" << room << ", passages=" << passages << '\n';
        errlog.flush();
        reported_passages = passages;

        const u_t n_teleports = (N < K) ? N - 1 : K - 1;
        errlog << "n_teleports = " << n_teleports << '\n';
        const u_t room0 = room;
        u_t n_teleports_done = 0;
        for (room = 1; n_teleports_done < n_teleports; )
        {
            if (room != room0)
            {
                fo << "T " << room << "\n"; fo.flush();
                ++n_teleports_done;
                u_t rep_room;
                fi >> rep_room >> passages;
                errlog << "rep_room="<<rep_room << ", passages="<<passages <<
                    '\n'; errlog.flush();
                if (rep_room != room) { 
                    errlog << "Error rep_room != room\n";
                    exit(1);
                }
                reported_passages += passages;
            }
            if (N <= K)
            {
                ++room;
            }
            else
            {
                room = (((room - 1) + 130003) % N) + 1;
            }
        }
        errlog << "ti="<<ti << ", reported_passages="<<reported_passages<<'\n'; 
        if (N <= K)
        {
            estimated_passages = reported_passages / 2;
        }
        else
        {
            estimated_passages = (reported_passages * N + K) / (2*K);
        }
        errlog << "estimated_passages=" << estimated_passages << '\n';
        errlog.flush();
        fo << "E " << estimated_passages << '\n'; fo.flush();
        // int judge;  fi >> judge;
        // errlog << "judge = " << judge << '\n'; errlog.flush();
    }    
}

void Problem::solve()
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
        cerr << "pid = " << getpid() << "\n"; errlog.flush();
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    string ignore;
    Problem problem(*pfi, *pfo, errlog);
    if (naive)
    {
         problem.solve_naive();
    }
    else
    {
         problem.solve_naive();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
