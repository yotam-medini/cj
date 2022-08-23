// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com -- Created:

#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstdlib>
#include <unistd.h>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<int> vi_t;
typedef vector<ull_t> vull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<vvu_t> vvvu_t;
typedef array<u_t,2 > au2_t;
typedef vector<au2_t> vau2_t;
typedef unordered_set<u_t> uosetu_t;

static unsigned dbg_flags;

static u_t nbits_on(ull_t n)
{
    u_t nb = 0;
    while (n)
    {
        if (n & 0x1)
        {
            ++nb;
        }
        n >>= 1;
    }
    return nb;
}

static string strbase(ull_t n, const u_t base)
{
    string s;
    while (n)
    {
        char digit = "0123456789abcdef"[n % base];
        s.push_back(digit);
        n /= base;
    }
    reverse(s.begin(), s.end());
    if (s.empty()) { s = string("0"); }
    return s;
}
// static string strhex(u_t n) { return strbase(n, 0x10); }
static string strbin(ull_t n, u_t width=8)
{
    string s = strbase(n, 2);
    u_t sz = s.size();
    if (sz < width)
    {
        s = string(width - sz, '0') + s;
    }
    return s;
}

u_t vu_find(const vu_t& a, u_t x)
{
    u_t i = find(a.begin(), a.end(), x) - a.begin();
    return i;
}

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

#if 0
class SubSet
{
 public:
    SubSet(u_t _nb=0, u_t idxs=0) : nb(_nb), values_idx(idxs) {}
    u_t nb;
    u_t values_idx;
};
typedef vector<SubSet> vsubset_t;
#endif

typedef array<u_t, 9> au9_t; // [nb] -> bit-combination
typedef vector<au9_t> vau9_t;

class State
{
 public:
    State() : nb(9), subset(0), distance(0x100), best_move(0x100), color(0) {}
    u_t nb;
    u_t subset; // bits combination into FactBits.nb_canon8_values
    vau9_t out_edges; // [canon8_values] 
    // vau9_t in_edges; // 
    u_t distance;
    u_t best_move;
    u_t color;
};
typedef vector<State> vstate_t;

typedef array<ull_t, 9> aull9_t;
class FactBits
{
 public:
    FactBits() {}
    void init() { if (canon8_values.empty()) { _init(); } }
    ull_t set_play(aull9_t& nb_sets, ull_t preset, u_t b8) const;
 private:
    void _init();
    void init_states();
    void compute_play_result_values();
    void winning_sets();
    u_t ivalue(u_t x) const { return vu_find(canon8_values, x); }
    u_t nb_ivalue(u_t nb, u_t x) const
    {
        return vu_find(nb_canon8_values[nb], x);
    }
    void print_states_graph(ostream& os=cerr) const;
    void solve_minmax();
    void state_solve_minmax(u_t nb, u_t si, u_t depth=0);
    void compute_circles();
    ull_t nbsi_key(ull_t nb, ull_t si) const
    {
        return (nb << 16) | si;
    }
    u_t canon8[0x100];
    vu_t canon8_values;
    vu_t nb_canon8_values[9];
    ull_t nb_canon8_sets[9];

    // [i][j] -> possible canon8_values,
    // where i, j are indices to canon8_values
    vvvu_t play_result_values; 

    vstate_t nb_states[9];
};

