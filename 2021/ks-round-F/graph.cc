// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef array<u_t, 2> au2_t;
typedef array<ull_t, 3> aull3_t;
typedef vector<au2_t> vau2_t;
typedef vector<aull3_t> vaull3_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef vector<bool> vb_t;
typedef set<u_t> setu_t;
typedef set<vu_t> setvu_t;
typedef pair<u_t, ull_t> u_ull_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class RE
{
 public:
    RE(u_t _room_mask=0, ull_t _energy=0) :
        rooms_mask(_room_mask), energy(_energy) 
        {}
    u_t rooms_mask;
    ull_t energy;
};

class Graph
{
 public:
    Graph(istream& fi);
    Graph(const vaull3_t& _rooms, const vau2_t& _corridors, ull_t _K) :
        N(_rooms.size()), M(_corridors.size()), K(_K),
        rooms(_rooms), corridors(_corridors),
        solution(0)
        {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    typedef unordered_map<u_t, ull_t> memo_t;
    typedef unordered_map<u_t, bool> memo_move_t;
    void set_adjs();
    bool possible(const vu_t& path);
    ull_t paths_count(const RE& re, vu_t& path);
    bool can_move_to(u_t rmask, u_t r_to);
    u_t N, M, K;
    vaull3_t rooms; // L. R, A
    vau2_t corridors;
    ull_t solution;
    vvu_t adjs;
    memo_t memo; // room-mask used to number of possible paths
    memo_move_t memo_move; // room-mask used to number of possible paths
    
};

Graph::Graph(istream& fi) : solution(0)
{
    fi >> N >> M >> K;
    rooms.reserve(N);
    corridors.reserve(M);
    for (u_t i = 0; i < N; ++i)
    {
        aull3_t room;
        fi >> room[0] >> room[1] >> room[2];
        rooms.push_back(room);
    }
    for (u_t i = 0; i < M; ++i)
    {
        u_t x, y;
        fi >> x >> y;
        if (x > y) { swap(x, y); }
        au2_t corridor{x, y};
        corridors.push_back(corridor);
    }
}

void Graph::solve_naive()
{
    set_adjs();
    setvu_t checked_paths;
    vu_t perm;
    permutation_first(perm, N);
    for (bool more = true; more; more = permutation_next(perm))
    {
        vu_t path;
        ull_t pts = 0;
        bool good = true;
        for (u_t i = 0; good && (i < N) && (pts < K); ++i)
        {
            u_t ri = perm[i];
            const aull3_t& room = rooms[ri];
            good = (i == 0) || ((room[0] <= pts) && (pts <= room[1]));
            path.push_back(ri);
            pts += room[2];
        }
        good = good && (pts == K);
        if (good)
        {
            if (checked_paths.find(path) == checked_paths.end())
            {
                checked_paths.insert(path);
                if (possible(path))
                {
                    ++solution;             
                }
            }
        }
    }
}

void Graph::set_adjs()
{
    adjs = vvu_t(N, vu_t());
    for (const au2_t& corridor: corridors)
    {
         u_t x = corridor[0], y = corridor[1];
         adjs[x].push_back(y);
         adjs[y].push_back(x);
    }
}

bool Graph::possible(const vu_t& path)
{
    bool ok = true;
    vb_t pre_path(N, false);
    pre_path[path[0]] = true;
    for (u_t i = 1, psz = path.size(); ok && (i < psz); ++i)
    {
        pre_path[path[i]] = true;
        u_t rx = path[i - 1];
        u_t ry = path[i];
        // Can we go from rx to ry (using path[0... i]
        vb_t checked(N, false);
        setu_t q;
        q.insert(rx);
        bool ry_found = false;
        while ((!q.empty()) && !ry_found)
        {
            u_t node = *(q.begin());
            q.erase(q.begin());
            for (u_t a: adjs[node])
            {
                ry_found = ry_found || (a == ry);
                if (!ry_found)
                {
                    if (pre_path[a] && !checked[a])
                    {
                        q.insert(a);
                        checked[a] = true;
                    }
                }
            }
        }
        ok = ry_found;
    }
    return ok;
}

void Graph::solve()
{
    set_adjs();
    RE re;
    vu_t path;
    solution = paths_count(re, path);
}

ull_t Graph::paths_count(const RE& re, vu_t& path)
{
    ull_t count = 0;
    const u_t key = re.rooms_mask;
    memo_t::iterator iter = memo.find(key);
    if (iter == memo.end())
    {
        const ull_t e = re.energy;
        if (e == K)
        {
            count = 1;
            if (dbg_flags & 0x1) { cerr << "Path:";
                for (u_t x: path) { cerr << ' ' << x; } cerr << '\n'; }
        }
        else
        {
            const u_t rmask = re.rooms_mask;
            for (u_t r = 0; r < N; ++r)
            {
                const aull3_t& room = rooms[r];
                const u_t rbit = (1u << r);
                if (((rmask & rbit) == 0) && 
                    ((rmask == 0) ||
                        ((room[0] <= e) && (e <= room[1]) &&
                        can_move_to(rmask, r))))
                {
                    RE re_r(rmask | rbit, e + room[2]);
                    path.push_back(r);
                    ull_t count_r = paths_count(re_r, path);
                    path.pop_back();
                    count += count_r;
                }
            }
        }
        memo_t::value_type v{key, count};
        iter = memo.insert(iter, v);
    }
    else
    {
        count = iter->second;
        if (dbg_flags & 0x1) { cerr << "HeadPath:";
            for (u_t x: path) { cerr << ' ' << x; } 
            cerr << " has " << count << " paths\n"; }
           
    }
    return count;
}

bool Graph::can_move_to(u_t rmask, u_t r_to)
{
    bool can = false;
    const u_t key = rmask | (1u << (r_to + 16));
    memo_move_t::iterator iter = memo_move.find(key);
    if (iter == memo_move.end())
    {
        if (rmask == 0)
        {
            can = true; // initual path
        }
        else
        {
            for (u_t r = 0; (r < N) && !can; ++r)
            {
                const u_t rbit = (1u << r);
                if ((rmask & rbit))
                {
                    const vu_t& r_adjs = adjs[r];
                    can = find(r_adjs.begin(), r_adjs.end(), r_to) != 
                        r_adjs.end();
                }
            }
        }
        memo_move_t::value_type v(key, can);
        iter = memo_move.insert(memo_move.end(), v);
    }
    can = iter->second;
    return can;
}

void Graph::print_solution(ostream &fo) const
{
    fo << ' ' << solution;
}

static int test(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
    }
    return rc;
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
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            return rc;
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

