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
typedef long long ll_t;
typedef unsigned long long ull_t;

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

class Blind
{
 public:
    Blind(istream& cfi, ostream& cfo, ErrLog &el) :
        fi(cfi), fo(cfo), errlog(el) {}
    void solve_naive();
    void solve();
 private:
    enum { TENP9 = 1000000000 };
    enum TResult { Center, Hit, Miss };
    TResult dthrow(ll_t x, ll_t y);
    void find_hit();
    void find_dim_center(u_t dim);
    istream& fi;
    ostream& fo;
    ErrLog &errlog;
    ll_t xyhit[2];
    ll_t center[2];
    bool found;
};

void Blind::solve_naive()
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
            found = false;
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

void Blind::solve()
{
    u_t T;
    ll_t A, B;
    fi >> T >> A >> B;
    (errlog << "T="<<T << ", A="<<A << ", B=" << B << '\n').flush();
    for (u_t ti = 0; ti < T; ++ti)
    {
        errlog << "ti="<<ti << '\n'; errlog.flush();
        found = false;
        find_hit();
        for (u_t dim = 0; (dim != 2) && !found; ++dim)
        {
            find_dim_center(dim);
        }
        errlog << "found?=" << found << '\n';
        if (!found)
        {
            errlog << "sending center ("<<
                center[0] << ", " <<  center[1] << ")\n";
            TResult tres = dthrow(center[0], center[1]);
            if (tres != Center)
            {
                (errlog << "Failed to find center: (" <<
                    center[0] << ", " << center[1] << '\n').flush();
                exit(1);
            }
        }
    }
}

void Blind::find_hit()
{
    bool hit = false;
    const ll_t ht9 = TENP9/2;
    for (int i = -1; (i <= 1) && !hit; ++i)
    {
        ll_t x = i*ht9;
        for (int j = -1; (j <= 1) && !hit; ++j)
        {
            ll_t y = j*ht9;
            errlog << "i="<<i << ", j="<<j << '\n';
            TResult r = dthrow(x, y);
            if (r != Miss)
            {
                hit = true;
                found = (r == Center);
                xyhit[0] = x;
                xyhit[1] = y;
            }
        }
    }
}

void Blind::find_dim_center(u_t dim)
{
    ll_t xy[2] = {xyhit[0], xyhit[1]};
    TResult tres;

    ll_t low, high, vmin, vmax;

    // min
    errlog << "dim:"<<dim << " search min\n";
    low = -TENP9; high = xyhit[dim];
    xy[dim] = low;
    tres = dthrow(xy[0], xy[1]);
    if (tres == Hit)
    {
        high = -TENP9;
    }
    while ((low + 1 < high) && !found)
    {
        ll_t mid = (low + high)/2;
        xy[dim] = mid;
        tres = dthrow(xy[0], xy[1]);
        if (tres == Hit)
        {
            high = mid;
        }
        else if (tres == Miss)
        {
            low = mid;
        }
        // else == Center found
    }
    vmin = high;
    errlog << "dim:"<<dim << " vmin=" << vmin << '\n';

    // max
    errlog << "dim:"<<dim << " search max\n";
    low = xyhit[dim]; high = TENP9;
    if (!found)
    {
        xy[dim] = high;
        tres = dthrow(xy[0], xy[1]);
        if (tres == Hit)
        {
            low = TENP9;
        }
    } 
    while ((low + 1 < high) && !found)
    {
        ll_t mid = (low + high)/2;
        xy[dim] = mid;
        tres = dthrow(xy[0], xy[1]);
        if (tres == Hit)
        {
            low = mid;
        }
        else if (tres == Miss)
        {
            high = mid;
        }
        // else == Center found
    }
    vmax = low;
    errlog << "dim:"<<dim << " vmax=" << vmax << '\n';

    center[dim] = (vmin + vmax)/2;
    errlog << "center[" << dim << "] = " << center[dim] << '\n';
}

Blind::TResult Blind::dthrow(ll_t x, ll_t y)
{
    TResult ret = Miss;
    (errlog << "send: x=" << x << ", y=" << y << '\n').flush();
    fo << x << ' ' << y << '\n' << flush;
    string answer;
    fi >> answer;
    (errlog << "  answer="<<answer << '\n').flush();
    if (answer == string("CENTER"))
    {
        ret = Center;
    }
    else if (answer == string("HIT"))
    {
        ret = Hit;
    }
    else if (answer == string("MISS"))
    {
        ret = Miss;
    }
    else
    {
        (errlog <<  "Bad answer: " << answer).flush();
        exit(1);
    }
    return ret;
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
    Blind problem(*pfi, *pfo, errlog);
    if (naive)
    {
         problem.solve_naive();
    }
    else
    {
         problem.solve();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
