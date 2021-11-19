// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

// #include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <utility>
#include <vector>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;

static unsigned dbg_flags;

class Silly
{
 public:
    Silly(istream& fi);
    Silly(u_t _N, const string& _S) : N(_N), S(_S) {}
    void solve_naive();
    void solve1();
    void solve();
    void print_solution(ostream&) const;
    const string& get_solution() const { return solution; }
 private:
    u_t N;
    string S;
    string solution;
};

Silly::Silly(istream& fi)
{
    fi >> N >> S;
}

void Silly::solve_naive()
{
    string x = S;
    for (bool change = true; change; )
    {
        const string x0(x);
        for (u_t n = 0; n < 10; ++n)
        {
            u_t n1 = (n + 1) % 10;
            u_t n2 = (n + 2) % 10;
            char sfrom[2]; 
            sfrom[0] = ('0' + n);
            sfrom[1] = ('0' + n1);
            char cto = '0' + n2;
            for (size_t i = 0; i + 1 < x.size(); ++i)
            {
                if ((x[i] == sfrom[0]) && (x[i + 1] == sfrom[1]))
                {
                    x[i] = cto;
                    for (size_t j = i + 1, j1 = j + 1; j1 < x.size(); j = j1++)
                    {
                        x[j] = x[j + 1];
                    }
                    x.pop_back();
                }
            }
        }
        change = (x != x0);
    }
    solution = x;
}

void Silly::solve1()
{
    list<char> x(S.begin(), S.end());
    ull_t n_loops = 0, n_iter = 0;
    typedef list<char>::iterator iter_t;
    for (bool change = true; change; )
    {
        change = false;
        u_t n_changes = 0;
        for (u_t n = 0; n < 10; ++n)
        {
            u_t n1 = (n + 1) % 10;
            u_t n2 = (n + 2) % 10;
            char sfrom[2]; 
            sfrom[0] = ('0' + n);
            sfrom[1] = ('0' + n1);
            char cto = '0' + n2;
            iter_t iter = x.begin();
            iter_t iter1 = iter;
            if (iter1 != x.end())
            {
                ++iter1;
            }
            for ( ; iter1 != x.end(); iter = iter1++)
            {
                if ((dbg_flags & 0x1) && (n_iter & (n_iter - 1)) == 0) {
                    cerr << "n_iter = " << n_iter << '\n';
                }
                if ((*iter == sfrom[0]) && (*iter1 == sfrom[1]))
                {
                    change = true;
                    *iter = cto;
                    iter_t iter1_next(iter1);
                    ++iter1_next;
                    x.erase(iter1);
                    iter1 = iter1_next;
                    ++n_changes;
                }
                ++n_iter;
            }
        }
        if (dbg_flags & 0x1) {
            cerr << "loop="<<n_loops << ", n_changes="<<n_changes << '\n'; }
        ++n_loops;
    }
    if (dbg_flags & 0x1) { cerr << "n_iter = " << n_iter << '\n'; }
    for (char c: x)
    {
        solution.push_back(c);
    }
}

void Silly::solve()
{
    list<char> x(S.begin(), S.end());
    typedef list<char>::iterator iter_t;
    iter_t iter = x.begin(), iter1(iter);
    if (iter1 != x.end()) { ++iter1; }
    while (iter1 != x.end())
    {
        iter_t iter2(iter1);
        if (iter2 != x.end()) { ++iter2; }
        char c = *iter;
        char c1 = *iter1;
        char c2 = (iter2 != x.end() ? *iter2 : 'X');
        u_t uc = c - '0';
        u_t uc1 = c1 - '0';
        if (((uc + 1) % 10 == uc1) && !((c1 == '0') && (c2 == '1')))
        {
            *iter = '0' + ((uc1 + 1) % 10);
            x.erase(iter1);
            if (iter != x.begin())
            {
                --iter;
            }
            iter1 = iter;
            ++iter1;
        }
        else
        {
            iter = iter1++;
        }
    }
    for (char c: x)
    {
        solution.push_back(c);
    }
}

void Silly::print_solution(ostream &fo) const
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

    void (Silly::*psolve)() =
        (naive ? &Silly::solve_naive : &Silly::solve);
    if (solve_ver == 1) { psolve = &Silly::solve1; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Silly silly(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (silly.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        silly.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}

static int test_case(u_t N, const string& S)
{
    int rc = 0;
    Silly silly(N, S);
    silly.solve();
    const string& sol = silly.get_solution();
    if (N < 20)
    {
        Silly silly_naive(N, S);
        silly_naive.solve_naive();
        const string& sol_naive = silly_naive.get_solution();
        if (sol != sol_naive)
        {
            cerr << "Failed\n"
                "test specific " << N << ' ' << S << '\n';
            rc = 1;
        }
    }
    return rc;
}

static int test_specific(int argc, char ** argv)
{
    u_t N = strtoul(argv[1], 0, 0);
    string S(argv[2]);
    return test_case(N, S);
}

static int test_random(int argc, char ** argv)
{
    int rc = 0;
    int ai = 1;
    u_t n_tests = strtoul(argv[ai++], 0, 0);
    u_t n_min = strtoul(argv[ai++], 0, 0);
    u_t n_max = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        u_t N = n_min + (n_min < n_max ? rand() % (n_max - n_min) : 0);
        string S;
        while (S.size() < N)
        {
            if (rand() % 2)
            {
                S.push_back('0' + (rand() % 10));
            }
            else
            {
                for (char c: "013579")
                {
                    if (S.size() < N)
                    {
                        S.push_back(c);
                    }
                }
            }
        }
        rc = test_case(N, S);
    }
    return rc;
}

static int test_slow()
{
    u_t N = 500000;
    string S; S.reserve(N);
    S.push_back('0');
    while (S.size() < N)
    {
        S.push_back('1');
        S.push_back('3');
        S.push_back('5');
        S.push_back('7');
        S.push_back('9');
    }
    return test_case(N, S);
}


static int test(int argc, char ** argv)
{
    dbg_flags = 0x1;
    int rc = ((argc > 1) && (string(argv[1]) == string("specific"))
        ? test_specific(argc - 1, argv + 1)
        : ((argc > 1) && (string(argv[1]) == string("slow"))
            ? test_slow()
            : test_random(argc, argv)));
    return rc;
}

int main(int argc, char **argv)
{
    int rc = ((argc > 1) && (string(argv[1]) == string("test"))
        ? test(argc - 1, argv + 1)
        : real_main(argc, argv));
    return rc;
}