    void (Graph::*psolve)() =
        (naive ? &Graph::solve_naive : &Graph::solve);
    if (solve_ver == 1) { psolve = &Graph::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Graph graph(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (graph.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        graph.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(const vaull3_t& rooms, const vau2_t& corridors, ull_t K)
{
    int rc = 0;
    const ull_t N = rooms.size();
    const ull_t M = corridors.size();
    ull_t solution = ull_t(-1), solution_naive = ull_t(-1);
    const bool small = (N <= 8);
    if (small)
    {
        Graph g(rooms, corridors, K);
        g.solve_naive();
        solution_naive = g.get_solution();
    }
    {
        Graph g(rooms, corridors, K);
        g.solve();
        solution = g.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failure: solution = "<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("graph-fail.in");
        f << "1\n" << N << ' ' << M << ' ' << K << '\n';
        for (const aull3_t& room: rooms)
        {
            f << room[0] << ' ' << room[1] << ' ' << room[2] << '\n';
        }
        for (const au2_t& corr: corridors)
        {
            f << corr[0] << ' ' << corr[1] << '\n';
        }
        f.close();
    }
    cerr << "N="<<N << ", M="<<M << ", solution="<<solution << '\n';
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    ull_t Nmin = strtoul(argv[ai++], 0, 0);
    ull_t Nmax = strtoul(argv[ai++], 0, 0);
    ull_t Rmax = strtoul(argv[ai++], 0, 0);
    ull_t Amax = strtoul(argv[ai++], 0, 0);
    cerr << "n_tests="<<n_tests << ", Nmin="<<Nmin << ", Nmax="<<Nmax <<
        ", Rmax="<<Rmax << ", Amax="<<Amax << '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        ull_t N = Nmin + rand() % (Nmax + 1 - Nmin);
        ull_t K = 0;
        vaull3_t rooms;
        while (rooms.size() < N)
        {
            aull3_t room;
            room[0] = 1 + rand() % (Rmax - 1);
            room[1] = room[0] + 1 + (rand() % (Rmax + 1 - room[0]));
            room[2] = 1 + rand() % Amax;
            if (rand() % 2 == 0)
            {
                K += room[2];
            }
            rooms.push_back(room);
        }
        if (K == 0)
        {
            K = rooms[rand() % N][2];
        }
        vau2_t corridors;
        for (u_t i = 0; i < N; ++i)
        {
            for (u_t j = i + 1; j < N; ++j)
            {
                if (rand() % 2 == 0)
                {
                    corridors.push_back(au2_t{i, j});
                }
            }
        }
        test_case(rooms, corridors, K);
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