void FactBits::_init()
{
    for (u_t n = 0; n < 0x100; ++n)
    {
        u_t canon = n;
        for (u_t x = n, rot = 1; rot < 8; ++rot)
        {
            x = ((x >> 1) | ((x & 1) << 7)) & 0xff;
            if (canon > x)
            {
                canon = x;
            }
        }
        canon8[n] = canon;
        canon8_values.push_back(canon);
    }
    sort(canon8_values.begin(), canon8_values.end());

    vector<u_t>::iterator trim =
        unique(canon8_values.begin(), canon8_values.end());
    canon8_values.erase(trim, canon8_values.end());
    const u_t nv = canon8_values.size();

    for (u_t x: canon8_values)
    {
        u_t nb = nbits_on(x);
        nb_canon8_values[nb].push_back(x);
    }

    if (dbg_flags & 0x2) { cerr << "#(canon8_values)=" << nv << '\n'; }
    if (dbg_flags & 0x4) {
        cerr << "(canon8_values)[" << canon8_values.size() << "]:\n";
        for (u_t vi = 0; vi < canon8_values.size(); ++vi) { cerr <<
            " [" << setw(2)<<vi << "] = " << strbin(canon8_values[vi]) << '\n';} 
        for (u_t nb = 0; nb <= 8; ++nb)
        {
            const vu_t& nvc8v = nb_canon8_values[nb];
            cerr << "NB=" << nb << " [" << nvc8v.size() << "]:\n";
            for (u_t x: nvc8v)
            {
                cerr << "  " << strbin(x) << '\n';
            }
        }
    }
    for (u_t i = 0; i <= 8; ++i)
    {
        nb_canon8_sets[i] = 0;
        for (u_t v: nb_canon8_values[i])
        {
            nb_canon8_sets[i] |= (ull_t(1) << ivalue(v));
        }
        if (dbg_flags & 0x2) { cerr << "nb_canon8_sets["<<i<<"] = " <<
            strbin(nb_canon8_sets[i], nv) << '\n'; }
    }
    // winning_sets();
    compute_play_result_values();
    init_states();
    // solve_minmax();
    compute_circles();
}

void FactBits::compute_play_result_values()
{
    const u_t nv = canon8_values.size();
    play_result_values = vvvu_t(nv, vvu_t(nv, vu_t()));
    for (u_t i = 0; i < nv; ++i)
    {
        const u_t x = canon8_values[i];
        for (u_t j = i; j < nv; ++j)
        {
            const u_t y = canon8_values[j];
            uosetu_t rvalues;
            for (u_t yrot = y, rot = 0; rot < 8; ++rot)
            {
                u_t r = canon8[x ^ yrot];
                rvalues.insert(rvalues.end(), r);
                yrot = (yrot >> 1) | ((yrot & 0x1) << 7);
            }
            vu_t srvalues(rvalues.begin(), rvalues.end());
            sort(srvalues.begin(), srvalues.end());
            play_result_values[i][j] = srvalues;
            play_result_values[j][i] = srvalues;
        }
    }
}

void FactBits::init_states()
{ 
    static const au9_t zau9_t = {0}; // zero initialized;
    // No need for nb (number of bits) == 0 which the final target state.
    State state0;
    state0.out_edges = vau9_t(canon8_values.size(), zau9_t);
    nb_states[0].push_back(state0);
    for (u_t nb = 0; nb <= 8; ++nb)
    {
        const u_t nb_nv = nb_canon8_values[nb].size();
        const size_t n_subsets = 1u << nb_nv;
        // note: nb_states[nb][0] will be used only for nb==0.
        nb_states[nb] = vector<State>(n_subsets, state0);
    }
    const u_t nv = canon8_values.size();
    for (u_t nb = 1; nb <= 8; ++nb)
    {
        const vu_t& nb_values = nb_canon8_values[nb];
        const u_t nb_nv = nb_values.size();
        const u_t n_subsets = 1u << nb_nv;
        // ignore empty subset=0 
        for (u_t subset = 0; subset < n_subsets; ++subset)
        {
            for (u_t j = 0; j < nv; ++j)
            {
                au9_t nb_subsets = zau9_t;
                for (u_t bi = 0; bi < nb_nv; ++bi)
                {
                    if (subset & (1u << bi))
                    {
                        const u_t i = ivalue(nb_values[bi]);
                        const vvu_t& prvi = play_result_values[i];
                        const vu_t& prvij = prvi[j];
                        for (u_t v: prvij)
                        {
                            // if (v != 0)
                            {
                                const u_t nbp = nbits_on(v);
                                u_t r = nb_ivalue(nbp, v);
                                nb_subsets[nbp] |= (1u << r);
                            }
                        }
                    }
                }
                nb_states[nb][subset].out_edges[j] = nb_subsets;
            }
        }
    }
    if (dbg_flags & 0x4) { print_states_graph(cerr); }
}

