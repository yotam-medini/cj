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

class TeState
{
 public:
    TeState(u_t _m1=0, u_t _m2=0, u_t _pending=0) :
        m1(_m1), m2(_m2), pending(_pending) {}
    void fill_m1()
    {
        --m1;
        --pending;
    }
    void fill_m2()
    {
        ++m1;
        --m2;
        --pending;
    }
    u_t m1, m2, pending;
};

class TeResult
{
 public:
    TeResult(u_t tm1=0, u_t tm2=0, double e=0.) :
        threshold_m1(tm1), threshold_m2(tm2), expect(e) {}
    u_t threshold_m1;
    u_t threshold_m2;
    double expect;
}; 
typedef vector<TeResult> vthr_t;
typedef vector<vthr_t> vvthr_t;

class ThresholdEngine
{
 public:
    // enum {N = 20, B = 15, NB = N*B};
    ThresholdEngine(u_t _N=20, u_t _B=15) :
        N(_N), B(_B), NB(_N*_B),
        pm1m2(NB + 1, vvthr_t())
    { 
        build(); 
    }
    const TeResult& get_threshold(u_t pending, u_t m1, u_t m2) const
    {
        return pm1m2[pending][m1][m2];
    }
    const TeResult& get_state_result(const TeState s) const
    {
        const TeResult& r =  pm1m2[s.pending][s.m1][s.m2];
        return r;
    }
    int report(
        ostream& os=cerr, 
        u_t m1_max=0, u_t m2_max=0, u_t pending_max=0) const;
    int report_default(ostream& os=cerr) const
    {
        return report(os, N, B, NB);
    }
 private:
    void build();
    bool state_is_possible(const TeState& state) const;
    void get_next_state(TeState& next, const TeState& state) const;
    void ti_must_fill(TeResult& ti, const TeState& state);
    void ti_may_fill(TeResult& ti, const TeState& state);
    void ti_m1(TeResult& ti, const TeState& state);
    void ti_m2(TeResult& ti, const TeState& state);

    // vvthr_t pm1m2[NB + 1]; // indexed [pending][#m1][#m2]
    const u_t N;
    const u_t B;
    const u_t NB;
    vector<vvthr_t> pm1m2; // indexed [pending][#m1][#m2]
};

void ThresholdEngine::build()
{
    for (u_t pending = 0; pending <= NB; ++pending)
    {
        const u_t m1_max = min<u_t>(N, pending);
        vvthr_t& m1_infos = pm1m2[pending];
        for (u_t m1 = 0; m1 <= m1_max; ++m1)
        {
            const u_t m2_max = min<u_t>(N - m1, pending - m1);
            vthr_t m2_infos; m2_infos.reserve(m2_max + 1);
            for (u_t m2 = 0; m2 <= m2_max; ++m2)
            {
                const TeState state{m1, m2, pending};
                // default values for (pending == m1 + 2*m2)
                TeResult ti(0, 0, (10*m1 + m2) * (0. + 9.)/2); 
                if (!state_is_possible(state))
                {
                     ti = TeResult(); // impossible state
                }
                else if (pending == m1 + 2*m2)
                {
                    if ((m1 > 0) && (m2 > 0))
                    {
                        ti_must_fill(ti, state);
                    }
                }
                else // (pending > m1 + m2)
                {
                    if ((m1 > 0) && (m2 > 0))
                    {
                        ti_may_fill(ti, state);
                    }
                    else if (m1 > 0)
                    {
                        ti_m1(ti, state);
                    }
                    else if (m2 > 0)
                    {
                        ti_m2(ti, state);
                    }
                }
                m2_infos.push_back(ti);
            }
            m1_infos.push_back(m2_infos);
        }
    }
}

bool ThresholdEngine::state_is_possible(const TeState& s) const
{
    bool possible = (s.m1 + 2*s.m2 <= s.pending) &&
        (s.pending && s.pending <= NB);
    if (possible)
    {
        u_t next_non_m1m2_lack = (s.pending - (s.m1 + 2*s.m2)) % B;
        u_t next_non_m1m2_height = (B - next_non_m1m2_lack) % B;
        possible = next_non_m1m2_height < B - 2;
    }
    return possible;
}

void ThresholdEngine::get_next_state(TeState& next, const TeState& state) const
{
    next = state;
    --next.pending;
    u_t next_non_m1m2_lack = (next.pending - (next.m1 + 2*next.m2)) % B;
    u_t next_non_m1m2_height = (B - next_non_m1m2_lack) % B;
    if (next_non_m1m2_height == B - 2)
    {
        ++next.m2;
    }
}

