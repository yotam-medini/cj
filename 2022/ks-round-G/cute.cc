// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ll_t> vll_t;
typedef vector<ull_t> vull_t;
typedef array<ll_t, 2> all2_t;
typedef array<ull_t, 2> aull2_t;
// typedef map<u_t, all2_t> uto_all2_t;

static unsigned dbg_flags;

class YXPos
{
 public:
    YXPos(size_t _yi=size_t(-1), size_t _yxi=size_t(-1)) : yi(_yi), yxi(_yxi) {}
    size_t yi;
    size_t yxi;
};
bool operator<(const YXPos& p0, const YXPos& p1)
{
    return make_tuple(p0) < make_tuple(p1);
}

class EnergyNext
{
 public:
    EnergyNext(ll_t _e=0, size_t _yi=0, size_t _yxi=0) :
        e(_e), yi(_yi), yxi(_yxi) {}
    ll_t e;
    size_t yi;
    size_t yxi;
};

class EnergyEdge
{
 public:
    EnergyEdge(ll_t _e=0, const YXPos& _f=YXPos(), const YXPos& _t=YXPos()) :
        e(_e), ypos_from(_f), ypos_to(_t) {}
    ll_t e;
    YXPos ypos_from;
    YXPos ypos_to;
};
bool operator<(const EnergyEdge& ee0, const EnergyEdge& ee1)
{
    return 
       make_tuple(ee0.e, ee0.ypos_from, ee0.ypos_to) <
       make_tuple(ee1.e, ee1.ypos_from, ee1.ypos_to);
}

class Flower
{
 public:
    Flower(ull_t x=0, ull_t y=0, ll_t c=0) : X(x), Y(y), C(c) {}
    string str() const
    {
        ostringstream oss;
        oss << "F{("<<X<<", "<<Y<<") C="<<C<<"}";
        return oss.str();
    }
    ull_t X, Y;
    ll_t C;
};
typedef vector<Flower> vflower_t;

class IFlower: public Flower
{
 public:
    IFlower(const Flower& f=Flower(), size_t _i=0): Flower(f), i(_i) {}
    size_t i;
};
typedef vector<IFlower> viflower_t;

class YFlower
{
 public:
    YFlower(ull_t _X=0, ull_t _C=0, size_t _i=0) :
        X(_X), C(_C), max_energy{0, 0}, i(_i), 
        continue_to(-1), continue_uturn_to(-1) {}
    ull_t X;
    ull_t C;
    ull_t max_energy[2]; // assuming 2 directions
    // for debug:
    size_t i; // orig index 
    size_t continue_to, continue_uturn_to;
}; 
typedef vector<YFlower> vyflower_t;
typedef vector<vyflower_t> vvyflower_t;

class Cute
{
 public:
    Cute(istream& fi);
    Cute(const vflower_t& _flowers, ll_t _E) :
        N(_flowers.size()), E(_E), flowers(_flowers), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
    const vu_t get_best_perm() const { return best_perm; }
 private:
    typedef vector<EnergyEdge> venergyedge_t;
    void try_perm(const vu_t& perm);
    bool try_energy(ll_t energy)
    {
        const bool better = (solution < energy);
        if (better)
        {
            solution = energy;
        }
        return better;
    }
    // non naive methods
    void set_yxflowers();
    void next_level_energy(vyflower_t& level_flowers, const vll_t& csums);
    void add_x_energy(u_t x, ll_t e_positive_right, ll_t e_negative_left); 
    void add_option_to(const EnergyEdge& eebase, const EnergyEdge& eeto)
    {
        add_option(eebase, eeto.e, eeto.ypos_from);
    }
    void add_option_to_chdir(const EnergyEdge& eebase, const EnergyEdge& eeto)
    {
        add_option(eebase, eeto.e - E, eeto.ypos_from);
    }
    void add_option(
        const EnergyEdge& eebase,
        ll_t energy_add,
        const YXPos& ypos_to);

    u_t N;
    ll_t E;
    vflower_t flowers;
    ll_t solution;

    // naive data
    vu_t best_perm;

    // non-naive data
    enum {RightPositive, LeftNegative}; // [0], [1]
    vvyflower_t yxflowers;
    vyflower_t low_flowers;
    venergyedge_t e_options;
    typedef map<u_t, EnergyEdge> x2e_t;
    typedef pair<x2e_t::const_iterator, x2e_t::const_iterator> x2e_cer_t;
    x2e_t x2e[2]; // x2e[0] decreasing, x2e[1] increasing
};

