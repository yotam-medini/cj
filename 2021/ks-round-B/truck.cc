// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;
// typedef map<u_t, u_t> utou_t;
typedef map<u_t, ull_t, greater<u_t>> utoull_t;
typedef unordered_set<u_t> setu_t;
typedef vector<setu_t> vsetu_t;

static unsigned dbg_flags;

class SegTreeGCD
{
 public:
    SegTreeGCD(const vu_t& _keys) : keys(_keys)
    {
        sort(keys.begin(), keys.end());
        tree = vull_t(4*keys.size(), 0);
    }
    void point_update(u_t key, ull_t value)
    {
        u_t v = lower_bound(keys.begin(), keys.end(), key) - keys.begin();
        update(key, value, v, 0, keys.size() - 1);
    }
    ull_t query(u_t low, u_t high) const;
 private:
    void update(u_t key, ull_t value, u_t v, u_t low, u_t high)
    {
        ull_t ret;
        if (low == high)
        {
            tree[key] = ret = value;
        }
        else
        {
            const u_t mid = (low + high)/2;
            const u_t vl = 2*v + 1;
            const u_t vr = 2*v + 2;
            if (v <= mid) {
                update(key, value, vl, low, mid);
            } else {
                update(key, value, vr, mid + 1, high);
            }
            if (tree[vl] == 0)
            {
                tree[v] = tree[vr];
            } 
            else if (tree[vr] == 0)
            {
                tree[v] = tree[vl];
            } 
            else
            {
                tree[v] = gcd(tree[vl], tree[vr]);
            }
        }
    }
    vu_t keys;
    vull_t tree;
};

class Road
{
 public:
    Road(u_t x=0, u_t y=0, u_t l=0, ull_t a=0) : 
        X(x), Y(y), L(l), A(a) {}
    u_t X, Y;
    u_t L;
    ull_t A;
};
typedef vector<Road> vroad_t;


class RoadTo
{
 public:
    RoadTo(u_t _to=0, u_t l=0, ull_t a=0, u_t d=0) : 
        to(_to), L(l), A(a), depth(d) {}
    u_t to;
    u_t L;
    ull_t A;
    u_t depth;
};
typedef vector<RoadTo> vroadto_t;


class Query
{
 public:
    Query(u_t c=0, u_t w=0): C(c), W(w) {}
    u_t C;
    u_t W;
};
typedef vector<Query> vquery_t;

class City
{
 public:
    City() : to(0), gcd_charge(0) {}
    u_t to;
    vu_t froms;
    RoadTo road_to;
    vu_t qidx;
    ull_t gcd_charge;
};
typedef vector<City> vcity;

class Truck
{
 public:
    Truck(istream& fi);
    Truck(const vroad_t& r, const vquery_t& q) :
        N(r.size() + 1), Q(q.size()), roads(r), queries(q) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vull_t& get_solution() const { return solution; }
 private:
    void set_roadsto();
    void set_cities_graph();
    u_t N, Q;
    vroad_t roads;
    vquery_t queries;
    vull_t solution;
    vroadto_t roadsto; // [0] unused
    vcity cities;
};

Truck::Truck(istream& fi)
{
    fi >> N >> Q;
    roads.reserve(N - 1);
    while (roads.size() < N - 1)
    {
        u_t x, y, l;
        ull_t a;
        fi >> x >> y >> l >> a;
        roads.push_back(Road(x, y, l, a));
    }
    queries.reserve(Q);
    while (queries.size() < Q)
    {
        u_t c, w;
        fi >> c >> w;
        queries.push_back(Query(c, w));
    }
}

void Truck::solve_naive()
{
    set_roadsto();
    for (u_t qi = 0; qi < Q; ++qi)
    {
        if ((dbg_flags & 0x1) & ((qi & (qi - 1)) == 0)) {
            cerr << "query " << qi << '/' << Q << '\n'; }
        const Query& q = queries[qi];
        ull_t a = 0;
        u_t c = q.C;
        while ((c != 1) && (a != 1))
        {
            const RoadTo roadTo = roadsto[c];
            if (q.W >= roadTo.L)
            {
                a = (a == 0 ? roadTo.A : gcd(a, roadTo.A));
            }
            c = roadTo.to;
        }
        solution.push_back(a);
    }
}

class StackElement
{
 public:
    StackElement(u_t c=0, u_t i=0) : city(c), ifrom(i) {}
    u_t city;
    u_t ifrom;
    utoull_t::iterator iter;
};