void FactBits::print_states_graph(ostream& os) const
{
    for (u_t nb = 0; nb <= 8; ++nb)
    {
        const u_t nb_nv = nb_canon8_values[nb].size();
        const vector<State>& states = nb_states[nb];
        const u_t ns = states.size();
        os << "{ nb="<<nb << " #(values)=" << nb_nv << ", #(states)=" <<
            ns << '\n';
        for (u_t si = 1; si < ns; ++si)
        {
            const State& state = nb_states[nb][si];
            os << "  { subset=[" << si << "]=" << strbin(si, 10) << '\n';
            for (u_t vi = 0; vi < canon8_values.size(); ++vi)
            {
                const u_t v = canon8_values[vi];
                os << "    " << strbin(v) << " -> {";
                const au9_t& vstates = state.out_edges[vi];
                for (u_t onb = 0; onb <= 8; ++onb)
                {
                    u_t osubset = vstates[onb];
                    if (osubset != 0)
                    { 
                        const u_t onb_nv = nb_canon8_values[onb].size();
                        os << onb << ": " << strbin(osubset, onb_nv) << ", ";
                    }
                }
                os << "}\n";
            }
            os << "  }\n";
        }
        os << "}\n";
    }
}

void FactBits::solve_minmax()
{
    nb_states[0][1].distance = 0;
    nb_states[0][1].color = 2;
    for (u_t nb = 1; nb <= 8; ++nb)
    {
        state_solve_minmax(nb, nb_states[nb].size() - 1);
    }
state_solve_minmax(4, 1u<<9);
    if (dbg_flags & 0x10) {
        cerr << "states moves:\n";
        for (u_t nb = 0; nb <= 8; ++nb) {
            cerr << "nb="<<nb << '\n';
            const vu_t& values = nb_canon8_values[nb];
            const u_t nb_nv = values.size();
            const vstate_t& states = nb_states[nb];
            for (u_t si = 1; si < states.size(); ++si) {
                const State& state = states[si];
                cerr << "  " << strbin(si, nb_nv) << " D=" << state.distance << 
                    ", best_move = " << strbin(state.best_move) << '\n';
            }
        }
    }
}

void FactBits::state_solve_minmax(u_t nb, u_t si, u_t depth)
{
    State& state = nb_states[nb][si];
    if (state.color == 0)
    {
if (((nb==4) && (si==512)) || nb==8) { 
 cerr << "nb="<<nb << ", depth="<<depth<<'\n';
}
        state.color = 1;
        u_t best_distance = 0x100, best_move = 0x100; // infinity
        const u_t nv = canon8_values.size();
        for (u_t vi = 0; vi < nv; ++vi)
        {
            u_t max_distance = 0;
            const au9_t& nb_subsets = state.out_edges[vi];
            for (u_t onb = 0; onb <= 8; ++onb)
            {
                u_t osubset = nb_subsets[onb];
                if (osubset != 0)
                {
                    state_solve_minmax(onb, osubset, depth + 1);
                    const State& ostate = nb_states[onb][osubset];
                    max_distance = max(max_distance, ostate.distance);
                }
            }
            if (max_distance < best_distance)
            {
                best_distance = max_distance;
                best_move = canon8_values[vi];
            }
        }
        state.distance = min(best_distance + 1, 0x100u);
        state.best_move = best_move;
        state.color = 2;
    }
}

void FactBits::compute_circles()
{
    typedef unordered_set<ull_t> setu_t;
    setu_t circled;
    nb_states[0][1].distance = 0;
    circled.insert(circled.end(), nbsi_key(0, 1));
    const u_t nv = canon8_values.size();
    u_t distance = 0;
    bool adding = true;
    while (adding)
    {
        ++distance;
        adding = false;
        vull_t keys_to_add;
        for (u_t nb = 1; nb <= 8; ++ nb)
        {
            vstate_t& states = nb_states[nb];
            const u_t ns = states.size();
            for (u_t si = 1; si < ns; ++si)
            {
                ull_t key = nbsi_key(nb, si);
                if (circled.find(key) == circled.end())
                {
                    State& state = states[si];
                    for (u_t vi = 0; (vi < nv) && state.distance == 0x100; ++vi)
                    {
                        const au9_t& out_edges = state.out_edges[vi];
                        bool can = true;
                        for (u_t onb = 0; can && (onb <= 8); ++onb)
                        {
                            u_t osi = out_edges[onb];
                            if (osi != 0)
                            {
                                u_t okey = nbsi_key(onb, osi);
                                if (circled.find(okey) == circled.end())
                                {
                                    can = false;
                                }
                            }
                        }
                        if (can)
                        {
                            state.distance = distance;
                            state.best_move = canon8_values[vi];
                            keys_to_add.push_back(key);
                        }
                    }
                }
            }
        }
        for (ull_t key: keys_to_add)
        {
            adding = true;
            circled.insert(circled.end(), key);
        }
    }
}

