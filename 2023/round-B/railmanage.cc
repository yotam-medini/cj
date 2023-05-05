// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<ull_t> vull_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class RailManage
{
 public:
    RailManage(istream& fi);
    RailManage(const vu_t& _D, const vull_t& _C) :
        N(_D.size()), D(_D), C(_C), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return solution; }
 private:
    ull_t play(const vu_t& order) const;
    void solve_cycles();
    u_t N;
    vu_t D;
    vull_t C;
    ull_t solution;
    vu_t in_count;
    vull_t available;
};

RailManage::RailManage(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(D));
    copy_n(istream_iterator<ull_t>(fi), N, back_inserter(C));
}

void RailManage::solve_naive()
{
    vu_t order; order.reserve(N);
    while (order.size() < N)
    {
        order.push_back(order.size());
    }
    solution = ull_t(-1);
    ull_t loops = 0;
    for (bool more = true; more;
        more = next_permutation(order.begin(), order.end()), ++loops)
    {
        if ((dbg_flags & 0x2) && ((loops & (loops -1)) == 0)) {
            cerr << "loops =" << loops << '\n'; }
        ull_t r = play(order);
        if (solution > r)
        {
            solution = r;
        }
    }
}

ull_t RailManage::play(const vu_t& order) const
{
    ull_t r = 0;
    vull_t avail(N, 0);
    for (u_t oi = 0; oi < N; ++oi)
    {
        const u_t station = order[oi];
        const ull_t a = avail[station];
        const ull_t c = C[station];
        if (c > a)
        {
            ull_t need = c - a;
            r += need;
        }
        u_t dest = D[station] - 1;
        avail[dest] += c;
    }
    return r;
}

void RailManage::solve()
{
    in_count.assign(N, 0);
    vau2_t in_count_v(N, au2_t{0, 0});
    for (u_t v = 0; v < N; ++v)
    {
        const u_t d = D[v] - 1;
        ++in_count[d];
        ++in_count_v[d][0];
        in_count_v[v][1] = v;
    }
    sort(in_count_v.begin(), in_count_v.end());

    available.assign(N, 0);
    u_t n_non_cycle = 0;
        if (dbg_flags & 0x2) { for (u_t j = 0; j < N; ++j) {
           cerr << ' ' << in_count[j]; } cerr << '\n'; }
    for (u_t i = 0; i < N; ++i)
    {
        const u_t v = in_count_v[i][1];
        if (in_count[v] == 0)
        {
            ++n_non_cycle;
            const ull_t Cv = C[v];
            if (Cv > available[v])
            {
                const u_t need = Cv - available[v];
                solution += need;
                available[v] = Cv;
            }
            const u_t d = D[v] - 1;
            available[v] -= Cv;
            available[d] += Cv;
            --in_count[d];
        }
        if (dbg_flags & 0x2) { for (u_t j = 0; j < N; ++j) {
           cerr << ' ' << in_count[j]; } cerr << '\n'; }
    }
    if (dbg_flags & 0x1) {
       cerr << "n_non_cycle=" << n_non_cycle << ", curr=" << solution << '\n'; }
    solve_cycles();
}

void RailManage::solve_cycles()
{
    for (u_t i = 0; i < N; ++i)
    {
        if (in_count[i] != 0)
        {
            if (in_count[i] != 1) {
               cerr << "ERROR in_count["<<i<<"]=" << in_count[i] << '\n'; 
               exit (1); }
            vu_t cycle;
            cycle.push_back(i);
            for (u_t j = D[i] - 1; j != cycle[0]; j = D[j] - 1)
            {
                if (in_count[j] != 1) { 
                   cerr << "ERROR in_count["<<j<<"j=" << in_count[j] << '\n'; 
                   exit (1); }
                cycle.push_back(j);
            }
            const u_t sz = cycle.size();
            ull_t base_need = 0; // assumin "no start in cycle"
            ull_t min_start_need = ull_t(-1);
            for (u_t j = 0, pj = sz - 1; j < sz; pj = j++)
            {
                const u_t pv = cycle[pj];
                const u_t v = cycle[j];
                in_count[v] = 0;
                const ull_t Cv = C[v];
                const ull_t av = available[v];
                ull_t start_extra_need = 0;
                if (Cv > av)
                {
                    ull_t available_in_cyle = av + C[pv];
                    ull_t need = 0;
                    if (Cv > available_in_cyle)
                    {
                        need = Cv - available_in_cyle;
                        base_need += need;
                    }
                    start_extra_need = (Cv - av) - need;
                }
                if (min_start_need > start_extra_need)
                {
                    min_start_need = start_extra_need;
                }
            }
            solution += base_need + min_start_need;
        }
    }
}

void RailManage::print_solution(ostream &fo) const
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

    void (RailManage::*psolve)() =
        (naive ? &RailManage::solve_naive : &RailManage::solve);
    if (solve_ver == 1) { psolve = &RailManage::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        RailManage railmanage(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (railmanage.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        railmanage.print_solution(fout);
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

static void save_case(const char* fn, const vu_t& D, const vull_t& C)
{
    ofstream f(fn);
    const u_t N = D.size();
    f << "1\n" << N;
    char sep = '\n';
    for (u_t d: D) { f << sep << d; sep = ' '; }
    sep = '\n';
    for (ull_t c: C) { f << sep << c; sep = ' '; }
    f << '\n';
    f.close();
}

static int test_case(const vu_t& D, const vull_t& C)
{
    int rc = 0;
    const u_t N = D.size();
    ull_t solution(-1), solution_naive(-1);
    bool small = (N <= 8);
    if (dbg_flags & 0x100) { save_case("railmanage-curr.in", D, C); }
    if (small)
    {
        RailManage p(D, C);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        RailManage p(D, C);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("railmanage-fail.in", D, C);
    }
    if (rc == 0) { cerr << "  ... N=" << N << " ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
    return rc;
}

static void rand_dest(vu_t& D, u_t N)
{
    D.reserve(N); D.clear();
    vu_t d_avail; d_avail.reserve(N);
    while (d_avail.size() < N)
    {
        d_avail.push_back(d_avail.size() + 1);
    }
    u_t i = 0;
    while (d_avail.size() > 2)
    {
        u_t d1 = D.size() + 1;
        while (d1 == D.size() + 1)
        {
            i = rand() % d_avail.size();
            d1 = d_avail[i];
        }
        D.push_back(d1);
        d_avail[i] = d_avail.back();
        d_avail.pop_back();
    }
    i = rand() % 2;
    if ((d_avail[i] == N - 1) || (d_avail[1 - 1] == N - 0))
    {
        i = 1 - i;
    }
    D.push_back(d_avail[i]);
    D.push_back(d_avail[1 - i]);
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
    const ull_t Cmin = strtoul(argv[ai++], nullptr, 0);
    const ull_t Cmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Cmin=" << Cmin << ", Cmax=" << Cmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t N = rand_range(Nmin, Nmax);
        vu_t D;
        rand_dest(D, N);
        vull_t C; C.reserve(N);
        while (C.size() < N)
        {
            C.push_back(rand_range(Cmin, Cmax));
        }
        rc = test_case(D, C);
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
