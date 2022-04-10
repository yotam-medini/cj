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

class Rule
{
 public:
    Rule(u_t unf=0, u_t alm=0, u_t pend=0, u_t thres=0) : 
        unfinished(unf),
        almost(alm),
        pending_blocks(pend),
        threshold(thres)
        {}
    string str() const
    {
        ostringstream os;
        os << "Rule(" << unfinished << ", " << almost << ", " <<
            pending_blocks << ", " << threshold << ")";
        return os.str();
    }
    u_t unfinished; // #(towers) < B
    u_t almost;     // #(towers) == B-1
    u_t pending_blocks; 
    u_t threshold;
};
typedef vector<Rule> vrule_t;

class Config
{
 public:
    Config(): low_to_low(0) {}
    vrule_t rules;
    u_t low_to_low;
    void print(ostream& os) const
    {
        os << "Config cfg;\n";
        for (const Rule& rule: rules)
        {
            os << "cfg.rules.push_back(" << rule.str() << ");\n";
        }
        os << "cfg.low_to_low = " << low_to_low << "\n";
    }
};

class ThresholdEngine
{
 public:
    ThresholdEngine(u_t _N, u_t _B) : N(_N), B(_B), 
        ad_threshold{N, vu_t{N*B, 10}}
    {
    }
    void build();
    u_t get_threshold(u_t a, u_t d) const
    { 
        u_t ret = ad_threshold[a][d]; 
        if ((ret != 9) && (a > 0) && (dbg_flags & 0x2)) { cerr << __func__ <<
            ": a=" << a << ", d=" << setw(2) << d <<
            ", threshold=" << ret << '\n'; }
        return ret;
    }
 private:
    // a = "almost" filled towers (B-1)
    // d = penDing blocks. (a <= d)
    // u = unfinished non-almost towers. (a + u <= N).
    u_t calc_u(u_t a, u_t d) const;
    const u_t N, B;
    vvu_t ad_threshold;
};

void ThresholdEngine::build()
{
    typedef vector<double> vd_t;
    typedef vector<vd_t> vvd_t;
    const u_t NB = N*B;
    vvd_t tail_expectation{N + 1, vd_t(NB + 1, -1.)};
    ad_threshold = vvu_t(size_t(N + 1), vu_t(NB + 1, 10));
    for (u_t a = 0; a <= N; ++a)
    {
        ad_threshold[a][0] = 0;
        tail_expectation[a][0] = 0.;
        for (u_t d = 0; d < a; ++d) // impossible
        {
            ad_threshold[a][0] = 12;
            tail_expectation[a][0] = -2.;
        }
        ad_threshold[a][a] = 0;
        tail_expectation[a][a] = a * ((0. + 9.)/2.);
        for (u_t d = a + 1; d <= NB; d += B) // impossible
        {
            ad_threshold[a][0] = 13;
            tail_expectation[a][0] = -3;
        }
    }
    for (u_t d = 1; d <= NB; ++d)
    {
        // for (u_t a = 0; a <= min(d, N); ++a)
        for (u_t a = 0; (a < d) && ((a + ((d - a) + (B - 1))/B) <= N); ++a)
        {
            const u_t non_a_height = (B - (d - a) % B) % B; // < B - 1
            if (a == 0)
            {
                if (d == 0)
                {
                    ad_threshold[0][d] = 0.;
                    tail_expectation[0][d] = 0.;
                }
                else if (d < B)
                {
                    ad_threshold[0][d] = 0.;
                    tail_expectation[0][d] = (0. + 9.)/2;
                }
                else if (non_a_height < B - 2)
                {
                    ad_threshold[0][d] = ad_threshold[0][d - 1];
                    tail_expectation[a][d] = tail_expectation[a][d - 1];
                }
                else // non_a_height == B - 2
                {
                    ad_threshold[0][d] = ad_threshold[1][d - 1];
                    tail_expectation[a][d] = tail_expectation[1][d - 1];
                }
            }
            else if (non_a_height != B - 1) // a > 0
            {
                double best_expect = 0;
                u_t best_threshold = 10; // undef
                for (u_t t = 0; t <= 9; ++t)
                {
                    double p_fill = (10. - t) / 10.;
                    double avg = (t + 9.) / 2.;
                    double xp_fill = avg + tail_expectation[a - 1][d - 1];
                    u_t a_next = a + (non_a_height == B - 2 ? 1 : 0);
                    double xp_next = tail_expectation[a_next][d - 1];
                    if (xp_fill < 0. || xp_next < 0.) {
                        cerr << "Bug @ " << __func__ << ':' << __LINE__ << '\n';
                    }
                    double expect = p_fill * xp_fill + (1. - p_fill) * xp_next;
                    if (best_expect < expect)
                    {
                        best_expect = expect;
                        best_threshold = t;
                    }
                }
                ad_threshold[a][d] = best_threshold;
                tail_expectation[a][d] = best_expect;
            }
        }
    }
    if (dbg_flags & 0x1)
    {
        const u_t N_maxshow = min<u_t>(20, N);
        for (u_t a = 0; a <= N_maxshow; ++a)
        {
            cerr << "{ Almost == " << a << ":\n";
            const vu_t& ad_ta = ad_threshold[a];
            const vd_t& texpa = tail_expectation[a];
            for (u_t d = 0; d <= N_maxshow * B; ++d)
            {
                cerr << "["<<a<<"]["<< setw(2) << d<<"] T=" << 
                    ad_ta[d] << ", tE=" << texpa[d] << '\n';
            }
            cerr << "}\n";
        }
    }
}

