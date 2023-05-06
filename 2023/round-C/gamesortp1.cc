// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
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
typedef vector<string> vs_t;
typedef map<char, u_t> ctou_t;

static unsigned dbg_flags;

class GameSortPart1
{
 public:
    GameSortPart1(istream& fi);
    GameSortPart1(const vs_t& _S) : P(_S.size()), S(_S) {};
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    const vs_t& get_solution() const { return solution; }
 private:
    void iterate(vs_t& head);
    bool is_sorted(const vs_t& ss) const;
    string next_perm_after(const string& before, const string& after) const;
    bool next_perm_after_advance(
        const string& before,
        string& head,
        ctou_t& c_cnt) const;
    void complete_tail(string& head, const ctou_t& c_cnt) const;
    u_t P;
    vs_t S;
    vs_t solution;
};

GameSortPart1::GameSortPart1(istream& fi)
{
    fi >> P;
    copy_n(istream_iterator<string>(fi), P, back_inserter(S));
}

void GameSortPart1::solve_naive()
{
    vs_t head; head.reserve(P);
    iterate(head);
}

void GameSortPart1::iterate(vs_t& head)
{
    if (solution.empty())
    {
        if (is_sorted(head))
        {
            if (head.size() == P)
            {
                solution = head;
            }
            else
            {
                string next = S[head.size()];
                sort(next.begin(), next.end());
                head.push_back(next);
                for (bool more = true; more;
                    more = next_permutation(
                        head.back().begin(), head.back().end()))
                {
                    iterate(head);
                }
                head.pop_back();
            }
        }
    }
}

bool GameSortPart1::is_sorted(const vs_t& ss) const
{
    bool ok = true;
    for (u_t i = 1; ok && (i < ss.size()); ++i)
    {
        ok = ss[i - 1] <= ss[i];
    }
    return ok;
}

void GameSortPart1::solve()
{
    string s0(S[0]);
    sort(s0.begin(), s0.end());
    solution.push_back(s0);
    for (u_t i = 1; (i < P) && !solution.empty(); ++i)
    {
        string next = next_perm_after(solution.back(), S[i]);
        if (next.empty())
        {
            solution.clear();
        }
        else
        {
            solution.push_back(next);
        }
    }
}

string GameSortPart1::next_perm_after(
    const string& before, 
    const string& after) const
{
    string ret;
    ctou_t c_cnt;
    for (char c: after)
    {
        auto er = c_cnt.equal_range(c);
        ctou_t::iterator iter = er.first;
        if (iter == er.second)
        {
            iter = c_cnt.insert(iter, ctou_t::value_type{c, 0});
        }
        ++(iter->second);
    }
    bool found = next_perm_after_advance(before, ret, c_cnt);
    if (!found) { ret = ""; }
    return ret;
}

bool GameSortPart1::next_perm_after_advance(
    const string& before,
    string& head,
    ctou_t& c_cnt) const
{
    bool found = true;
    if (head.size() < before.size())
    {
        found = false;
        if (!c_cnt.empty())
        {
            const char bc = before[head.size()];
            ctou_t::iterator iter = c_cnt.lower_bound(bc);
            if ((found = (iter != c_cnt.end())))
            {
                const char c = iter->first;
                head.push_back(c);
                ctou_t::iterator iter_next(iter); ++iter_next;
                bool zeroed = ((--(iter->second)) == 0);
                if (zeroed)
                {
                    c_cnt.erase(iter);
                }
                if (bc == c)
                {
                    found = next_perm_after_advance(before, head, c_cnt);
                }
                if (!found)
                {
                    head.pop_back();
                    if (zeroed)
                    {
                        iter = c_cnt.insert(iter_next,
                            ctou_t::value_type{c, 1});
                    }
                    else
                    {
                        ++(iter->second);
                    }
                    ++iter;
                    if ((found = (iter != c_cnt.end())))
                    {
                        head.push_back(iter->first);
                        --(iter_next->second); // no need to erase if 0
                        found = true; 
                    }
                }
                if (found)
                {
                    complete_tail(head, c_cnt);
                }
            }
        }
    }
    return found;
}

void GameSortPart1::complete_tail(string& head, const ctou_t& c_cnt) const
{
    for (const ctou_t::value_type& cc: c_cnt)
    {
        head.insert(head.end(), size_t(cc.second), cc.first);
    }
}

void GameSortPart1::print_solution(ostream &fo) const
{
    if (solution.empty())
    {
        fo << " IMPOSSIBLE";
    }
    else
    {
        fo << " POSSIBLE";
        char sep = '\n';
        for (const string& s: solution)
        {
            fo << sep << s;
            sep = ' ';
        }
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

    void (GameSortPart1::*psolve)() =
        (naive ? &GameSortPart1::solve_naive : &GameSortPart1::solve);
    if (solve_ver == 1) { psolve = &GameSortPart1::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        GameSortPart1 gamesortp1(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (gamesortp1.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        gamesortp1.print_solution(fout);
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

static void save_case(const char* fn, const vs_t& S)
{
    const u_t P = S.size();
    ofstream f(fn);
    f << "1\n" << P;
    char sep = '\n';
    for (const string& s: S)
    {
        f << sep << s;
        sep = ' ';
    }
    f << '\n';
    f.close();
}

static int test_case(const vs_t& S)
{
    const u_t P = S.size();
    int rc = 0;
    vs_t solution, solution_naive;
    u_t Lmax = 0;
    for (const string& s: S)
    {
        Lmax = max<u_t>(Lmax, s.size());
    }
    bool small = (P <= 3) && (Lmax <= 8);
    if (dbg_flags & 0x100) { save_case("gamesortp1-curr.in", S); }
    if (small)
    {
        GameSortPart1 p(S);
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        GameSortPart1 p(S);
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution.size() << " != " <<
            solution_naive.size() << " = solution_naive\n";
        save_case("gamesortp1-fail.in", S);
    }
    
    if (rc == 0) { cerr << "  P="<< P;
        for (u_t i = 0; i < min<u_t>(P, 3); ++i) { cerr << ' ' << S[i]; }
        cerr  << "  ..." <<
        (small ? " (small) " : " (large) ") << " --> ";
        for (u_t i = 0; i < min<u_t>(P, 3); ++i) { cerr << ' ' << solution[i]; }
        cerr << '\n'; }
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
    const u_t Pmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Pmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Lmin = strtoul(argv[ai++], nullptr, 0);
    const u_t Lmax = strtoul(argv[ai++], nullptr, 0);
    const u_t Cmax = strtoul(argv[ai++], nullptr, 0);
    cerr << "n_tests=" << n_tests <<
        ", Pmin=" << Pmin << ", Pmax=" << Pmax <<
        ", Lmin=" << Lmin << ", Lmax=" << Lmax <<
        ", Cmax=" << Cmax <<
        '\n';
    for (u_t ti = 0; (rc == 0) && (ti < n_tests); ++ti)
    {
        cerr << "Tested: " << ti << '/' << n_tests << '\n';
        const u_t P = rand_range(Pmin, Pmax);
        vs_t S; S.reserve(P);
        while (S.size() < P)
        {
            const u_t L = rand_range(Lmin, Lmax);
            string s; s.reserve(L);
            while (s.size() < L)
            {
                const char c = 'A' + rand() % Cmax;
                s.push_back(c);
            }
            S.push_back(s);
        }
        rc = test_case(S);
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
