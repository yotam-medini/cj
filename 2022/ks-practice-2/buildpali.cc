// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
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
typedef array<u_t, 26> au26_t;
typedef vector<au26_t> vau26_t;

static unsigned dbg_flags;

class BuildPali
{
 public:
    BuildPali(istream& fi);
    BuildPali(const string& _s, const vau2_t& _lrs) :
        N(_s.size()), Q(_lrs.size()), S(_s), lrs(_lrs), solution(0) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    u_t get_solution() const { return solution; }
 private:
    u_t N, Q;
    string S;
    vau2_t lrs;
    ull_t solution;
};

BuildPali::BuildPali(istream& fi) : solution(0)
{
    fi >> N >> Q;
    S.reserve(N + 1);
    fi >> S;
    lrs.reserve(Q);
    while (lrs.size() < Q)
    {
        au2_t lr;
        fi >> lr[0] >> lr[1];
        lrs.push_back(lr);
    }
}

void BuildPali::solve_naive()
{
    for (const au2_t& lr: lrs)
    {
        u_t az_count[26];
        fill_n(az_count, 26, 0);
        for (u_t i = lr[0] - 1; i <= lr[1]- 1; ++i)
        {
            ++az_count[S[i] - 'A'];
        }
        u_t n_odd = 0;
        for (u_t i = 0; i < 26; ++i)
        {
            n_odd += (az_count[i]) % 2;
        }
        if (n_odd <= 1)
        {
            ++solution;
        }
    }
}

void BuildPali::solve()
{
    vau26_t az_count;
    az_count.reserve(N + 1);
    static au26_t z26;
    az_count.push_back(z26);
    for (char c: S)
    {
        az_count.push_back(az_count.back());
        az_count.back()[c - 'A']++;
    }
    for (const au2_t& lr: lrs)
    {
        const au26_t& b = az_count[lr[0] - 1];
        const au26_t& e = az_count[lr[1]];
        u_t n_odd = 0;
        for (u_t i = 0; i < 26; ++i)
        {
            const u_t nc = e[i] - b[i];
            n_odd += nc % 2;
        }
        if (n_odd <= 1)
        {
            ++solution;
        }
    }    
}

void BuildPali::print_solution(ostream &fo) const
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

    void (BuildPali::*psolve)() =
        (naive ? &BuildPali::solve_naive : &BuildPali::solve);
    if (solve_ver == 1) { psolve = &BuildPali::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        BuildPali buildpali(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (buildpali.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        buildpali.print_solution(fout);
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

static void save_case(const char* fn, const string& s, const vau2_t& lrs)
{
    const u_t N = s.size();
    const u_t Q = lrs.size();
    ofstream f(fn);
    f << "1\n" << N << ' ' << Q << '\n';
    f << s << '\n';
    for (const au2_t& lr: lrs)
    {
        f << lr[0] << ' ' << lr[1] << '\n';
    }
    f.close();
}

static int test_case(const string& s, const vau2_t& lrs)
{
    int rc = 0;
    u_t solution(-1), solution_naive(-1);
    const u_t N = s.size();
    bool small = (N < 0x10);
    if (dbg_flags & 0x100) { save_case("buildpali-curr.in", s, lrs); }
    if (small)
    {
        BuildPali p{s, lrs};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        BuildPali p(s, lrs);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("buildpali-fail.in", s, lrs);
    }
    if (rc == 0) { cerr << "  ..." <<
        (small ? " (small) " : " (large) ") << s << 
        "[" << lrs[0][0] << ", " << lrs[0][1] << "] --> " <<
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
    const string letters(argv[ai++]);
    cerr << "n_tests=" << n_tests <<
        ", Nmin=" << Nmin << ", Nmax=" << Nmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = rand_range(Nmin, Nmax);
        string s;
        while (s.size() < N)
        {
            s.push_back(letters[rand() % letters.size()]);
        }
        for (u_t l = 1; (rc == 0) && (l <= N); ++l)
        {
            for (u_t r = l; (rc == 0) && (r <= N); ++r)
            {
                vau2_t lrs;
                au2_t lr{l, r};
                lrs.push_back(lr);
                rc = test_case(s, lrs);
            }
        }
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