void Truck::solve()
{
    set_cities_graph();
    for (u_t qi = 0; qi < Q; ++qi)
    {
        cities[queries[qi].C].qidx.push_back(qi);
    }
    solution = vull_t(Q, 0);
    utoull_t level_to_ci;
    // vau2_t stack_city_ifrom;
    // stack_city_ifrom.push_back(au2_t{1, 0}); // start from city=1
    vector<StackElement> stack_city;
    stack_city.push_back(StackElement(1, 0));
    stack_city.back().iter = level_to_ci.end();
    while (!stack_city.empty())
    {
        StackElement& e = stack_city.back();
        City& city_to = cities[e.city];
        if (e.ifrom == city_to.froms.size())
        {
            if (e.city != 1)
            {
                level_to_ci.erase(e.iter);
            }
            stack_city.pop_back();
        }
        else
        {
            u_t a = city_to.froms[e.ifrom];
            City& fcity = cities[a];
            ++e.ifrom;
            stack_city.push_back(StackElement(a, 0));
            utoull_t::iterator iter = level_to_ci.lower_bound(fcity.road_to.L);
            if (iter == level_to_ci.end())
            {
                fcity.gcd_charge = fcity.road_to.A;
            }
            else
            {
                ull_t gcdA = iter->second;
                fcity.gcd_charge = gcd(gcdA, fcity.road_to.A);
            }
            const utoull_t::value_type v{fcity.road_to.L, fcity.gcd_charge};
            StackElement& f = stack_city.back();
            f.iter = level_to_ci.insert(iter, v);
            for (u_t qi: fcity.qidx)
            {
                const u_t W = queries[qi].W;
                iter = level_to_ci.lower_bound(W);
                if (iter != level_to_ci.end())
                {
                    solution[qi] = iter->second;
                }
#if 0
                if (iter == level_to_ci.end())
                {
                    --iter;
                }
                if ((iter->first > W) && (iter != level_to_ci.begin()))
                {
                    --iter;
                }
                if (iter->first <= W)
                {
                    solution[qi] = iter->second;
                }
#endif
            }
        }
    }
}

void Truck::set_roadsto()
{
    vsetu_t connections(N + 1, setu_t());
    for (u_t ri = 0; ri < roads.size(); ++ri)
    {
        const Road& road = roads[ri];
        connections[road.X].insert(ri);
        connections[road.Y].insert(ri);
    }
    roadsto = vroadto_t(N + 1, RoadTo());
    u_t depth = 0;
    vu_t depth_cities;
    depth_cities.push_back(1);
    u_t roads_set = 1;
    while (roads_set < N)
    {
        ++depth;
        vu_t next_depth_cities;
        for (u_t city: depth_cities)
        {
            for (u_t ri: connections[city])
            {
                const Road& road = roads[ri];
                u_t acity = (city == road.X ? road.Y : road.X);
                RoadTo& roadTo = roadsto[acity];
                if (roadTo.to == 0) // still unset
                {
                    roadTo = RoadTo(city, road.L, road.A, depth);
                    next_depth_cities.push_back(acity);
                }
            }
        }
        roads_set += next_depth_cities.size();
        swap(depth_cities, next_depth_cities);
    }
}

void Truck::set_cities_graph()
{
    cities = vcity(N + 1, City()); // cities[0], cities[1] are  unused
    vsetu_t connections(N + 1, setu_t());
    for (u_t ri = 0; ri < roads.size(); ++ri)
    {
        const Road& road = roads[ri];
        connections[road.X].insert(ri);
        connections[road.Y].insert(ri);
    }
    u_t depth = 0;
    vu_t depth_cities;
    depth_cities.push_back(1);
    u_t cities_set = 1;
    while (cities_set < N)
    {
        vu_t next_depth_cities;
        for (u_t dci: depth_cities)
        {
            City& city = cities[dci];
            for (u_t ri: connections[dci])
            {
                const Road& road = roads[ri];
                u_t f = (dci == road.X ? road.Y : road.X);
                City& fcity = cities[f];
                if (fcity.road_to.to == 0) // still unset
                {
                    if (f == 1)
                    {
                        fcity.road_to = RoadTo(u_t(-1), 0, 0, 0);
                    }
                    else
                    {
                        fcity.road_to = RoadTo(dci, road.L, road.A, depth);
                        city.froms.push_back(f);
                    }
                    next_depth_cities.push_back(f);
                }
            }
        }
        ++depth;
        cities_set += next_depth_cities.size();
        swap(depth_cities, next_depth_cities);
    }
}

