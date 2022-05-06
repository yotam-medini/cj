// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <array>
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
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

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

#include "thresholds2.cc"

class Towers
{
 public:
    Towers(u_t _N, u_t _B, ErrLog& el) :
         N(_N), B(_B), errlog(el), tblocks(N, vu_t()),
         n_unfinished(N), m1(0), m2(0), pending(N*B)
         {}
    u_t put_get(u_t digit);
    void show() const;
    ull_t compute_score() const;
 private:
    u_t get_index_of_max_tower_lt(u_t ltval) const;
    const TeResult& get_threshold() const;
    const u_t N, B;
    ErrLog& errlog;
    vvu_t tblocks;
    u_t n_unfinished;
    u_t m1;
    u_t m2;
    u_t pending;
};

u_t Towers::put_get(u_t digit)
{
    const TeResult r = get_threshold();
    u_t tower = N;
    if (dbg_flags & 0x8) { errlog << "digit="<<digit << 
        ", unfinished="<<n_unfinished << ", m1="<<m1 << ", m2="<<m2 <<
        ", pending="<<pending << ", threshold_m1="<<r.threshold_m1 << 
        ", threshold_m2="<<r.threshold_m2 << '\n'; }
    if (digit >= r.threshold_m1)
    {
        u_t ti;
        for (ti = 0; (ti < N) && (tblocks[ti].size() != B - 1); ++ti) {;}
        if (ti < N)
        {
            tower = ti;
        }
    }
    if ((tower == N) && (digit >= r.threshold_m2))
    {
        u_t ti;
        for (ti = 0; (ti < N) && (tblocks[ti].size() != B - 2); ++ti) {;}
        if (ti < N)
        {
            tower = ti;
        }
    }
    if (tower == N)
    {
        u_t ti;
        for (ti = 0; (ti < N) && (tblocks[ti].size() >= B - 2); ++ti) {;}
        if (ti < N)
        {
            tower = ti;
        }
        else
        {
            if (dbg_flags & 0x8) { errlog << "ERROR: digit="<<digit << 
                ", unfinished="<<n_unfinished << ", m1="<<m1 << ", m2="<<m2 <<
                ", pending="<<pending << ", threshold_m1="<<r.threshold_m1 << 
                ", threshold_m2="<<r.threshold_m2 << '\n'; }
            exit(1);
        }
    }
    tblocks[tower].push_back(digit);
    if (tblocks[tower].size() == B)
    {
        --n_unfinished;
        --m1;
    }
    else if (tblocks[tower].size() == B - 1)
    {
        ++m1;
        --m2;
    }
    else if (tblocks[tower].size() == B - 2)
    {
        ++m2;
    }
    --pending;
    return tower;    
}

void Towers::show() const
{
    for (u_t ti = 0; ti < tblocks.size(); ++ti)
    {
        const vu_t& tblock = tblocks[ti];
        errlog << "[" << ti << "][" << tblock.size() << "]";
        for (int i = tblock.size() - 1; i >= 0; --i)
        {
            errlog << tblock[i];
        }
        errlog << ' ';
    }
    errlog << '\n';
    errlog.flush();
}

ull_t Towers::compute_score() const
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

u_t Towers::get_index_of_max_tower_lt(u_t ltval) const
{
    u_t ret = N;
    u_t maxh = B + 1;
    for (u_t ti = 0; ti < N; ++ti)
    {
        u_t h = tblocks[ti].size();
        if ((h < ltval) && ((maxh > B) || (maxh < h)))
        {
            maxh = h;
            ret = ti; 
        }
    }
    return ret;
}

const TeResult& Towers::get_threshold() const
{
    static ThresholdEngine engine(N, B);
    const TeState state(m1, m2, pending);
    const TeResult& r = engine.get_state_result(state);
    return r;
}

class Digit
{
 public:
    Digit(istream& cfi, ostream& cfo, ErrLog &el) :
        fi(cfi), fo(cfo), errlog(el) {}
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

void Digit::solve()
{
    fi >> T >> N >> B >> P;
    if (dbg_flags & 0x1) {
        errlog << "T="<<T << ", N="<<N << ", B="<<B << ", P="<<P << '\n'; }
    errlog.flush();
    ull_t total_score = 0;
    u_t digit;
    int judge;

    for (u_t itest = 0; itest < T; ++itest)
    {
        if (dbg_flags & 0x2) { errlog << "itest="<<itest << '\n'; }
        Towers towers(N, B, errlog);
        for (u_t di = 0; di < N*B; ++di)
        {
            fi >> digit;
            if (dbg_flags & 0x8) { errlog << "itest="<<itest << 
                ", di="<<di << ", digit="<<digit <<'\n';  errlog.flush(); }
            u_t itower = towers.put_get(digit);
            if (dbg_flags & 0x8) { errlog << "di="<<di <<
                ", itower=" << itower << '\n';  errlog.flush(); }
            fo << (itower + 1) << '\n'; fo.flush();
            if (dbg_flags & 0x4) { towers.show(); }
        }
        ull_t blocks_score = towers.compute_score();
        if (dbg_flags & 0x2) { 
            towers.show(); 
            errlog << "End-of-test: " << itest << ", score="<<blocks_score<<'\n';
        }
        total_score += blocks_score;
    }
    fi >> judge;
    if (dbg_flags & 0x1) {  errlog<<"judge="<<judge << 
        ",\n  P="<<P << ", total_score="<<total_score<< 
        ", /P = " << double(total_score)/double(P) << '\n'; 
    }
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
        errlog << '[' << tblock.size() << ']';
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

static void test_engine(u_t N, u_t B)
{
    ThresholdEngine engine(N, B);
}

int main(int argc, char ** argv)
{
    const string dash("-");

    int rc = 0, ai;
    u_t engine_N = 0, engine_B = 0;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-engine"))
        {
            engine_N = strtoul(argv[++ai], 0, 0);
            engine_B = strtoul(argv[++ai], 0, 0);
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
    cerr << "dbg_flags="<<dbg_flags << '\n';

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
    if (dbg_flags & 0x200)
    {
        cerr << "pid = " << getpid() << "\n"; errlog.flush();
        errlog << "pid = " << getpid() << "\n"; errlog.flush();
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    if (engine_N > 0)
    {
        test_engine(engine_N, engine_B);
    }
    else
    {
        Digit digit(*pfi, *pfo, errlog);
        digit.solve();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
