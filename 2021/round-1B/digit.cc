// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <algorithm>
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
    void show_blocks(const vvu_t& tblocks) const;
    ull_t compute_block_score(const vvu_t& tblocks) const;
    u_t get_index_of_max_tower_lt(const vu_t& towers, u_t ltval) const;
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
    ull_t total_score = 0;
    u_t digit, ti;
    int judge;
    for (u_t itest = 0; itest < T; ++itest)
    {
        errlog << "itest="<<itest << '\n'; errlog.flush();
        vu_t towers(N, 0);
        const vu_t::const_iterator tbegin = towers.begin(), tend = towers.end();
        vvu_t tblocks(N, vu_t());
        u_t end_tower_threshold = 9;
        for (u_t di = 0; di < N*B; ++di)
        {
            fi >> digit;
            if (dbg_flags & 0x8) { errlog << "itest="<<itest << 
                ", di="<<di << ", digit="<<digit <<'\n';  errlog.flush(); }
            int tower = -1;
            if (towers[N - 2] == B - 1)
            {
                end_tower_threshold = 8;
            }
            if (digit >= end_tower_threshold)
            {
                for (ti = 0; ((ti < N) && (towers[ti]) != B - 1); ++ti)
                {
                    ;
                }
                if (ti < N)
                {
                    tower = ti;
                }
                else
                {
                    tower = get_index_of_max_tower_lt(towers, B);
                }
            }
            else
            {
                if (false && (digit < 1))
                {
                    tower = min_element(tbegin, tend) - tbegin;
                }
                else
                {
                    tower = get_index_of_max_tower_lt(towers, B - 1);
                    if (tower == int(N))
                    {
                        tower = get_index_of_max_tower_lt(towers, B);
                    }
                }
            }
            ++towers[tower];
            tblocks[tower].push_back(digit);
            if (dbg_flags & 0x8) { errlog << "di="<<di <<
                ", tower=" << tower << '\n';  errlog.flush(); }
            fo << (tower + 1) << '\n'; fo.flush();
            if (dbg_flags & 0x4) { show_blocks(tblocks); }
        }
        show_blocks(tblocks);
        ull_t blocks_score = compute_block_score(tblocks);
        errlog << "End-of-test: " << itest << ", score="<<blocks_score << '\n';
        total_score += blocks_score;
    }
    fi >> judge;
    errlog<<"judge="<<judge << ", P="<<P << ", total_score="<<total_score<< '\n';
}

void Digit::solve()
{
     solve_naive();
     
}

u_t Digit::get_index_of_max_tower_lt(const vu_t& towers, u_t ltval) const
{
    u_t ret = N;
    u_t maxt = B + 1;
    for (u_t ti = 0; ti < N; ++ti)
    {
        u_t tval = towers[ti];
        if ((tval < ltval) && ((maxt > B) || (maxt < tval)))
        {
            maxt = tval;
            ret = ti; 
        }
    }
    return ret;
}

void Digit::show_blocks(const vvu_t& tblocks) const
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

ull_t Digit::compute_block_score(const vvu_t& tblocks) const
{
    ull_t score = 0;
    for (const vu_t& tblock: tblocks)
    {
        ull_t tenp = 1;
        ull_t tower_number = 0;
        for (u_t d: tblock)
        {
            tower_number += tenp * ull_t(d);
            tenp *= 10;
        }
        score += tower_number;
    }
    return score;
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