Cute::Cute(istream& fi) : solution(0)
{
    fi >> N >> E;
    flowers.reserve(N);
    while (flowers.size() < N)
    {
        ull_t x, y;
        ll_t c;
        fi >> x >> y >> c;
        flowers.push_back(Flower(x, y, c));
    }
}

void Cute::solve_naive()
{
    vu_t perm; perm.reserve(N);
    while (perm.size() < N) { perm.push_back(perm.size()); }
    for (bool more = true; more;
        more = next_permutation(perm.begin(), perm.end()))
    {
         try_perm(perm);
    }
    if (dbg_flags & 0x1) { cerr << "best_perm:";
        for (u_t x: best_perm) { cerr << ' ' << x; } cerr << '\n'; }
}

void Cute::try_perm(const vu_t& perm)
{
    ull_t xcurr = 0, ycurr = 1000000000000000000;
    bool possible = true;
    bool positive = true;
    ll_t energy = 0;
    for (u_t fi = 0; possible && (fi < N); ++fi)
    {
        const Flower& f = flowers[perm[fi]];
        possible = f.Y <= ycurr;
        if (possible)
        {
            // const ll_t dy = ycurr - f.Y;
            // const ll_t dx = (xcurr < f.X ? f.X - xcurr : xcurr - f.X);
            possible = true; // (dy <= dx);
            if (possible)
            {
                if (positive)
                {
                    if (f.X < xcurr)
                    {
                        energy -= E;
                        positive = false;
                    }
                }
                else // !positive
                {
                    if (xcurr < f.X)
                    {
                        energy -= E;
                        positive = true;
                    }
                }
                energy += f.C;
            }
            if (try_energy(energy)) 
            { 
                const vu_t::const_iterator pb = perm.cbegin();
                best_perm.clear();
                best_perm.insert(best_perm.end(), pb, pb + (fi + 1));
            }
            xcurr = f.X;
            ycurr = f.Y;
        }
    }
}

void Cute::solve()
{
    set_yxflowers();
    const size_t ny = yxflowers.size();
    // lowest level flowers;
    for (u_t yi = 0; yi < ny; ++yi)
    {
        vyflower_t& level_flowers = yxflowers[yi];

        const size_t sz = level_flowers.size();
        vll_t csums; 
        csums.reserve(sz + 1);
        csums.push_back(0);
        for (const YFlower& f: level_flowers)
        {
            csums.push_back(csums.back() + f.C); 
        }
        typedef vector<x2e_t::iterator> vx2eiter_t;
        vx2eiter_t next_low[2];
        for (u_t i: {0, 1}) { next_low[i] = vx2eiter_t(sz, x2e[i].end()); }

        const venergyedge_t vzen(sz, EnergyEdge(0, N, N));
        venergyedge_t onedir[2] = {vzen, vzen};
        for (u_t dir: {0, 1})
        {
            // right-positive (no U-turn)
            const int ib = (dir == 0 ? sz - 1 : 0);
            const int ie = (dir == 0 ? -1 : sz);
            const int step = (dir == 0 ? 1 : -1);
            for (int i = ib, ipre = -1; i != ie; ipre = i, i += step) 
            {
                const YFlower& f = level_flowers[ib];
                const YXPos yxpos(yi, i);
                const EnergyEdge ee_base(f.C, YXPos(yi, i));
                onedir[dir][ib] = f.C;
                e_options.clear();
                if (ipre != -1)
                {
                    add_option_to(ee_base, onedir[dir][ipre]);
                }
                if (yi > 0)
                {
                    x2e_cer_t er;
                    // going down keeping direction
                    er = x2e[dir].equal_range(f.X);
                    if (er.first != er.second)
                    {
                        add_option_to(ee_base, er.first->second);
                    }
                    else // f.x not found in x2e[dir]
                    {
                        if (er.first == x2e[0].end())
                        {
                            const EnergyEdge& ee_low =
                                x2e[dir].rbegin()->second;
                            add_option_to_chdir(ee_base, ee_low);
                        }
                    }
                    // going down changing direction
                    er = x2e[1 - dir].equal_range(f.X);
                    if (er.first != er.second)
                    {
                        add_option_to_chdir(ee_base, er.first->second);
                    }
                }
            }
        }
        venergyedge_t uturn[2] = {vzen, vzen};
    }
}

void Cute::add_option(
    const EnergyEdge& eebase,
    ll_t energy_add,
    const YXPos& ypos_to)
{
    EnergyEdge ee(eebase.e + energy_add, eebase.ypos_from, ypos_to);
    e_options.push_back(ee);
}

