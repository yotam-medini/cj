// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
// #include <iterator>
// #include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<string> vstr_t;
typedef array<char, 26> ac26_t;
typedef unordered_set<string> hset_s_t;

static unsigned dbg_flags;

class Colliding
{
 public:
    Colliding(istream& fi);
    Colliding(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    ac26_t encoding;
    u_t N;
    vstr_t words;
    bool collision;
};

Colliding::Colliding(istream& fi) : collision(false)
{
    for (u_t i = 0; i < 26; ++i)
    {
        string s;
        fi >> s;
        encoding[i] = s[0];
    }
    fi >> N;
    words.reserve(N);
    while (words.size() < N)
    {
        string s;
        fi >> s;
        words.push_back(s);
    }
}

void Colliding::solve_naive()
{
    hset_s_t seen;
    hset_s_t encoded;
    for (u_t i = 0; (i < N) && !collision; ++i)
    {
        const string& word = words[i];
        if (seen.find(word) == seen.end())
        {
            seen.insert(word);
            const u_t wsz = word.size();
            string word_encoded;
            for (u_t ci = 0; ci < wsz; ++ci)
            {
                const char c = word[ci];
                const char ce = encoding[c - 'A'];
                word_encoded.push_back(ce);
            }
            if (encoded.find(word_encoded) == encoded.end())
            {
                encoded.insert(word_encoded);
            }
            else
            {
                collision = true;
            }
        }
    }
}

void Colliding::solve()
{
    solve_naive();
}

void Colliding::print_solution(ostream &fo) const
{
    fo << ' ' << (collision ? "YES" : "NO");
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

    void (Colliding::*psolve)() =
        (naive ? &Colliding::solve_naive : &Colliding::solve);
    if (solve_ver == 1) { psolve = &Colliding::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Colliding colliding(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (colliding.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        colliding.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("colliding-curr.in"); }
    if (small)
    {
        Colliding p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        Colliding p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("colliding-fail.in");
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
