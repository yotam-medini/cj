// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <iterator>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef long long ll_t;
typedef unsigned long long ull_t;
typedef vector<ll_t> vll_t;

typedef vector<u_t> vu_t;

static unsigned dbg_flags;

class Happy
{
 public:
    Happy(istream& fi);
    Happy(const vll_t& _A) : N(_A.size()), A(_A), solution(0) {}; 
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ll_t get_solution() const { return solution; }
 private:
    bool is_happy(size_t b, size_t e) const;
    ll_t happy_subsum(size_t b, size_t e) const;
    ll_t happy_growing_subsums(size_t b, size_t& e, ll_t &last_sum) const;
    u_t N;
    vll_t A;
    ll_t solution;
};

Happy::Happy(istream& fi) : solution(0)
{
    fi >> N;
    copy_n(istream_iterator<ll_t>(fi), N, back_inserter(A));
}

void Happy::solve_naive()
{
    for (size_t b = 0; b < N; ++b)
    {
        ull_t b_subsums = 0;
        for (size_t e = b + 1; e <= N; ++e)
        {
            ll_t subsum = happy_subsum(b, e);
            if ((dbg_flags & 0x2) && (subsum > 0)) {
                cerr << "  subsum["<<b<<','<<e<<")="<<subsum<<'\n'; }
            if (subsum > 0)
            {
                b_subsums += subsum;
            }
        }    
        solution += b_subsums;
        if ((dbg_flags & 0x1) && (b_subsums > 0)) {
            cerr << "b_subsums(b="<<b<<")="<<b_subsums<<'\n'; }
    }
}

class StackNode
{
 public:
    StackNode(ll_t _S=0, size_t _i=0) : S(_S), i(_i) {}
    ll_t S;
    size_t i;
};
bool operator<(const StackNode& sn0, const StackNode& sn1)
{
    return (sn0.S < sn1.S);
}
typedef vector<StackNode> vstacknode_t;


#if 1
void Happy::solve() // See happy.tex (or happy.pdf via pdfLaTeX)
{
    for (size_t i = 0; i < N; )
    {
        vll_t S, W;
        S.push_back(0);
        W.push_back(0);
        vstacknode_t stack;
        // stack.push_back(StackNode(0, size_t(-1)));
        vu_t seg_end;
        for (size_t ib = 0; (i < N) && (S.back() >= 0); ++i, ++ib)
        {
            S.push_back(S.back() + A[i]);
            W.push_back(W.back() + ib*A[i]);
            const StackNode node(S.back(), ib);
            seg_end.push_back(N); // infinity
            while ((!stack.empty()) && (node < stack.back()))
            {
                const size_t ipop = stack.back().i;
                seg_end[ipop + 1] = ib;
                stack.pop_back();
            }
            stack.push_back(node);
        }
        if (S.back() < 0)
        {
            S.pop_back();
            W.pop_back();
            seg_end.pop_back();
        }
        const size_t seg_sz = seg_end.size();
        for (size_t b = 0; b < seg_sz; ++b)
        {
            const size_t e = (seg_end[b] == N ? seg_sz : seg_end[b]);
            if (b < e)
            {
                const ll_t add = e*(S[e] - S[b]) - (W[e] - W[b]);
                solution += add;
            }
        }
    }
}

#else 

void Happy::solve()
{
    for (size_t b = 0; b < N; ++b)
    {
        ll_t subsum = 0;
        for (size_t e = b + 1; (e <= N) && (subsum >= 0); ++e)
        {
            subsum = happy_subsum(b, e);
            if (subsum > 0)
            {
                solution += subsum;
            }
        }    
    }    
}
#endif

bool Happy::is_happy(size_t b, size_t e) const
{
    ll_t curr = 0;
    for (size_t i = b; (i < e) && (curr >= 0); ++i)
    {
        curr += A[i];
    }
    return (curr >= 0);
}

ll_t Happy::happy_subsum(size_t b, size_t e) const
{
    ll_t curr = 0;
    for (size_t i = b; (i < e) && (curr >= 0); ++i)
    {
        curr += A[i];
    }
    return curr;
}

ll_t Happy::happy_growing_subsums(size_t b, size_t& e, ll_t& last_sum) const
{
    ll_t subsum = 0, subsums = 0;
    e = b + 1;
    for (e = b; (e < N) && subsum >= 0; ++e)
    {
        subsum += A[e];
        if (subsum >= 0)
        {
            last_sum = subsum;
            subsums += subsum;
        }
    }
    return subsums;
}

void Happy::print_solution(ostream &fo) const
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

    void (Happy::*psolve)() =
        (naive ? &Happy::solve_naive : &Happy::solve);
    if (solve_ver == 1) { psolve = &Happy::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Happy happy(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (happy.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        happy.print_solution(fout);
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

static void save_case(const char* fn, const vll_t& A)
{
    ofstream f(fn);
    f << "1\n" << A.size() << '\n';
    const char *sep = "";
    for (ll_t x: A) { f << sep << x; sep = " "; }
    f << '\n';
    f.close();
}

static int test_case(const vll_t& A)
{
    int rc = 0;
    const u_t N = A.size();
    ull_t solution(-1), solution_naive(-1);
    bool small = N < 0x10;
    if (dbg_flags & 0x100) { save_case("happy-curr.in", A); }
    if (small)
    {
        Happy p(A);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Happy p(A);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("happy-fail.in", A);
    }
    if (rc == 0) {
        cerr << "[N=" << N  << "] ";
        for (u_t i = 0; (i < N) && (i < 4); ++i) { cerr << ' ' << A[i]; }
        cerr << " ..." <<
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
    const ll_t Amin = strtol(argv[ai++], nullptr, 0);
    const ll_t Amax = strtol(argv[ai++], nullptr, 0);
    cerr << "n_tests = " << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        ", Amin=" << Amin << ", Amax=" << Amax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t N = rand_range(Nmin, Nmax);
        vll_t A; A.reserve(N);
        ll_t delta = (Amax + 1) - Amin;
        while (A.size() < N)
        {
            ll_t x = Amin + (rand() % delta);
            A.push_back(x);
        }
        rc = test_case(A);
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