#if 0
void Cute::solve()
{
    set_yxflowers();
    const size_t ny = yxflowers.size();
    // lowest level flowers;
    for (u_t yi = 0; yi < ny; ++yi)
    {
        vyflower_t& level_flowers = yxflowers[yi];

        const size_t sz = level_flowers.size();
        vll_t csums; 
#if 1
        csums.reserve(sz + 1);
        csums.push_back(0);
        for (const YFlower& f: level_flowers)
        {
            csums.push_back(csums.back() + f.C); 
        }
#else
        csums.reserve(sz);
        csums.push_back(level_flowers[0].C);
        for (size_t i = 1; i < sz; ++i)
        {
            csums.push_back(csums.back() + level_flowers[i].C); 
        }
#endif
        if (yi == 0)
        {
            for (size_t i = 0; i < sz; ++i)
            {
                // ull_t alt_rl = csums[sz] - csums[i];
                YFlower& f = level_flowers[i];
                ll_t i_energy = f.C;
                ll_t left_energy = csums[i];
                ll_t right_energy = csums[sz] - csums[i + 1];
                f.max_energy[0] = i_energy + right_energy +
                    max(left_energy - E, 0ll);
                f.max_energy[1] = i_energy + left_energy +
                    max(right_energy - E, 0ll);
                add_x_energy(f.X, f.max_energy[0], f.max_energy[1]);
                // try_energy(f.max_energy[0]);
                // try_energy(f.max_energy[1]);
            }
            low_flowers = low_flowers; // copy
        }
        else
        {
            next_level_energy(level_flowers, csums);
        }
    }
}
#endif

void Cute::set_yxflowers()
{
    viflower_t sflowers; sflowers.reserve(N);
    for (size_t i = 0; i < N; ++i)
    {
        sflowers.push_back(IFlower(flowers[i], i));
    }
    sort(sflowers.begin(), sflowers.end(),
        [](const Flower& f0, const Flower& f1) -> bool
        {
            const bool lt = f0.Y < f1.Y;
            return lt;
        });
    for (size_t si = 0; si < N; )
    {
        vyflower_t yflowers;
        const ull_t y = sflowers[si].Y;
        for ( ; (si < N) && (sflowers[si].Y == y); ++si)
        {
            const IFlower& f = sflowers[si];
            yflowers.push_back(YFlower(f.X, f.C, f.i));
        }
        sort(yflowers.begin(), yflowers.end(),
            [](const YFlower& f0, YFlower& f1) -> bool
            {
                const bool lt = f0.X < f1.X;
                return lt;
            });
        yxflowers.push_back(yflowers);
    }
}

void Cute::next_level_energy(vyflower_t& level_flowers, const vll_t& csums)
{
#if 0
    const size_t sz = level_flowers.size();
    for (size_t i = 0; i < sz; ++i)
    {
        YFlower& f = level_flowers[i];
        ll_t i_energy = f.C;
        ll_t left_energy = csums[i];
        ll_t right_energy = csums[sz] - csums[i + 1];
        f.max_energy[0] = i_energy + right_energy +
            max(left_energy - E, 0ll);
        f.max_energy[1] = i_energy + left_energy +
            max(right_energy - E, 0ll);
    }
    for (size_t i = 0; i < sz; ++i)
    {
        YFlower& f = level_flowers[i];
        auto er = x2e.equal_range(f.X);
        x2e_t::iterator iter = er.first;
        if (iter != er.second)
        { // found
            ;
        }
        else 
        { // not found
            ;
        }
    }
#endif
}

void Cute::add_x_energy(u_t x, ll_t e_positive_right, ll_t e_negative_left)
{
#if 0
    x2e_t::iterator iter = x2e.find(x);
    if (iter != x2e.end())
    {
        all2_t& e2 = iter->second;
        e2[0] = max(e2[0], e_positive_right);
        e2[1] = max(e2[1], e_negative_left);
    }
    else
    {
        const all2_t e2{e_positive_right, e_negative_left};
        iter = x2e.insert(iter, x2e_t::value_type{x, e2});
    }
    const all2_t& e2 = iter->second;
    try_energy(e2[0]);
    try_energy(e2[1] + E);
#endif
}

