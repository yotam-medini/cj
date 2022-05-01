// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>
#include <vector>
#include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<string> vs_t;

static unsigned dbg_flags;

class Letters
{
 public:
    Letters(istream& fi);
    Letters(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void reduce();
    bool singles_ok() const;
    bool is_good(const vu_t& perm);
    void reduced();
    bool is_mono(const string& s) const;
    u_t N;
    vs_t blocks;
    bool possible;
    string solution;
    vs_t monos, non_monos; // reduced
};

Letters::Letters(istream& fi) : possible(false)
{
    fi >> N;
    copy_n(istream_iterator<string>(fi), N, back_inserter(blocks));
}

void Letters::solve_naive()
{
    vu_t perm(N);
    iota(perm.begin(), perm.end(), 0);
    bool more = true;
    for (; more && !possible; more = next_permutation(perm.begin(), perm.end()))
    {
        possible = is_good(perm);
    }
}

void Letters::solve()
{
    possible = singles_ok();
    if (possible)
    {
        reduce();
        if (non_monos.empty())
        {
            string bigmono;
            for (const string& s: monos)
            {
                bigmono += s;
            }
            solution = bigmono;
        }
        else
        {
            u_t c_begin_count[26];
            u_t c_end_count[26];
            u_t c_in_count[26];
            // bool has_mono[26];
            for (u_t i = 0; i < 26; ++i)
            {
                c_begin_count[i] = 0;
                c_end_count[i] = 0;
                c_in_count[i] = 0;
                // has_mono[i] = false;
            }
#if 0
            for (const string& mono: monos)
            {
                has_mono[mono[0] - 'A'] = true;
            }
#endif
            for (const string& block: non_monos)
            {
                const u_t sz = block.size();
                ++c_begin_count[block[0] - 'A'];
                ++c_end_count[block[sz - 1] - 'A'];
                bool cany[26];
                for (u_t ci = 0; ci < 26; ++ci) { cany[ci] = false; }
                for (u_t si = 1; si < sz - 1; ++si)
                {
                    const char c = block[si];
                    cany[c - 'A'] = true;
                }
                for (u_t ci = 0; ci < 26; ++ci) 
                { 
                    if (cany[ci])
                    {
                         ++c_in_count[ci];
                    }
                }
            }
            for (u_t ci = 0; possible && (ci < 26); ++ci)
            {
                possible = possible && (c_begin_count[ci] <= 1);
                possible = possible && (c_end_count[ci] <= 1);
                possible = possible && (c_in_count[ci] <= 1);
            }
            // look for single c_begin_count
            string big;
            u_t ci, mi, ri;
            for (ci = 0; 
                 possible && (ci < 26) && (
                      (c_begin_count[ci] == 0) || (
                          ((c_begin_count[ci] == 1) && (c_end_count[ci] == 1))));
                 ++ci) {}
            possible = possible && (ci < 26);
            char c = 'A' + ci;
            while (possible && !non_monos.empty())
            {
                for (mi = 0; mi < monos.size() && ( monos[mi][0] != c); ++mi)
                {}
                if (mi < monos.size())
                {
                    big += monos[mi];
                    monos[mi] = monos.back();
                    monos.pop_back();
                }
                for (ri = 0; (ri < non_monos.size()) && (non_monos[ri][0] != c);
                     ++ri)
                { }
                possible = possible && (ri < non_monos.size());
                if (possible)
                {
                    big += non_monos[ri];
                    non_monos[ri] = non_monos.back();
                    non_monos.pop_back();
                }
                c = big.back();
            }
            for (mi = 0; mi < monos.size() && ( monos[mi][0] != c); ++mi)
            {}
            if (mi < monos.size())
            {
                big += monos[mi];
                monos[mi] = monos.back();
                monos.pop_back();
            }
            for (const string& mono: monos)
            {
                ci = mono[0] - 'A';
                if (
                   (c_begin_count[ci] > 0) ||
                   (c_end_count[ci] > 0) ||
                   (c_in_count[ci] > 0))
                {
                    possible = false;
                }
                big += mono;
            }
            if (possible)
            {
                solution = big;
            }
        }
    }
}

bool Letters::singles_ok() const
{
    bool ok = true;
    for (u_t i = 0; ok && (i < N); ++i)
    {
        const string& s = blocks[i];
        const u_t sz = s.size();
        for (u_t ci = 0; ok && (ci < 26); ++ci)
        {
            const char c = 'A' + ci;
            u_t n_segs = 0;
            u_t si = 0;
            while ((si < sz) && (n_segs < 2))
            {
                for ( ; (si < sz) && (s[si] != c); ++ si)
                {
                    ;
                }
                const u_t seg_begin = si;
                for ( ; (si < sz) && (s[si] == c); ++ si)
                {
                    ;
                }
                const u_t seg_end = si;
                if (seg_begin < seg_end)
                {
                    ++n_segs;
                }
            }
            ok = (n_segs < 2);
        }
    }
    return ok;
}

bool Letters::is_good(const vu_t& perm)
{
    string s;
    for (u_t i: perm)
    {
        s += blocks[i];
    }
    bool good = true;
    const u_t sz = s.size();
    for (u_t ci = 0; good && (ci < 26); ++ci)
    {
        const char c = 'A' + ci;
        u_t n_segs = 0;
        u_t si = 0;
        while ((si < sz) && (n_segs < 2))
        {
            for ( ; (si < sz) && (s[si] != c); ++ si)
            {
                ;
            }
            const u_t seg_begin = si;
            for ( ; (si < sz) && (s[si] == c); ++ si)
            {
                ;
            }
            const u_t seg_end = si;
            if (seg_begin < seg_end)
            {
                ++n_segs;
            }
        }
        good = (n_segs < 2);
    }
    if (good)
    {
        solution = s;
    }
    return good;
}

void Letters::reduce()
{
    vs_t b(blocks);
    sort(b.begin(), b.end());
    vs_t tmonos;
    for (const string& block: blocks)
    {
        if (is_mono(block))
        {
            tmonos.push_back(block);
        }
        else
        {
            non_monos.push_back(block);
        }
    }
    sort(tmonos.begin(), tmonos.end());
    if (!tmonos.empty())
    {
        monos.push_back(tmonos[0]);
        for (u_t mi = 1; mi < monos.size(); ++mi)
        {
            string& bmono = monos.back();
            const string& mono = tmonos[mi];
            if (bmono[0] == mono[0])
            {
                bmono += mono;
            }
            else
            {
                monos.push_back(mono);
            }
        }
    }
}

bool Letters::is_mono(const string& s) const
{
    bool ret = true;
    char c0 = s[0];
    for (char c: s)
    {
        ret = ret && (c == c0);
    }
    return ret;
}

void Letters::print_solution(ostream &fo) const
{
    if (possible)
    {
        fo << ' ' << solution;
    }
    else
    {
        fo << " IMPOSSIBLE";
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

    void (Letters::*psolve)() =
        (naive ? &Letters::solve_naive : &Letters::solve);
    if (solve_ver == 1) { psolve = &Letters::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Letters letters(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (letters.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        letters.print_solution(fout);
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

static int test_case(int argc, char ** argv)
{
    int rc = rand_range(0, 1);
    ull_t solution(-1), solution_naive(-1);
    bool small = rc == 0;
    if (small)
    {
        Letters p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Letters p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution="<<solution << " != " <<
            solution_naive << " = solution_naive\n";
        ofstream f("letters-fail.in");
        f << "1\n";
        f.close();
    }
    return rc;
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 0;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
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
