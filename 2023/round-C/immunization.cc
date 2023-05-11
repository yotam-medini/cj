// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
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

typedef array<vu_t, 2> avu2_t;
typedef map<ll_t, avu2_t> x_to_clients_t;
typedef map<ll_t, vu_t> lltovu_t;

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
    u_t visit(avu2_t& station);
    u_t V, M;
    vll_t P, D;
    vll_t X;
    vull_t solution;
    x_to_clients_t x_to_clients;
    uosetu_t vaccines;
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
    ll_t xcurr = 0;
    for (u_t m = 0; m < M; ++m)
    {
        u_t completed = 0;
        const ll_t dx = X[m];
        const ll_t xnext = xcurr + dx;
        x_to_clients_t::iterator iter;
        if (m == 0)
        {
            iter = x_to_clients.find(0);
            if (iter != x_to_clients.end())
            {
                completed += visit(iter->second);
                if (iter->second[1].empty())
                {
                    x_to_clients.erase(iter);
                }
            }
        }
        iter = x_to_clients.lower_bound(xcurr);
        if (dbg_flags & 0x1) {
            cerr << "m="<<m << ", xcurr="<<xcurr << ", *iter=";
            if (iter != x_to_clients.end()) { cerr << iter->first; } else {
                cerr << "e"; } cerr << '\n'; }
        if (dx > 0)
        {
            x_to_clients_t::iterator iter_next(iter);
            for ( ; (iter != x_to_clients.end()) && (iter->first <= xnext);
                iter = iter_next)
            {
                ++iter_next;
                completed += visit(iter->second);
                if (iter->second[1].empty())
                {
                    x_to_clients.erase(iter);
                }
            }
        }
        else // dx < 0
        {
            x_to_clients_t::reverse_iterator 
                riter = make_reverse_iterator(iter), riter_next(riter);
            //for ( ; (riter != x_to_clients.rend()) && (riter->first >= xnext);
            bool iterate = (riter != x_to_clients.rend());
            for ( ; iterate && (riter->first >= xnext); riter = riter_next)
            {
                ++riter_next;
                iterate = (riter_next != x_to_clients.rend());
                completed += visit(riter->second);
                if (riter->second[1].empty())
                {
                    iter = next(riter).base();
                    x_to_clients_t::iterator iter_prev(iter);
                    --iter_prev;
                    x_to_clients.erase(iter);
                    if (iterate)
                    {
                        iter = iter_prev;
                        ++iter;
                        riter_next = make_reverse_iterator(iter);
                    }
                }
            }
        }
        solution.push_back(completed);
        xcurr = xnext;
    }
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
                x_to_clients_t::value_type v{x, avu2_t()};
                iter = x_to_clients.insert(iter, v);
            }
            u_t zo = (xd.second ? 1 : 0);
            iter->second[zo].push_back(i);
        }
    }
}

u_t Immunization::visit(avu2_t& station)
{
     u_t completed = 0;
     vu_t& xvaccs = station[0];
     vaccines.insert(xvaccs.begin(), xvaccs.end());
     xvaccs.clear(); 
     vu_t& dests = station[1];
     for (u_t di = 0; di < dests.size(); )
     {
         const u_t i = dests[di];
         uosetu_t::iterator iter = vaccines.find(i);
         if (iter != vaccines.end())
         {
             vaccines.erase(iter);
             dests[di] = dests.back();
             dests.pop_back();
             ++completed;
         }
         else
         {
             ++di;
         }
     }
     return completed;
}

void Immunization::solve()
{
    solution.reserve(M);
    lltovu_t xtop;
    lltovu_t::iterator xiter;
    for (u_t i = 0; i < V; ++i)
    {
        const ll_t x = P[i];
        auto er = xtop.equal_range(x);
        xiter = er.first;
        if (xiter == er.second)
        {
            xiter = xtop.insert(xiter, lltovu_t::value_type{x, vu_t()});
        }
        xiter->second.push_back(i);
    }
    lltovu_t available;
    xiter = xtop.find(0);
    if (xiter != xtop.end())
    {
        const vu_t& vp = xiter->second;
        for (u_t v: vp)
        {
            vu_t vd; vd.push_back(v);
            available.insert(lltovu_t::value_type{D[v], vd});
        }
        xtop.erase(xiter);
    }
    ll_t xcurr = 0;
    for (u_t m = 0; m < M; ++m)
    {
        u_t completed = 0;
        ll_t dx = X[m];
        ll_t xnext = xcurr + dx;
        const ll_t xl = (xcurr < xnext ? xcurr : xnext);
        const ll_t xr = (xcurr < xnext ? xnext : xcurr);

        lltovu_t::iterator iter_l = available.lower_bound(xl);
        for (xiter = iter_l; (xiter != available.end()) && (xiter->first <= xr);
            ++xiter)
        {
            completed += xiter->second.size();
        }
        available.erase(iter_l, xiter);

        iter_l = xtop.lower_bound(xl);
        for (xiter = iter_l; (xiter != xtop.end()) && (xiter->first <= xr);
            ++xiter)
        {
            const ll_t x = xiter->first;
            for (u_t v: xiter->second)
            {
                const ll_t y = D[v];
                if ((xl < y) && (y <= xr) && ((x < y) == (dx > 0)))
                {
                    ++completed;
                }
                else
                {
                    auto er = available.equal_range(y);
                    lltovu_t::iterator aiter = er.first;
                    if (aiter == er.second)
                    {
                        lltovu_t::value_type yv{y, vu_t()};
                        aiter = available.insert(aiter, yv);
                    }
                    aiter->second.push_back(v);
                }
            }
        }
        xtop.erase(iter_l, xiter);
        solution.push_back(completed);
        xcurr = xnext;
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
