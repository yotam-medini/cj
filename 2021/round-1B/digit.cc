// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <algorithm>
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
    u_t threshold = 10;
    for (u_t i = 0; (threshold == 10) && (i < cfg.rules.size()); ++i)
    {
        const Rule& rule = cfg.rules[i];
        if ((n_unfinished <= rule.unfinished) &&
            (n_almost <= rule.almost) &&
            (n_pending <= rule.pending_blocks))
        {
            threshold = rule.threshold;
        }
    }
    threshold = min<u_t>(threshold, 9);
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
    cfg_best.rules.push_back(Rule(2, 1, 6, 6));
    cfg_best.rules.push_back(Rule(3, 1, 15, 7));
    cfg_best.rules.push_back(Rule(4, 1, 17, 8));
    cfg_best.low_to_low = 3;

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

int main(int argc, char ** argv)
{
    const string dash("-");

    bool learn = false;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-learn"))
        {
            learn = true;
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
    else
    {
        Digit digit(*pfi, *pfo, errlog);
        digit.solve();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
