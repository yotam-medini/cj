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
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

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

class Digit
{
 public:
    Digit(istream& cfi, ostream& cfo, ErrLog &el) :
        fi(cfi), fo(cfo), errlog(el) {}
    void solve_naive();
    void solve();
 private:
    bool readline_ints(vi_t &v);
    void show_blocks(const vvu_t& tblocks);
    istream& fi;
    ostream& fo;
    ErrLog &errlog;
    u_t T, N, B;
    ull_t P;
};

bool Digit::readline_ints(vi_t &v)
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

void Digit::solve_naive()
{
    fi >> T >> N >> B >> P;
    errlog << "T="<<T << ", N="<<N << ", B="<<B << ", P="<<P << '\n';
    errlog.flush();
    u_t digit, judge, ti;
    for (u_t itest = 0; itest < T; ++itest)
    {
        errlog << "itest="<<itest << '\n'; errlog.flush();
        vu_t towers(N, 0);
        vvu_t tblocks(N, vu_t());
        for (u_t di = 0; di < N*B; ++di)
        {
            fi >> digit;
            errlog << "itest="<<itest << ", di="<<di << ", digit="<<digit <<'\n';
            errlog.flush();
            int tower = -1;
            if (digit < 9)
            {
                for (ti = 0; ((ti < N) && (towers[ti]) >= B - 1); ++ti)
                {
                    ;
                }
                if (ti < N)
                {
                    tower = ti;
                }
            }
            else // == 9
            {
                for (ti = 0; ((ti < N) && (towers[ti]) != B - 1); ++ti)
                {
                    ;
                }
                if (ti < N)
                {
                    tower = ti;
                }
            }
            if (tower == -1)
            {
                u_t shortest = B + 1;
                for (ti = 0; ti < N; ++ti)
                {
                    if (shortest > towers[ti])
                    {
                        shortest = towers[ti];
                        tower = ti;
                    }
                }
            }
            ++towers[tower];
            tblocks[tower].push_back(digit);
            errlog << "di="<<di << ", tower=" << tower << '\n';  errlog.flush();
            fo << (tower + 1) << '\n'; fo.flush();
            show_blocks(tblocks);
        }
        show_blocks(tblocks);
    }
    fi >> judge;
    errlog << "judge=" << judge << '\n';
}

void Digit::solve()
{
     solve_naive();
     
}

void Digit::show_blocks(const vvu_t& tblocks)
{
    for (const vu_t& tblock: tblocks)
    {
        for (int i = tblock.size() - 1; i >= 0; --i)
        {
            errlog << tblock[i];
        }
        errlog << ' ';
    }
    errlog << '\n';
    errlog.flush();
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
    Digit digit(*pfi, *pfo, errlog);
    if (naive)
    {
         digit.solve_naive();
    }
    else
    {
         digit.solve_naive();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
