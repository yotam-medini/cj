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

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<u_t> vu_t;

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

class MedianSort
{
 public:
    MedianSort(istream& cfi, ostream& cfo, ErrLog &el) :
        fi(cfi), fo(cfo), errlog(el) {}
    void solve_naive();
    void solve();
 private:
    bool readline_ints(vi_t &v);
    void sol_add();
    void show_sol() const;
    istream& fi;
    ostream& fo;
    ErrLog &errlog;
    vu_t sol;
};

bool MedianSort::readline_ints(vi_t &v)
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

void MedianSort::show_sol() const
{
    errlog << "sol: {";
    for (u_t x: sol)
    {
        errlog << ' ' << x;
    }
    errlog << " }\n"; errlog.flush();
}

void MedianSort::solve_naive()
{
    u_t T, N, Q;
    fi >> T >> N >> Q;
    errlog << "T="<<T << ", N="<<N << ", Q="<<Q << '\n'; errlog.flush();
    for (u_t ti = 0; ti < T; ++ti)
    {
        errlog << "ti="<<ti << '\n';
        sol.clear();
        fo << "1 2 3\n"; fo.flush();
        int med;
        fi >> med;
        errlog << "med=" << med << '\n'; errlog.flush();
        sol.push_back(med != 1 ? 1 : 2);
        sol.push_back(med);
        sol.push_back(1 + 2 + 3 - (sol[0] + med));
        show_sol();
        while (sol.size() < N)
        {
            sol_add();
            show_sol();
        }
        const char *sep = "";
        for (u_t x: sol)
        {
            fo << sep << x; sep = " ";
        }
        fo << '\n'; fo.flush();
        int verdict;
        fi >> verdict; 
        errlog << "verdict: " << verdict << '\n';
        if (verdict != 1)
        {
            errlog << "exiting 1\n";
            exit(1);
        }
    }
}

void MedianSort::sol_add()
{
    const u_t sz = sol.size();
    const u_t next = sz + 1;
    u_t ilow = 0; u_t ihigh = sz;
    errlog << "ilow="<<ilow << ", ihigh="<<ihigh << '\n'; errlog.flush();
    while (ilow < ihigh)
    {
        u_t lh_sz = ihigh - ilow;
        u_t qi0 = ilow + lh_sz/3;
        u_t qi1 = ilow + (2*lh_sz)/3;
        if (qi0 == qi1)
        {
            errlog << "Converting qi0="<<qi0 << ", qi1="<<qi1 << " to: ";
            if (qi0 == 0)
            {
                ++qi1;
            }
            else
            {
                --qi0;
            }
            errlog << " qi0="<<qi0 << ", qi1="<<qi1 << '\n';
        }
        u_t q0 = sol[qi0];
        u_t q1 = sol[qi1];
        errlog << "qi0="<<qi0 << ", qi1="<<qi1 <<
            ", q0="<<q0 << ", q1="<<q1 << ", next="<<next << '\n';
        fo << q0 << ' ' << q1 << ' ' << next << '\n'; fo.flush();
        u_t med;
        fi >> med;
        errlog << "(" << q0 << ' ' << q1 << ' ' << next << ") med=" <<
            med << '\n';
        if (med == q0)
        {
            ihigh = qi0; // (qi0 > 0 ? qi0 - 1 : qi0);
        }
        else if (med == q1)
        {
            ilow = qi1 + 1;
        }
        else
        {
            ilow = qi0 + 1;
            ihigh = qi1;
        }
        errlog << "ilow="<<ilow << ", ihigh="<<ihigh << '\n';
        // errlog << "qi0="<<qi0 << ", qi1="<<qi1 << '\n';
    }
    vu_t newsol; newsol.reserve(sz + 1);
    newsol.insert(newsol.end(), sol.begin(), sol.begin() + ilow);
    newsol.push_back(next);
    newsol.insert(newsol.end(), sol.begin() + ilow, sol.end());
    swap(sol, newsol);
}

void MedianSort::solve()
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
    if (dbg_flags & 0x1) 
    {
        errlog << "pid = " << getpid() << "\n"; errlog.flush();
    }
    if (dbg_flags & 0x2)
    {
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    string ignore;
    MedianSort problem(*pfi, *pfo, errlog);
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
