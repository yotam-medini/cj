// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef array<u_t, 2> au2_t;
typedef vector<au2_t> vau2_t;

static unsigned dbg_flags;

class Genetic
{
 public:
    Genetic(istream& fi);
    Genetic(const string& _A, const string& _B, const vau2_t& _q) :
        A(_A), B(_B), Q(_q.size()), queries(_q) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vu_t& get_solution() const { return solution; }
 private:
    u_t kmp(u_t p, u_t s) const;
    string A, B;
    u_t Q;
    vau2_t queries;
    vu_t solution;
};

Genetic::Genetic(istream& fi)
{
    // copy_n(istream_iterator<u_t>(fi), N, back_inserter(a));
    fi >> A >> B >> Q;
    queries.reserve(Q);
    while (queries.size() < Q)
    {
        u_t p, s;
        fi >> p >> s;
        queries.push_back(au2_t{p, s});
    }
}

void Genetic::solve_naive()
{
    solution.reserve(Q);
    for (const au2_t& ps: queries)
    {
        const u_t p = ps[0], s = ps[1];
        const size_t sfx_pos = B.size() - s;
        u_t m = 0;
        for (u_t i = 0; i < p; ++i)
        {
            for (u_t sz = m + 1; (i + sz <= p) && (sz <= s); ++sz)
            {
                if (A.substr(i, sz) == B.substr(sfx_pos, sz))
                {
                     m = sz;
                }
            }
        }
        solution.push_back(m);
    }
}

void Genetic::solve()
{
    solution.reserve(Q);
    for (const au2_t& ps: queries)
    {
        u_t r = kmp(ps[0], ps[1]);
        solution.push_back(r);
    }
}

u_t Genetic::kmp(u_t p, u_t s) const
{
    u_t r = 0;
    const char* suffix = B.c_str() + B.size() - s;

    // build pi
    vu_t pi(s + 2, 0);
    pi[0] = 0;
    pi[1] = 0;
    size_t k = 0;
    for (size_t q = 2; q <= s; ++q)
    {
        const char pq = suffix[q - 1];
        while ((k > 0) && (suffix[k] != pq))
        {
            k = pi[k];
        }
        if (suffix[k] == pq)
        {
            ++k;
        }
        pi[q] = k;
    }

    // search
    u_t i = 0;
    u_t q = 0;
    while ((i < p) && (r < p))
    {
        while ((q > 0) && (suffix[q] != A[i]))
        {
            q = pi[q];
        }
        if (suffix[q] == A[i])
        {
            ++q;
        }
        ++i;
        if (r < q)
        {
            r = q;
        }
    }

    return r;
}

void Genetic::print_solution(ostream &fo) const
{
    for (u_t s: solution)
    {
        fo << ' ' << s;
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

    void (Genetic::*psolve)() =
        (naive ? &Genetic::solve_naive : &Genetic::solve);
    if (solve_ver == 1) { psolve = &Genetic::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Genetic genetic(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (genetic.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        genetic.print_solution(fout);
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

static void save_case(
    const char* fn,
    const string& A,
    const string& B,
    const vau2_t& queries
)
{
    ofstream f(fn);
    const u_t Q = queries.size();
    f << "1\n" << A << ' ' << B << ' ' << Q;
    for (const au2_t& q: queries)
    {
        f << q[0] << ' ' << q[1] << '\n';
    }
    f.close();
}

static int test_case(const string& A, const string& B, const vau2_t& queries)

{
    int rc = 0;
    const u_t Q = queries.size();
    vu_t solution, solution_naive;
    bool small = (A.size() < 0x100) && (B.size() < 0x100) && (Q < 0x100);
    if (dbg_flags & 0x100) { save_case("genetic-curr.in", A, B, queries); }
    if (small)
    {
        Genetic p(A, B, queries);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Genetic p(A, B, queries);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: inconsistent solutions\n";
        vau2_t bad_query;
        for (u_t i = 0; (i < Q) && bad_query.empty(); ++i)
        {
            if (solution[i] != solution_naive[i])
            {
                bad_query.push_back(queries[i]);
            }
        }
        save_case("genetic-fail.in", A, B, bad_query);
    }
    if (rc == 0) { cerr << "  ...";
        if (small) { cerr << ' ' << A << ' ' << B << ' ' << Q; }
        cerr << (small ? " (small) " : " (large) ") << " -->";
        if (Q <= 8) {
            for (u_t i = 0; i < Q; ++i) { cerr << ' ' << solution[i]; }
        } else {
            for (u_t i = 0; i < 4; ++i) { cerr << ' ' << solution[i]; }
            cerr << " ... ";
            for (u_t i = Q - 4; i < Q; ++i) { cerr << ' ' << solution[i]; }
        }
        cerr << '\n';
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
    const u_t cmax = strtoul(argv[ai++], nullptr, 0);
    const u_t qmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Amin = strtoul(argv[ai++], nullptr, 0);
    const u_t Amax = strtoul(argv[ai++], nullptr, 0);
    const u_t Bmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Bmax = strtoul(argv[ai++], nullptr, 0);
#if 0
    const u_t Pmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Pmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Smin = strtoul(argv[ai++], nullptr, 0);
    const u_t Smax = strtoul(argv[ai++], nullptr, 0);
#endif
    cerr << "n_tests=" << n_tests << ", cmax=" << cmax << ", qmax=" << qmax <<
        ", Amin=" << Amin << ", Amax=" << Amax <<
        ", Bmin=" << Bmin << ", Bmax=" << Bmax <<
        // ", Pmin=" << Pmin << ", Pmax=" << Pmax <<
        // ", Smin=" << Smin << ", Smax=" << Smax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t Asz = rand_range(Amin, Amax);
        u_t Bsz = rand_range(Bmin, Bmax);
        string A, B;
        while (A.size() < Asz)
        {
            A.push_back('a' + rand() % cmax);
        }
        while (B.size() < Bsz)
        {
            B.push_back('a' + rand() % cmax);
        }
        vau2_t queries;
        if (Asz * Bsz <= qmax)
        {
            for (u_t p = 1; (p <= Asz); ++p)
            {
                for (u_t s = 1; (s <= Bsz); ++s)
                {
                    queries.push_back(au2_t{p, s});
                }
            }
        }
        else
        {
            while (queries.size() <= qmax)
            {
                const au2_t query{rand_range(1, Asz), rand_range(1, Bsz)};
                queries.push_back(query);
            }
        }
        rc = test_case(A, B, queries);
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