void ThresholdEngine::ti_must_fill(TeResult& ti, const TeState& state)
{
    double expect_max = 0.;
    TeState state_m1(state); --state_m1.m1;
    TeState state_m2(state); --state_m2.m2;
    for (u_t d1 = 1; d1 <= 9; ++d1)
    {
        const double p1_fill = (10 - d1)/10.;
        const double v1_fill = 10*(d1 + 9.)/2;
        const double e1_fill = p1_fill * v1_fill;
        const u_t d2 = 0;
        {
            const double p2_fill = 1. - p1_fill;
            const double v2_fill = (d2 + (d1 - 1.))/10.;
            const double e2_fill = p2_fill * v2_fill;
            const double e_d1d2 = e1_fill + e2_fill;
            if (expect_max < e_d1d2)
            {
                expect_max = e_d1d2;
                ti = TeResult(d1, d2, e_d1d2);
            }
        }
    }
}

void ThresholdEngine::ti_may_fill(TeResult& ti, const TeState& state)
{
    double expect_max = 0.;
    TeState state_m1(state);
    state_m1.fill_m1();
    const double e_state_m1 = get_state_result(state_m1).expect;
    TeState state_m2(state);
    state_m2.fill_m2();
    const double e_state_m2 = get_state_result(state_m2).expect;
    TeState state_next;
    get_next_state(state_next, state);
    const double vnext = get_state_result(state_next).expect;
    for (u_t d1 = 1; d1 <= 9; ++d1)
    {
        const double p1_fill = (10 - d1)/10.;
        const double v1_fill = 10*(d1 + 9.)/2 + e_state_m1;
        const double e1_fill = p1_fill * v1_fill;
        for (u_t d2 = 0; d2 < d1; ++d2)
        {
            const double p2_fill = double(d1 - d2)/10.;
            const double v2_fill = (d2 + (d1 - 1.))/2. + e_state_m2;
            const double e2_fill = p2_fill * v2_fill;
            const double p_next = 1. - (p1_fill + p2_fill);
            const double e_next = p_next * vnext;
            const double e_d1d2 = e1_fill + e2_fill + e_next;
            if (expect_max < e_d1d2)
            {
                expect_max = e_d1d2;
                ti = TeResult(d1, d2, e_d1d2);
            }
        }
    }
}

void ThresholdEngine::ti_m1(TeResult& ti, const TeState& state)
{
    double expect_max = 0.;
    TeState state_m1(state);
    state_m1.fill_m1();
    const double e_state_m1 = get_state_result(state_m1).expect;
    TeState state_next;
    get_next_state(state_next, state);
    const double vnext = get_state_result(state_next).expect;
    for (u_t d1 = 0; d1 <= 9; ++d1)
    {
        const double p1_fill = (10 - d1)/10.;
        const double v1_fill = 10*(d1 + 9.)/2 + e_state_m1;
        const double e1_fill = p1_fill * v1_fill;
        {
            const double p_next = 1. - p1_fill;
            const double e_next = p_next * vnext;
            const double e_d1 = e1_fill + e_next;
            if (expect_max < e_d1)
            {
                expect_max = e_d1;
                ti = TeResult(d1, 0, e_d1);
            }
        }
    }
}

void ThresholdEngine::ti_m2(TeResult& ti, const TeState& state)
{
    double expect_max = 0.;
    TeState state_m2(state);
    state_m2.fill_m2();
    double e_state_m2 = get_state_result(state_m2).expect;
    TeState state_next;
    get_next_state(state_next, state);
    const double vnext = get_state_result(state_next).expect;
    {
        for (u_t d2 = 0; d2 <= 9; ++d2)
        {
            const double p2_fill = double(10 - d2)/10.;
            const double v2_fill = (d2 + 9.)/2. + e_state_m2;
            const double e2_fill = p2_fill * v2_fill;
            const double p_next = 1. - p2_fill;
            const double e_next = p_next * vnext;
            const double e_d2 = e2_fill + e_next;
            if (expect_max < e_d2)
            {
                expect_max = e_d2;
                ti = TeResult(0, d2, e_d2);
            }
        }
    }
}

int ThresholdEngine::report(
   ostream& os,
   u_t c_m1_max, 
   u_t c_m2_max,
   u_t c_pending_max) const
{
    for (u_t m1 = 0; m1 <= c_m1_max; ++m1)
    {
        const u_t m2_max = min((N - m1), c_m2_max);
        for (u_t m2 = 0; m2 <= m2_max; ++m2)
        {
            u_t pending_max = min(NB - (m1 + m2)*B, c_pending_max);
            for (u_t pending = m1 + 2*m2; pending <= pending_max; ++pending)
            {
                const TeResult& ti = get_threshold(pending, m1, m2);
                cout << "[" << setw(2) << m1 << "][" << setw(2) << m2 << "][" <<
                    setw(3) << pending << "] T1=" << ti.threshold_m1 <<
                    ", T2=" << ti.threshold_m2 << ", E=" << ti.expect << "\n";
            }
        }
    }
    return 0;
}

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
