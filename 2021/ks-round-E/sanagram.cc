// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <array>
#include <set>
#include <numeric>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;

typedef array<u_t, 2> au2_t; // N, char
typedef set<au2_t> setau2_t;
typedef setau2_t::iterator iter_t;
typedef setau2_t::reverse_iterator riter_t;

static unsigned dbg_flags;

void permutation_first(vu_t &p, u_t n)
{
    p = vu_t(n);
    iota(p.begin(), p.end(), 0);
}

bool permutation_next(vu_t &p)
{
    u_t n = p.size();

    // find the last 'in order'
    int j = n - 1 - 1;
    while ((j >= 0) && (p[j] > p[j + 1]))
    {
        --j;
    }
    bool ret = j >= 0;
    if (ret)
    {
        // Find last > p[j]. Must find since p[j] < p[j+1]
        u_t l;
        for (l = n - 1; p[j] >= p[l]; --l);
        swap(p[j], p[l]);
        reverse(p.begin() + j + 1, p.end());
    }
    return ret;
}

class Sanagram
{
 public:
    Sanagram(istream& fi);
    Sanagram(const string& _s) : S(_s) {}
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const string& get_solution() const { return solution; }
 private:
    void dec_update(setau2_t& setau2, riter_t riter);
    bool even(const setau2_t& setau2) const;
    void rotate_complete(string& candid);
    string S;
    string solution;
    set<au2_t> pending;
    vu_t available_places[26];    
};

Sanagram::Sanagram(istream& fi)
{
    fi >> S;
}

void Sanagram::solve_naive()
{
    const size_t n = S.size();
    vu_t p;
    bool found = false;
    permutation_first(p, n);
    for (permutation_first(p, n); (!found) && permutation_next(p); )
    {
        string a;
        bool good = true;
        for (size_t i = 0; good && (i < n); ++i)
        {
            const char c = S[p[i]];
            a.push_back(c);
            good = (c != S[i]);
        }
        if (good)
        {
            found = true;
            solution = a;
        }
    }
}

void Sanagram::solve()
{
    const u_t N = S.size();
    for (u_t i = 0; i < N; ++i)
    {
        u_t ci = S[i] - 'a';
        available_places[ci].push_back(i);
    }
    for (u_t ci = 0; ci < 26; ++ci)
    {
        const u_t nc = available_places[ci].size();
        if (nc > 0)
        {
            pending.insert(pending.end(), au2_t{nc, ci});
        }
    }
    bool possible = true;
    string candid(S.size(), ' ');
    u_t located = 0;
    while ((pending.size() > 1) && !even(pending))
    {
        riter_t iter = pending.rbegin();
        au2_t pc = *iter;
        riter_t iter_next = iter; ++iter_next;
        au2_t pc_next = *iter_next;
        char c = 'a' + pc[1];
        char c_next = 'a' + pc_next[1];
        u_t pca = (*iter)[1];
        u_t pca_next = (*iter_next)[1];
        u_t ai = available_places[pca].back();
        u_t ai_next = available_places[pca_next].back();
        candid[ai_next] = c;
        candid[ai] = c_next;
        available_places[pca].pop_back();
        available_places[pca_next].pop_back();
        dec_update(pending, iter);
        dec_update(pending, iter_next);
        located += 2;
    }
    possible = located == N;
    if (!possible && even(pending) && (pending.size() > 1))
    {
        rotate_complete(candid);           
        possible = true;
    }
    if (possible)
    {
        solution = candid;
    }
}

bool Sanagram::even(const setau2_t& setau2) const
{
    const au2_t& low = *(setau2.begin());
    const au2_t& high = *(setau2.rbegin());
    bool ret = (low[0] == high[0]);
    return ret;
}

void Sanagram::rotate_complete(string& candid)
{
    vu_t cis;
    for (const au2_t& nc: pending)
    {
        cis.push_back(nc[1]);
    }
    for (u_t cisi = 0, sz = cis.size(); cisi < sz; ++cisi)
    {
        u_t cisi1 = (cisi + 1) % sz;
        char c = 'a' + cis[cisi];
        for (u_t ai: available_places[cis[cisi1]])
        {
            candid[ai] = c;
        }
    }
}

void Sanagram::dec_update(setau2_t& setau2, riter_t riter)
{
    au2_t pc = *riter;
    ++riter;
    iter_t iter(riter.base());
    setau2.erase(iter);
    if (--pc[0] > 0)
    {
        setau2.insert(setau2.end(), pc);
    }
}

void Sanagram::print_solution(ostream &fo) const
{
    static const string impossible("IMPOSSIBLE");
    fo << ' ' << (solution.empty() ? impossible : solution);
}

static int test(int argc, char **argv)
{
    int rc = 0;
    int ai = 0;
    const u_t n_tests = strtoul(argv[ai++], 0, 0);
    const u_t maxlen = strtoul(argv[ai++], 0, 0);
    const u_t maxc = strtoul(argv[ai++], 0, 0);
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "tested: " << ti << '/' << n_tests << '\n';
        size_t sz = rand() % maxlen;
        if (sz == 0) { sz = 1; }
        string s;
        while (s.size() < sz)
        {
            char c = 'a' + (rand() % maxc);
            s.push_back(c);
        }
        Sanagram real(s);
        real.solve();
        const string& solution = real.get_solution();
        if (sz <= 8)
        {
            Sanagram naive(s);
            naive.solve_naive();
            const string& solution_naive = naive.get_solution();
            if (solution_naive.empty() != solution.empty())
            {
                rc = 1;
                cerr << "Inconsistent: sanagram(" << s << "): real: " <<
                    solution << ", naive: " << solution_naive << '\n';
            }
        }
        if (rc == 0)
        {
            cerr << "sanagram( " << s << " ) = " << solution << '\n';
        }
    }
    return rc;
}

int main(int argc, char ** argv)
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
        else if (opt == string("-test"))
        {
            rc = test(argc - (ai + 1), argv + (ai + 1));
            exit(rc);
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

    void (Sanagram::*psolve)() =
        (naive ? &Sanagram::solve_naive : &Sanagram::solve);
    if (solve_ver == 1) { psolve = &Sanagram::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        Sanagram sanagram(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (sanagram.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        sanagram.print_solution(fout);
        fout << "\n";
        fout.flush();
    }

    if (pfi != &cin) { delete pfi; }
    if (pfo != &cout) { delete pfo; }
    return 0;
}
