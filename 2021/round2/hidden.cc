// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long l_t;
typedef unsigned long ul_t;
typedef unsigned long long ll_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<vu_t> vvu_t;

static unsigned dbg_flags;

static const ull_t MOD_BIG = 1000000000 + 7; // 1000000007

// See: https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
static
l_t extended_gcd(l_t& bezout_x, l_t& bezout_y, const l_t a, const l_t b)
{
    
    l_t s = 0, old_s = 1;
    l_t r = b, old_r = a;
         
    while (r != 0)
    {
        ldiv_t div_result = ldiv(old_r, r);
        l_t quotient = div_result.quot;
        l_t smod = old_s - quotient * s;
        old_r = r;  r = div_result.rem;
        old_s = s;  s = smod;
    }

    bezout_x = old_s;
    bezout_y = (b ? (old_r - old_s * a) / b : 0);
    return ((a == 0) && (b == 0) ? 0 : old_r);
}

static ul_t invmod(ll_t a, ul_t m)
{
    l_t x, y;
    extended_gcd(x, y, a, m);
    ull_t inv = (x + m) % m;
    return inv;
}
    
ul_t invmod_big(ul_t a)
{
    return invmod(a, MOD_BIG);
}

ul_t choode_mod_big(ul_t m, ul_t n)
{
    ull_t c = 1;
    for (; n > 0; --m, --n)
    {
        const ull_t div_n = invmod_big(n);
        c = (c * m) % MOD_BIG;
        c = (c * div_n) % MOD_BIG;
    }
    return c;
}

void permute_to_shown(vu_t& shown, const vu_t& p)
{
    shown.clear();
    shown.reserve(p.size());
    for (u_t i = 0; i < p.size(); ++i)
    {
        u_t n_shown = 0;
        for (u_t j = 0; j <= i; ++j)
        {
            bool j_shown = true;
            for (u_t k = j + 1; j_shown && (k <= i); ++k)
            {
                j_shown = p[k] < p[j];
            }
            if (j_shown)
            {
                ++n_shown;
            }
        }
        shown.push_back(n_shown);
    }
}

class Hidden
{
 public:
    Hidden(istream& fi);
    Hidden(const vu_t& v) : N(v.size()), V(v), solution(0) {}; 
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ul_t get_solution() const { return solution; }
 private:
    ul_t sub_solve(u_t b, u_t e, u_t v);
    u_t N;
    vu_t V;
    ul_t solution;
    vvu_t at;
};

Hidden::Hidden(istream& fi) : solution(0)
{
    fi >> N;    
    copy_n(istream_iterator<u_t>(fi), N, back_inserter(V));
}

void Hidden::solve_naive()
{
    vu_t p; p.reserve(N);
    while (p.size() < N)
    {
        p.push_back(p.size());
    }
    u_t n = 0;
    for (bool more = true; more; more = next_permutation(p.begin(), p.end()))
    {
        vu_t shown;
        permute_to_shown(shown, p);
        if (shown == V)
        {
            ++n;
            if (n == MOD_BIG)
            {
                n = 0;
            }
        }
    }
    solution = n;
}

void Hidden::solve()
{
    at.assign(N, vu_t());
}

void Hidden::print_solution(ostream &fo) const
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

    void (Hidden::*psolve)() =
        (naive ? &Hidden::solve_naive : &Hidden::solve);
    if (solve_ver == 1) { psolve = &Hidden::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Hidden hidden(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (hidden.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        hidden.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static void save_case(const char* fn, const vu_t& shown)
{
    ofstream f(fn);
    f << "1\n" << shown.size() << '\n';
    for (u_t x: shown) { f << x << '\n'; }
    f.close();
}

static int test_case(const vu_t& shown)
{
    int rc = 0;
    const u_t N = shown.size();
    const bool small = N <= 12;
    ul_t solution(-1), solution_naive(-1);
    if (dbg_flags & 0x100) { save_case("hidden-curr.in", shown); }
    if (small)
    {
        Hidden p(shown);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Hidden p(shown);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("hidden-fail.in", shown);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> " <<
        solution << '\n'; }
    return rc;
}

static int test_all(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    if (string(argv[ai]) == string("-debug"))
    {
        dbg_flags = strtoul(argv[ai + 1], nullptr, 0);
        ai += 2;
    }
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    cerr << ", Nmin=" << Nmin << ", Nmax=" << Nmax << '\n';
    for (u_t ti = 0, N = Nmin; (rc == 0) && (N <= Nmax); ++N)
    {
        set<vu_t> shown_combs;
        cerr << "Tested: " << ti << ", N=" << N << '\n';
        vu_t p; p.reserve(N);
        while (p.size() < N)
        {
            p.push_back(p.size());
        }
        for (bool more = true; more;
            more = next_permutation(p.begin(), p.end()))
        {
            vu_t shown;
            permute_to_shown(shown, p);
            shown_combs.insert(shown_combs.end(), shown);
        }
        cerr << "N="<<N << " #(shown combinations)="<<shown_combs.size() <<'\n';
        for (set<vu_t>::const_iterator iter = shown_combs.begin();
            (rc == 0) && (iter != shown_combs.end()); ++iter)
        {
            cerr << "ti=" << ti << '\n'; ++ti;
            const vu_t& shown = *iter;
            rc = test_case(shown);
        }
    }
    return rc;
}

static int analysis(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    const u_t Nmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Nmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "Nmin="<<Nmin << ", Nmax="<<Nmax << '\n';
    for (u_t N = Nmin; N <= Nmax; ++N)
    {
        cout << "\nN=" << N << '\n';
        typedef map<vu_t, ull_t> vcount_t;
        vcount_t vcount;
        vu_t p; p.reserve(N);
        while (p.size() < N)
        {
            p.push_back(p.size());
        }
        for (bool more = true; more;
            more = next_permutation(p.begin(), p.end()))
        {
            vu_t shown;
            permute_to_shown(shown, p);
            vcount_t::iterator iter = vcount.find(shown);
            if (iter == vcount.end())
            {
                iter = vcount.insert(iter, vcount_t::value_type(shown, 0));
            }
            ++(iter->second);
        }
        for (const vcount_t::value_type& vc: vcount)
        {
            const vu_t& v = vc.first;
            cout << "["; const char* sep = "";
            for (u_t x: v) { cout << sep << x; sep = " "; }
            cout << "]: " << vc.second << '\n';
        }
    }
    return rc;
}

int main(int argc, char **argv)
{
    int rc = 0;
    const string cmd0(argc > 1 ? argv[1] : "");
    if (cmd0 == string("test"))
    {
        rc = test_all(argc - 2, argv + 2);
    }
    else if (cmd0 == string("analysis"))
    {
        rc = analysis(argc - 2, argv + 2);
    }
    else
    {
        rc = real_main(argc, argv);
    }
    return rc;
}