u_t ThresholdEngine::calc_u(u_t a, u_t d) const
{
    int u = 0;
    if (d > a)
    {
        u = ((d - a) + (B + 1)) / B;
    }
    return u;
}

class Towers
{
 public:
    Towers(u_t _N, u_t _B, ErrLog& el, const Config& _cfg=Config()) :
         N(_N), B(_B), errlog(el), cfg(_cfg), tblocks(N, vu_t()),
         n_unfinished(N), n_almost(0), n_pending(N*B)
         {}
    u_t put_get(u_t digit);
    void show() const;
    ull_t compute_score() const;
 private:
    u_t get_index_of_max_tower_lt(u_t ltval) const;
    u_t get_threshold() const;
    const u_t N, B;
    ErrLog& errlog;
    Config cfg;
    vvu_t tblocks;
    u_t n_unfinished;
    u_t n_almost;
    u_t n_pending;
};

u_t Towers::put_get(u_t digit)
{
    u_t threshold = get_threshold();
    u_t tower = N;
    if (dbg_flags & 0x8) { errlog << "digit="<<digit << 
        ", unfinished="<<n_unfinished << ", almost="<<n_almost <<
        ", n_pending="<<n_pending << ", threshold="<<threshold << '\n'; }
    if (digit >= threshold)
    {
        u_t ti;
        for (ti = 0; (ti < N) && (tblocks[ti].size() != B - 1); ++ti)
        {
            ;
        }
        if (ti < N)
        {
            tower = ti;
        }
        else
        {
            tower = get_index_of_max_tower_lt(B);
        }
    }
    else
    {
        tower = get_index_of_max_tower_lt(B - 1);
        if (tower == N)
        {
            tower = get_index_of_max_tower_lt(B);
        }
        else if (digit < cfg.low_to_low)
        {
            cerr << "BUG! @" << __LINE__ << '\n'; exit(1);
            u_t h = tblocks[tower].size();
            int alt_tower = get_index_of_max_tower_lt(h);
            if (alt_tower < int(N))
            {
                tower = alt_tower;
            }
        }
    }
    tblocks[tower].push_back(digit);
    if (tblocks[tower].size() == B)
    {
        --n_unfinished;
        --n_almost;
    }
    else if (tblocks[tower].size() == B - 1)
    {
        ++n_almost;
    }
    --n_pending;
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

u_t Towers::get_threshold() const
{
    static ThresholdEngine engine(N, B);
    static bool built = false;
    if (!built)
    {
        engine.build();
        built = true;
    }
    u_t threshold = engine.get_threshold(n_almost, n_pending);
    return threshold;
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

    Config cfg_best; // see do_learn below
    cfg_best.rules.push_back(Rule(2, 1, 4, 6));
    cfg_best.rules.push_back(Rule(2, 1, 5, 7));
    cfg_best.rules.push_back(Rule(3, 2, 16, 8));
    cfg_best.low_to_low = 0;

    for (u_t itest = 0; itest < T; ++itest)
    {
        if (dbg_flags & 0x2) { errlog << "itest="<<itest << '\n'; }
        Towers towers(N, B, errlog, cfg_best);
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

class Learn
{
 public:
    enum {T = 50, N= 20, B=15};
    Learn(ErrLog& el) : errlog(el), best_score(0) {}
    void run();
 private:
    void test_current_config();
    ErrLog& errlog;
    Config cfg_current;
    Config cfg_best;
    ull_t best_score;
    static const ull_t P1, P2;
    void iterate();
};
const ull_t Learn::P1 = 860939810732536850;
const ull_t Learn::P2 = 937467793908762347;

void Learn::run()
{
    test_current_config();
    for (u_t threshold = 8; threshold >= 5; --threshold)
    {
        for (u_t low_to_low: {0, 1, 2, 3})
        {
            cfg_current.low_to_low = low_to_low;
            cfg_current.rules.clear();
            cfg_current.rules.push_back(Rule(2, 1, 3, threshold));
            iterate();
        }
    }
    cout << "best_score = " << best_score << ", /P2 = " <<
        double(best_score)/double(P2) << " with:\n";
    cfg_best.print(cout);
    
}

void Learn::test_current_config()
{
    cerr << __func__ << '\n';
    ull_t min_total_score = P2;
    for (u_t sample = 0; sample < 3; ++sample)
    {
        ull_t total_score = 0;
        for (u_t t = 0; t < T; ++t)
        {
            Towers towers(N, B, errlog, cfg_current);
            for (u_t b = 0; b < N*B; ++b)
            {
                u_t digit = rand() % 10;
                towers.put_get(digit); // ignore returned tower index;
            }
            ull_t tscore = towers.compute_score();
            total_score += tscore;
        }
        min_total_score = min<ull_t>(min_total_score, total_score);
    }
    ull_t total_score = min_total_score;
    cout << "total_score = " << total_score << 
        ", /P1 = " << double(total_score)/double(P1) <<
        ", /P2 = " << double(total_score)/double(P2) << '\n';
    if (best_score < total_score)
    {
        best_score = total_score;
        cfg_best = cfg_current;
    }
}

void Learn::iterate()
{
    vrule_t& rules = cfg_current.rules;
    u_t i = rules.size() - 1;
    for ( ; rules[i].almost <= rules[i].unfinished; ++rules[i].almost)
    {
        for ( ; rules[i].pending_blocks <= 2*B; ++rules[i].pending_blocks)
        {
            if (rules[i].threshold == 8)
            {
                test_current_config();
            }
            else
            {
                Rule next_rule(rules[i]);
                ++next_rule.unfinished;
                ++next_rule.threshold;
                rules.push_back(next_rule);
                iterate();
                rules.pop_back();
            }
        }
    }
}

static void do_learn(ErrLog& el)
{
    Learn learn(el);
    learn.run();
}

static void test_engine(u_t N, u_t B)
{
    ThresholdEngine engine(N, B);
    engine.build();
}

int main(int argc, char ** argv)
{
    const string dash("-");

    bool learn = false;
    int rc = 0, ai;
    u_t engine_N = 0, engine_B = 0;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-learn"))
        {
            learn = true;
        }
        else if (opt == string("-engine"))
        {
            engine_N = strtoul(argv[++ai], 0, 0);
            engine_B = strtoul(argv[++ai], 0, 0);
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
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
    if (dbg_flags & 0x200)
    {
        errlog << "pid = " << getpid() << "\n"; errlog.flush();
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    if (learn)
    {
        do_learn(errlog);
    }
    else if (engine_N > 0)
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
