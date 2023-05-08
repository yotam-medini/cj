// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef long long ll_t;
typedef vector<u_t> vu_t;
typedef vector<ll_t> vll_t;
typedef vector<ull_t> vull_t;
typedef unordered_set<u_t> uosetu_t;

static unsigned dbg_flags;

#if 0
class ClientBase
{
 public:
    ClientBase(_dest=false, u_t _i=0, bool) : dest(_dest), i=(i) {}
    bool dest;
    u_t i;
};
class ClientFull : public ClientBase
{
 public:
    Client(ll_t _x=0, bool _dest=false, u_t _i=0) : x(_x), i=(i), dest(_dest) {}
    bool dest;
    u_t i;
};
class PClient : public ClientBase
{
 public:
    Client(ll_t _x=0, bool _dest=false) : x(_x), dest(_dest) {}
    ll_t x;
    bool dest;
    tuple<ll_t, bool> as_tuple() const { return make_tuple(x, dest); }
};
bool operator<(const Client& c0, const Client& c1)
{
    return c0.as_tuple() < c1.as_tuple();
}
#endif

class Client
{
 public:
    Client(ll_t _i=0, bool _dest=false) : i(_i), dest(_dest) {}
    u_t i;
    bool dest;
};

typedef vector<Client> vclient_t;
typedef map<ll_t, vclient_t> x_to_clients_t;

class Immunization
{
 public:
    Immunization(istream& fi);
    Immunization(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void build_clients();
    u_t V, M;
    vll_t P, D;
    vll_t X;
    vull_t solution;
    x_to_clients_t x_to_clients;
};

Immunization::Immunization(istream& fi)
{
    fi >> V >> M;
    copy_n(istream_iterator<ll_t>(fi), V, back_inserter(P));
    copy_n(istream_iterator<ll_t>(fi), V, back_inserter(D));
    copy_n(istream_iterator<ll_t>(fi), M, back_inserter(X));
    solution.reserve(M);
}

void Immunization::solve_naive()
{
    build_clients();
}

void Immunization::solve()
{
    solve_naive();
}

void Immunization::build_clients()
{
    for (u_t i = 0; i < V; ++i)
    {
        typedef pair<ll_t, bool> xd_t;
        for (const xd_t& xd: {xd_t(P[i], false), xd_t(D[i], true)})
        {
            const ll_t x = xd.first;
            auto er = x_to_clients.equal_range(x);
            x_to_clients_t::iterator iter = er.first;
            if (iter == er.second)
            {
                x_to_clients_t::value_type v{x, vclient_t()};
                iter = x_to_clients.insert(iter, v);
            }
            iter->second.push_back(Client(i, xd.second));
        }
    }
}

void Immunization::print_solution(ostream &fo) const
{
    for (u_t y: solution)
    {
        fo << ' ' << y;
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

    void (Immunization::*psolve)() =
        (naive ? &Immunization::solve_naive : &Immunization::solve);
    if (solve_ver == 1) { psolve = &Immunization::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Immunization immunization(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (immunization.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        immunization.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("immunization-curr.in"); }
    if (small)
    {
        Immunization p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Immunization p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("immunization-fail.in");
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
