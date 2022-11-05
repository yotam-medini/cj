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
typedef vector<vu_t> vvu_t;
// typedef map<u_t, all2_t> uto_all2_t;

static unsigned dbg_flags;

class YXPos
{
 public:
    YXPos(size_t _yi=size_t(-1), size_t _yxi=size_t(-1)) : yi(_yi), yxi(_yxi) {}
    string str() const
    {
        ostringstream ostr;
        ostr << "(" << size_t_str(yi) << ", " << size_t_str(yxi) << ")";
        return ostr.str();
    }
    size_t yi;
    size_t yxi;
 private:
    string size_t_str(size_t i) const
    {
        ostringstream ostr;
        if (yxi == size_t(-1))
        {
            ostr << "inf";
        }
        else
        {
            ostr << i;
        }
        return ostr.str();
    }
};
bool operator<(const YXPos& p0, const YXPos& p1)
{
    return make_tuple(p0.yi, p0.yxi) < make_tuple(p1.yi, p1.yxi);
}

class EnergyEdge
{
 public:
    EnergyEdge(ll_t _e=0, const YXPos& _f=YXPos(), const YXPos& _t=YXPos()) :
        e(_e), ypos_from(_f), ypos_to(_t) {}
    string str() const
    {
        ostringstream ostr;
        ostr << "{e=" << e << ", f=" << ypos_from.str() <<
            ", t=" << ypos_to.str() << "}";
        return ostr.str();
    }
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
typedef vector<viflower_t> vviflower_t;

class YFlower
{
 public:
    YFlower(ull_t _X=0, ull_t _C=0, size_t _i=0) :
        X(_X), C(_C), i(_i) {}
    ull_t X;
    ull_t C;
    EnergyEdge energy_edge[2]; // 2 directions
    // for debug:
    size_t i; // orig index 
}; 
typedef vector<YFlower> vyflower_t;
typedef vector<vyflower_t> vvyflower_t;

class Cute
{
 public:
    Cute(istream& fi);
    Cute(const vflower_t& _flowers, ll_t _E) :
        N(_flowers.size()), E(_E), flowers(_flowers), solution(0) {}
    void solve_naive0();
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
    const vu_t get_best_perm() const { return best_perm; }
 private:
    typedef vector<EnergyEdge> venergyedge_t;
    typedef array<venergyedge_t, 2> venergyedge2_t;
    void naive_iterate(size_t yi);
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
    void process_yflowers_onedir(size_t yi, u_t dir);
    void process_yflowers_uturn(size_t yi, u_t dir);
    void pick_and_update(size_t yi, u_t dir);
    void add_option_to(
        const EnergyEdge& eebase,
        const EnergyEdge& eeto,
        ll_t e_add=0)
    {
        add_option(eebase, eeto.e + e_add, eeto.ypos_from);
    }
    void add_option(
        const EnergyEdge& eebase,
        ll_t energy_add,
        const YXPos& ypos_to);
    void print_venergyedge(
        const char* label, 
        const venergyedge2_t& vee, 
        ostream& os=cerr) const;
    void print_x2e(ostream& os=cerr) const;

    u_t N;
    ll_t E;
    vflower_t flowers;
    ll_t solution;

    // naive data
    vu_t best_perm;
    vvu_t yidxs;
    vu_t prefix;