void Cute::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int real_main(int argc, char ** argv)
{
    const string dash("-");

    bool naive = false;
    bool tellg = false;
    u_t solve_ver = 0;
    int rc = 0, ai;

    for (ai = 1; (rc == 0) && (ai < argc) && (argv[ai][0] == '-') &&
        argv[ai][1] != '\0'; ++ai)
    {
        const string opt(argv[ai]);
        if (opt == string("-naive"))
        {
            naive = true;
        }
        else if (opt == string("-solve1"))
        {
            solve_ver = 1;
        }
        else if (opt == string("-debug"))
        {
            dbg_flags = strtoul(argv[++ai], nullptr, 0);
        }
        else if (opt == string("-tellg"))
        {
            tellg = true;
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

    string ignore;
    unsigned n_cases;
    *pfi >> n_cases;
    getline(*pfi, ignore);

    void (Cute::*psolve)() =
        (naive ? &Cute::solve_naive : &Cute::solve);
    if (solve_ver == 1) { psolve = &Cute::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Cute cute(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (cute.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        cute.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static u_t rand_range(u_t nmin, u_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

#if 0
static void save_case(const char* fn)
{
    ofstream f(fn);
    f << "1\n";
    f.close();
}

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (dbg_flags & 0x100) { save_case("cute-curr.in"); }
    if (small)
    {
        Cute p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Cute p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("cute-fail.in");
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
    return rc;
}
#endif

static int test_case(const vflower_t& flowers, ll_t E)
{
    int rc = 0;
    const u_t N = flowers.size();
    Cute p(flowers, E);
    p.solve_naive();
    ll_t solution = p.get_solution();
    vu_t perm = p.get_best_perm();
    cerr << "flowers[" << N << "]:";
    for (const Flower& f: flowers) { cerr << ' ' << f.str(); }
    cerr << " -> " << solution << "  perm=(";
    for (u_t x: perm) { cerr << ' ' << x; } cerr << ")\n";
    typedef map<ull_t, u_t> ycount_t;
    ycount_t ycount, yperm_count;
    for (const Flower& f: flowers)
    {
        ycount_t::iterator iter = ycount.find(f.Y);
        if (iter == ycount.end())
        {
            iter = ycount.insert(iter, ycount_t::value_type(f.Y, 0));
        }
        ++(iter->second);
    }
    for (u_t i: perm)
    {
        const Flower& f = flowers[i];
        ycount_t::iterator iter = yperm_count.find(f.Y);
        if (iter == yperm_count.end())
        {
            iter = yperm_count.insert(iter, ycount_t::value_type(f.Y, 0));
        }
        ++(iter->second);
    }
    for (const ycount_t::value_type& kv: yperm_count)
    {
        const ull_t y = kv.first;
        if (kv.second < ycount[y])
        {
            cerr << "solution #(y=" << y << ")=" << kv.second <<
                " < " << ycount[y] << '\n';
                rc = 1;
        }
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t n_tests = strtoul(argv[ai++], nullptr, 0);
    const size_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const size_t Nmax = strtoul(argv[ai++], nullptr, 0);
    const ll_t Xmin = strtoul(argv[ai++], nullptr, 0);
    const ll_t Xmax = strtoul(argv[ai++], nullptr, 0);
    const ll_t Ymin = strtoul(argv[ai++], nullptr, 0);
    const ll_t Ymax = strtoul(argv[ai++], nullptr, 0);
    const ll_t Cmin = strtoul(argv[ai++], nullptr, 0);
    const ll_t Cmax = strtoul(argv[ai++], nullptr, 0);
    const ll_t Emin = strtoul(argv[ai++], nullptr, 0);
    const ll_t Emax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Xmin=" << Xmin << ", Xmax=" << Xmax <<
        ", Ymin=" << Ymin << ", Ymax=" << Ymax <<
        ", Cmin=" << Cmin << ", Cmax=" << Cmax <<
        ", Emin=" << Emin << ", Emax=" << Emax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        size_t N = rand_range(Nmin, Nmax); 
        ll_t E = rand_range(Emin, Emax); 
        set<aull2_t> XYs;
        vflower_t flowers; flowers.reserve(N);
        while (flowers.size() < N)
        {
             aull2_t xy;
             do
             {
                 xy[0] = rand_range(Xmin, Xmax);
                 xy[1] = rand_range(Ymin, Ymax);
             } while (XYs.find(xy) != XYs.end());
             XYs.insert(XYs.end(), xy);
             ll_t c = rand_range(Cmin, Cmax);
             flowers.push_back(Flower(xy[0], xy[1], c));
        }
        rc = test_case(flowers, E);
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test_random(argc - 2, argv + 2)
        : real_main(argc, argv));
    return rc;
}