void Truck::print_solution(ostream &fo) const
{
    for (ull_t a: solution)
    {
        fo << ' ' << a;
    }
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
            dbg_flags = strtoul(argv[++ai], 0, 0);
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

    void (Truck::*psolve)() =
        (naive ? &Truck::solve_naive : &Truck::solve);
    if (solve_ver == 1) { psolve = &Truck::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Truck truck(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (truck.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        truck.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static ull_t rand_range(ull_t nmin, ull_t nmax)
{
    u_t r = nmin + rand() % (nmax + 1 - nmin);
    return r;
}

static void save_case(
    const char* fn,
    const vroad_t& roads,
    const vquery_t& queries,
    u_t qi)
{
    const u_t N = roads.size() + 1, Q = queries.size();
    ofstream f(fn);
    f << "1\n" << N << ' ' << (qi == Q ? Q : 1) << '\n';
    for (const Road& road: roads) {
        f << road.X<<' '<<road.Y<<' '<<road.L<<' '<<road.Y << '\n'; }
    if (qi == Q) {
        for (u_t i = 0; i < Q; ++i) {
           f << queries[i].C << ' ' <<  queries[i].W << '\n';
        }
    } else {
        f << queries[qi].C << ' ' <<  queries[qi].W << '\n';
    }
    f.close();
}

static int test_case(const vroad_t& roads, const vquery_t& queries)
{
    int rc = 0;
    const u_t N = roads.size() + 1, Q = queries.size();
    if (dbg_flags & 0x4) { save_case("truck-curr.in", roads, queries, Q); }
    vull_t solution, solution_naive;
    bool small = (N < 0x10) && (Q < 0x10);
    if (small)
    {
        Truck p(roads, queries);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Truck p(roads, queries);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        u_t qi = mismatch(solution_naive.begin(), solution_naive.end(),
            solution.begin()).first - solution_naive.begin();
        ull_t sqi = (qi < solution.size() ? solution[qi] : ull_t(-1));
        cerr << "Failed: solution="<< sqi <<
            " != " << solution_naive[qi] << " = solution_naive\n";
        save_case("truck-fail.in", roads, queries, qi);
    } else if (small) {
        cerr << "  N="<<N << ", Q="<<Q;
        for (ull_t x: solution) { cerr << ' ' << x; }
        cerr << '\n';
    }    
    return rc;
}

static void rand_roads(vroad_t& roads, u_t N, u_t Lmax, u_t Cmax, ull_t Amax)
{
    roads.reserve(N - 1);
    vu_t depth_cities;
    depth_cities.push_back(1);
    vu_t pending;
    for (u_t c = 2; c <= N; ++c) { pending.push_back(c); }
    vu_t levels; levels.reserve(Lmax);
    for (u_t level = 1; level <= Lmax; ++level) { levels.push_back(level); }
    while (!pending.empty())
    {
        vu_t next_depth_cities;
        u_t cmin = 1;
        for (u_t c: depth_cities)
        {
            u_t n_children = rand_range(cmin, min<u_t>(Cmax, pending.size()));
            for (u_t a = 0; a < n_children; ++a)
            {
                u_t pi = rand() % pending.size();
                u_t ac = pending[pi];
                pending[pi] = pending.back();
                pending.pop_back();
                next_depth_cities.push_back(ac);
                u_t x = c, y = ac;
                if (rand() % 2 == 1) { swap(x, y); }
                {
                    u_t li = rand() % levels.size();
                    u_t level = levels[li];
                    levels[li] = levels.back();
                    levels.pop_back();
                    roads.push_back(Road(x, y, level, rand_range(1, Amax)));
                }
            }
            cmin = 0;
        }
        swap(depth_cities, next_depth_cities);
    }
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
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t Nmin = strtoul(argv[ai++], 0, 0);
    u_t Nmax = strtoul(argv[ai++], 0, 0);
    u_t Qmin = strtoul(argv[ai++], 0, 0);
    u_t Qmax = strtoul(argv[ai++], 0, 0);
    ull_t Amax = strtoul(argv[ai++], 0, 0);
    u_t Cmax = strtoul(argv[ai++], 0, 0);
    u_t Lmax = strtoul(argv[ai++], 0, 0);
    u_t Wmax = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests=" << n_tests << 
        ", Nmin=" << Nmin <<
        ", Nmax=" << Nmax <<
        ", Qmin=" << Qmin <<
        ", Qmax=" << Qmax <<
        ", Amax=" << Amax <<
        ", Cmax=" << Cmax <<
        ", Lmax=" << Lmax <<
        ", Wmax=" << Wmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(Nmin, Nmax);
        u_t Q = rand_range(Qmin, Qmax);
        vroad_t roads; 
        rand_roads(roads, N, max(Lmax, N - 1), Cmax, Amax);
        vquery_t queries;
        while (queries.size() < Q)
        {
            queries.push_back(Query(rand_range(2, N), rand_range(1, Wmax)));
        }
        rc = test_case(roads, queries);
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