    // non-naive data
    enum {RightPositive, LeftNegative}; // [0], [1]
    vvyflower_t yxflowers;
    vyflower_t low_flowers;
    venergyedge_t e_options;
    typedef map<u_t, EnergyEdge> x2e_t;
    typedef pair<x2e_t::const_iterator, x2e_t::const_iterator> x2e_cer_t;
    typedef pair<x2e_t::iterator, x2e_t::iterator> x2e_er_t;
    x2e_t x2e[2]; // x2e[0] decreasing, x2e[1] increasing
    venergyedge2_t onedir;
    venergyedge2_t uturn;
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

void Cute::solve_naive0()
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
    for (u_t fi = 0; possible && (fi < perm.size()); ++fi)
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

void Cute::solve_naive()
{
    viflower_t sflowers; sflowers.reserve(N);
    for (size_t i = 0; i < N; ++i)
    {
        sflowers.push_back(IFlower(flowers[i], i));
    }
    sort(sflowers.begin(), sflowers.end(),
        [](const Flower& f0, const Flower& f1) -> bool
        {
            const bool lt = f0.Y > f1.Y;
            return lt;
        });
   for (u_t i = 0; i < N; )
   {
       const ull_t y = sflowers[i].Y;
       vu_t yidx;
       for ( ; (i < N) && (sflowers[i].Y == y); ++i)
       {
           yidx.push_back(sflowers[i].i);
       }
       yidxs.push_back(yidx);
   }
   naive_iterate(0);
}

void Cute::naive_iterate(size_t yi)
{
    if (yi == yidxs.size())
    {
        try_perm(prefix);
    }
    else // recurse
    {
        const u_t prefix_size = prefix.size();
        const vu_t& yidx = yidxs[yi];
        const u_t ysize = yidx.size();
        const u_t mask_max = 1u << ysize;
        for (u_t mask = 0; mask < mask_max; ++mask)
        {
            vu_t comb;
            for (u_t yfi = 0; yfi < ysize; ++yfi)
            {
                 if (mask & (1u << yfi))
                 {
                     comb.push_back(yidx[yfi]);
                 }
            }
            sort(comb.begin(), comb.end());
            for (bool more = true; more;
                more = next_permutation(comb.begin(), comb.end()))
            {
                prefix.erase(prefix.begin() + prefix_size, prefix.end());
                prefix.insert(prefix.end(), comb.begin(), comb.end());
                naive_iterate(yi + 1);
            }
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
        if (dbg_flags & (0x2 | 0x4 | 0x8)) { cerr << "yi=" << yi << '\n'; }
        const size_t sz = yxflowers[yi].size();
        const venergyedge_t vzen(sz, EnergyEdge(0, N, N));
        for (u_t dir: {0, 1})
        {
            onedir[dir] = uturn[dir] = vzen;
        }
        for (u_t dir: {0, 1})
        {
            process_yflowers_onedir(yi, dir);
        }
        if (dbg_flags & 0x4) { print_venergyedge("onedir", onedir); }
        for (u_t dir: {0, 1})
        {
            process_yflowers_uturn(yi, dir);
        }
        if (dbg_flags & 0x4) { print_venergyedge("uturn", uturn); }
        for (u_t dir: {0, 1})
        {
            pick_and_update(yi, dir);
        }
        if (dbg_flags & 0x8) { print_x2e(); }
        if (dbg_flags & 0x2) {
           for (u_t i = 0; i < sz; ++i) { const YFlower& f = yxflowers[yi][i];
               cerr << "F["<<i<<"] X=" << f.X << 
                   ", e[0]=" << f.energy_edge[0].str() <<
                   ", e[1]=" << f.energy_edge[1].str() << '\n'; }
        }
    }
    solution = x2e[0].begin()->second.e;
}

void Cute::process_yflowers_onedir(size_t yi, u_t dir)
{
    vyflower_t& level_flowers = yxflowers[yi];
    const size_t sz = level_flowers.size();

    const int ib = (dir == 0 ? sz - 1 : 0);
    const int ie = (dir == 0 ? -1 : sz);
    const int step = (dir == 0 ? -1 : 1);
    for (int i = ib, ipre = -1; i != ie; ipre = i, i += step) 
    {
        const YFlower& f = level_flowers[i];
        const YXPos yxpos(yi, i);
        const EnergyEdge ee_base(f.C, yxpos);
        e_options.clear();
        if (ipre == -1)
        {
            add_option(ee_base, 0, YXPos(N, N)); 
        }
        else
        {
            add_option_to(ee_base, onedir[dir][ipre]);
        }
        if (yi > 0)
        {
            x2e_cer_t er;
            for (bool keepdir: {true, false})
            {
                const u_t subdir = keepdir ? dir : 1 - dir;
                const ll_t e_add = keepdir ? 0 : -E;
                er = x2e[subdir].equal_range(f.X);
                x2e_t::const_iterator iter = er.first;
                if (er.first != er.second)
                {
                    add_option_to(ee_base, iter->second, e_add);
                }
                else // f.X not found in x2e[dir]
                {
                    if (subdir == 0)
                    {
                        if (iter != x2e[0].end())
                        {
                            add_option_to(ee_base, iter->second, e_add);
                        }
                    }
                    else // dir == 1
                    {
                        if (iter != x2e[1].begin())
                        {
                            --iter;
                            add_option_to(ee_base, iter->second, e_add);
                        }
                    }
                }
            }
        }
        onedir[dir][i] = *max_element(e_options.begin(), e_options.end());
    }
}

void Cute::process_yflowers_uturn(size_t yi, u_t dir)
{
    vyflower_t& level_flowers = yxflowers[yi];
    const int sz = level_flowers.size();
    const int ib = (dir == 0 ? sz - 1 : 0);
    const int ie = (dir == 0 ? -1 : sz);
    const int step = (dir == 0 ? -1 : 1);
    ll_t csum = 0;
    for (int i = ib, inext = i + step; i != ie; i = inext, inext += step) 
    {
        const YFlower& f = level_flowers[ib];
        const YXPos yxpos(yi, i);
        int step_to = (dir == 0 ? (i + 1 < sz ? 1 : -1) : (i == 0 ? 1 : -1));
        const YXPos pos_to(yi, i + step_to);
        csum += f.C;
        if (inext != ie)
        {
            const EnergyEdge& tailturn = onedir[1 - dir][inext];
            uturn[dir][i] = EnergyEdge(
                csum + tailturn.e - E, yxpos, pos_to);
        }
        else
        {
            uturn[dir][i] = onedir[dir][i];
        }
    }
}

void Cute::pick_and_update(size_t yi, u_t dir)
{
    vyflower_t& level_flowers = yxflowers[yi];
    const size_t sz = level_flowers.size();
    x2e_t& x2ed = x2e[dir];

    for (size_t i = 0; i < sz; ++i)
    {
        YFlower& f = level_flowers[i];
        const EnergyEdge& ee = (onedir[dir][i].e < uturn[dir][i].e
            ? uturn[dir][i] : onedir[dir][i]);
        f.energy_edge[dir] = ee;
        const x2e_t::value_type v(f.X, ee);
        x2e_er_t er = x2ed.equal_range(f.X);
        x2e_t::iterator iter = er.second;
        if (er.first != er.second)
        {
            x2ed.erase(er.first);
        }
        iter = x2ed.insert(iter, v);
        if (dir == 0)
        {
            x2e_t::iterator iterb(iter);
            for ( ; (iterb != x2ed.begin()) && (iterb->second.e <= ee.e);
                --iterb)
            { ; }
            if (iterb->second.e > ee.e)
            {
                ++iterb;
            }
            x2ed.erase(iterb, iter);
        }
        else // dir == 1
        {
            ++iter;
            x2e_t::iterator iter_next(iter);
            if (iter != x2ed.end()) { ++iter_next; }
            for (; ((iter != x2ed.end()) && (iter->second.e <= ee.e)); 
                iter = iter_next++)
            {
                x2ed.erase(iter);
            }
        }
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

void Cute::print_venergyedge(
    const char* label, 
    const venergyedge2_t& vee, 
    ostream& os) const
{
    os << "{" << label << '\n';
    for (u_t dir: {0, 1})
    {
        os << " [" << dir << "]:\n";
        for (const EnergyEdge& ee: vee[dir])
        {
            os << "  " << ee.str() << '\n';
        }
    }
    os << "}\n";
}

void Cute::print_x2e(ostream& os) const
{
    os << "{x2e:\n";
    for (u_t dir: {0, 1})
    {
        os << " [" << dir << "]:\n";
        for (auto const& [X, ee] : x2e[dir])
        {
            os << "  X=" << X << " -> " << ee.str() << '\n';
        }
    }
    os << "}\n";
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