ull_t FactBits::set_play(aull9_t& nb_sets, ull_t preset, u_t b8) const
{
    ull_t rset = 0;
    const u_t nv = canon8_values.size();
    for (u_t bi = 0; bi < nv; ++bi)
    {
        if (preset & (ull_t(1) << bi))
        {
            const u_t v = canon8_values[bi];
            for (u_t x = b8, rot = 0; rot < 8; ++rot)
            {
                u_t vx = v ^ x;
                rset |= (ull_t(1) << ivalue(canon8[vx]));
                x = (x >> 1) | ((x & 0x1) << 7);
            }
        }
    }
    fill(nb_sets.begin(), nb_sets.end(), 0);
    for (u_t bi = 0; bi < nv; ++bi)
    {
        const ull_t bit = ull_t(1) << bi;
        if (rset & bit)
        {
            const u_t v = canon8_values[bi];
            nb_sets[nbits_on(v)] |= bit;
        }
    }
    return rset;
}

void FactBits::winning_sets()
{
    const u_t nv = canon8_values.size();
    const ull_t one = 1;
    ull_t win_set = 1; // with the first zero-set 00000000
    bool growing = true;
    u_t step = 0;
    while (growing)
    {
        ++step;
        growing = false;
        ull_t win_set_next = win_set;
        for (u_t i = 0; i < nv; ++i)
        {
            const u_t x = canon8_values[i];
            const ull_t xbit = one << i;
            for (u_t j = i; j < nv; ++j)
            {
                const u_t y = canon8_values[j];
                const ull_t ybit = one << j;
                bool winning = (win_set & (xbit | ybit)) == ull_t(0);
                for (u_t rot = 0, xrot = x; winning && (rot < 8); ++rot)
                {
                    xrot = ((xrot >> 1) | ((xrot & 0x1) << 7)) & 0xff;
                    u_t xy = canon8[xrot ^ y];
                    u_t ixy = ivalue(xy);
                    ull_t xybit = one << ixy;
                    winning = (win_set & xybit) != 0;
                }
                if (winning)
                {
                    growing = true;
                    if (dbg_flags & 0x8) {
                        cerr <<  "step="<<step << ", winning += {" << 
                            strbin(x) << ", " << strbin(y)  << "}\n"; }
                    win_set_next |= (xbit | ybit);
                }
            }
        }
        win_set = win_set_next;
    }
}

class Asedatab
{
 public:
    Asedatab(istream& cfi, ostream& cfo, ErrLog &el) :
        fi(cfi), fo(cfo), errlog(el) {}
    void solve_naive();
    void solve();
 private:
    static FactBits factBits;
    bool readline_ints(vi_t &v);
    istream& fi;
    ostream& fo;
    ErrLog &errlog;
};
FactBits Asedatab::factBits;

bool Asedatab::readline_ints(vi_t &v)
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

void Asedatab::solve_naive()
{
    factBits.init();
}

void Asedatab::solve()
{
     solve_naive();
}

static int real_main(int argc, char ** argv)
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
        int slept = 0;
        while (slept < 90)
        {
            sleep(1);
            ++slept;
        }
    }

    string ignore;
    Asedatab asedatab(*pfi, *pfo, errlog);
    if (naive)
    {
         asedatab.solve_naive();
    }
    else
    {
         asedatab.solve_naive();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int stand_alone(int argc, char ** argv)
{
    for (int ai = 0; (ai < argc) && (argv[ai][0] == '-'); ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], 0, 0);
        }
    }
    FactBits  factBits;
    factBits.init();
    return 0;
}

int main(int argc, char ** argv)
{
    int rc = 0;
    if ((argc > 1) && (string(argv[1]) == string("standalone")))
    {
        rc = stand_alone(argc - 2, argv + 2);
    }
    else
    {
        rc = real_main(argc, argv);
    }
    return rc;
}
