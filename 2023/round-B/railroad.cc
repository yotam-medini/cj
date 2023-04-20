// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_set>
#include <utility>
#include <set>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef deque<u_t> dqu_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;
typedef set<u_t> setu_t;
typedef unordered_set<u_t> hsetu_t;
typedef vector<hsetu_t> vhsetu_t;

static unsigned dbg_flags;

class Railroad
{
 public:
    Railroad(istream& fi);
    Railroad(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void build_graph_without_line(vvu_t& g, u_t l);
    bool is_graph_connected(const vvu_t& g);
    // void build_graph();
    void build_lines_graph();
    u_t N, L;
    vu_t K;
    vvu_t S;
    u_t solution;
    // vvu_t graph;
};

Railroad::Railroad(istream& fi) : solution(0)
{
    fi >> N >> L;
    K.reserve(L);
    S.reserve(L);
    while (K.size() < L)
    {
        u_t k;
        fi >> k;
        K.push_back(k);
        vu_t s;
        copy_n(istream_iterator<u_t>(fi), k, back_inserter(s));
        S.push_back(s);
    }
}

void Railroad::solve_naive()
{
    for (u_t i = 0; i < L; ++i)
    {
        vvu_t graph;
        build_graph_without_line(graph, i);
        if (!is_graph_connected(graph))
        {
            ++solution;
        }
    }
}

bool Railroad::is_graph_connected(const vvu_t& graph)
{
    // BFS    
    vector<bool> visited(N, false);
    dqu_t q;
    q.push_back(0); 
    visited[0] = true;
    u_t n_visited = 1;
    while (!q.empty())
    {
        u_t station = q.front();
        q.pop_front();
        for (u_t nbr: graph[station])
        {
            if (!visited[nbr])
            {
                q.push_back(nbr);
                visited[nbr] = true;
                ++n_visited;
            }
        }
    }
    bool connected = (n_visited == N);
    return connected;
}

void Railroad::solve()
{
    solve_naive();
}

void Railroad::build_graph_without_line(vvu_t& graph, u_t skip_line)
{
    vhsetu_t station_stations(N, hsetu_t());
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        for (u_t i = (l != skip_line ? 0 : N); i < line.size(); ++i)
        {
            const u_t s0 = line[i] - 1;
            for (u_t j = i + 1; j < line.size(); ++j)
            {
                const u_t s1 = line[j] - 1;
                station_stations[s0].insert(s1);
                station_stations[s1].insert(s0);
            }
        }
    }
    graph.reserve(N);
    for (u_t i = 0; i < N; ++i)
    {
         const hsetu_t& hnbrs = station_stations[i];
         vu_t nbrs(hnbrs.begin(), hnbrs.end());
         sort(nbrs.begin(), nbrs.end());
         graph.push_back(nbrs);
    }
    if (dbg_flags & 0x1) {
        cerr << "graph: {\n";
        for (u_t s = 0; s < L; ++s)
        {
             cerr << "  " << s << ":";
             for (u_t ns: graph[s]) { cerr << ' ' << ns; }
             cerr << '\n';
        }
        cerr << "}\n";
    }
}

void Railroad::build_lines_graph()
{
    vvu_t lgraph;
    vhsetu_t station_lines(N + 1, hsetu_t());
    for (u_t l = 0; l < L; ++l)
    {
        const vu_t& line = S[l];
        for (u_t station: line)
        {
            station_lines[station].insert(l);
        }
    }
    lgraph.reserve(L);
    for (u_t l = 0; l < L; ++l)
    {
        setu_t nbrs;
        const vu_t& line = S[l];
        for (u_t station: line)
        {
            const hsetu_t& slines = station_lines[station];
            for (u_t nl: slines)
            {
                nbrs.insert(nl);
            }
        }
        lgraph.push_back(vu_t(nbrs.begin(), nbrs.end()));
    }
    if (dbg_flags & 0x1) {
        cerr << "Lines graph: {\n";
        for (u_t l = 0; l < L; ++l)
        {
             cerr << "  " << l << ":";
             for (u_t nl: lgraph[l]) { cerr << ' ' << nl; }
             cerr << '\n';
        }
        cerr << "}\n";
    }
}

void Railroad::print_solution(ostream &fo) const
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

    void (Railroad::*psolve)() =
        (naive ? &Railroad::solve_naive : &Railroad::solve);
    if (solve_ver == 1) { psolve = &Railroad::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Railroad railroad(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (railroad.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        railroad.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("railroad-curr.in"); }
    if (small)
    {
        Railroad p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Railroad p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("railroad-fail.in");
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
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
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        rc = test_case(argc, argv);
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
