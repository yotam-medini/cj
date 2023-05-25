// CodeJam
// Author:  Yotam Medini  yotam.medini@gmail.com --

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
// #include <iterator>
#include <map>
// #include <set>

#include <cstdlib>

using namespace std;

typedef unsigned u_t;
typedef unsigned long ul_t;
typedef unsigned long long ull_t;
typedef vector<u_t> vu_t;
typedef vector<string> vs_t;
typedef map<char, u_t> ctou_t;

static unsigned dbg_flags;

class GameSortPart2
{
 public:
    GameSortPart2(istream& fi);
    GameSortPart2(const vu_t&) {}; // TBD for test_case
    void solve_naive();
    void solve();
    void print_solution(ostream&) const;
    ull_t get_solution() const { return 0; }
 private:
    void iterate(vs_t& head, size_t pos);

    void test_candidate(const vs_t& partition);
    string next_perm_after(const string& before, const string& after) const;
    bool next_perm_after_advance(
        const string& before,
        string& head,
        ctou_t& c_cnt) const;
    void complete_tail(string& head, const ctou_t& c_cnt) const;

    void special_cases();
    void special_case_2();
    void special_case_3();
    void special_case_p();
    size_t P;
    string S;
    vs_t solution;
};

GameSortPart2::GameSortPart2(istream& fi)
{
    fi >> P >> S;
}

void GameSortPart2::solve_naive()
{
    vs_t head;
    iterate(head, 0);
}

void GameSortPart2::iterate(vs_t& head, size_t pos)
{
    if (solution.empty())
    {
        if (head.size() == P)
        {
            test_candidate(head);
        }
        else if (head.size() == P - 1)
        {
            head.push_back(S.substr(pos));
            iterate(head, S.size());
            head.pop_back();
        }
        else
        {
            unsigned pending = P - head.size();
            size_t sz_next_max = (S.size() - pos) - (pending - 1);
            for (size_t sz_next = 1;
                solution.empty() && (sz_next <= sz_next_max); ++sz_next)
            {
                const string next = S.substr(pos, sz_next);
                head.push_back(next);
                iterate(head, pos + sz_next);
                head.pop_back();
            }
        }
    }
}

void GameSortPart2::test_candidate(const vs_t& partition)
{
    string curr = partition[0];
    sort(curr.begin(), curr.end());
    bool increasing = true;
    for (size_t i = 1; increasing && (i < P); ++i)
    {
        string next = next_perm_after(curr, partition[i]);
        if (!next.empty())
        {
            curr = next;
        }
        else
        {
            increasing = false;
        }
    }
    if (!increasing) // other player loses
    {
         solution = partition;
    }
}

string GameSortPart2::next_perm_after(
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
    if (found)
    {
        complete_tail(ret, c_cnt);
    }
    else
    { 
        ret = "";
    }
    return ret;
}

bool GameSortPart2::next_perm_after_advance(
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
                        iter = c_cnt.insert(c_cnt.end(),
                            ctou_t::value_type{c, 1});
                    }
                    else
                    {
                        iter = c_cnt.find(c); // need to refresh iter
                        ++(iter->second);
                    }
                    ++iter;
                    if ((found = (iter != c_cnt.end())))
                    {
                        head.push_back(iter->first);
                        --(iter->second); // no need to erase if 0
                        found = true; 
                    }
                }
            }
        }
    }
    return found;
}

void GameSortPart2::complete_tail(string& head, const ctou_t& c_cnt) const
{
    for (const ctou_t::value_type& cc: c_cnt)
    {
        head.insert(head.end(), size_t(cc.second), cc.first);
    }
}

void GameSortPart2::solve()
{
    special_cases();
    size_t n_same = 1;
    for (size_t i = 1; solution.empty() && (i < P); ++i)
    {
        char pre = S[i - 1];
        char c = S[i];
        if (pre > c)
        {
            if (pre > 0)
            {
                solution.push_back(S.substr(0, i - 1));
            }
            solution.push_back(S.substr(i - 1, 1));
            solution.push_back(S.substr(i));
        }
        else if (pre == c)
        {
            ++n_same;
            if (n_same == 3)
            {
                if (i > 3)
                {
                    solution.push_back(S.substr(0, i - 3));
                }
                solution.push_back(S.substr(i - 3, 2));
                solution.push_back(S.substr(i, 1));
                if (i + 1 < P)
                {
                     solution.push_back(S.substr(i + 1));
                }
                solution.push_back(S.substr(i - 1, 1));
                solution.push_back(S.substr(i));

            }
        }
        else
        {
            n_same = 1;
        }
    }
}

void GameSortPart2::special_cases()
{
    if (P == 2)
    {
        special_case_2();
    }
    else if (P == 3)
    {
        special_case_3();
    }
    else if (P == S.size())
    {
        special_case_p();
    }
}

void GameSortPart2::special_case_2()
{
    for (size_t cut = 1; solution.empty() && (cut + 1 < S.size()); ++cut)
    {
        string s0 = S.substr(0, cut);
        string s1 = S.substr(cut);
        sort(s0.begin(), s0.end());
        sort(s1.begin(), s1.end(), greater<char>());
        if (s0 > s1)
        {
            solution.push_back(s0);
            solution.push_back(s1);
        }
    }
}

void GameSortPart2::special_case_3()
{
    for (size_t cut1 = 1; solution.empty() && (cut1 + 2 < S.size()); ++cut1)
    {
        string s0 = S.substr(0, cut1);
        sort(s0.begin(), s0.end());
        for (size_t cut2 = cut1 + 1;
            solution.empty() && (cut2 + 1 < S.size()); ++cut2)
        {
            string s1 = S.substr(cut1, cut2 - cut1);
            s1 = next_perm_after(s0, s1);
            string s2 = S.substr(cut1, cut2 - cut1);
            sort(s2.begin(), s2.end(), greater<char>());
            if ((s0 > s1) || (s1 > s2))
            {
                solution.push_back(s0);
                solution.push_back(s1);
                solution.push_back(s2);
            }
        }        
    }        
}

void GameSortPart2::special_case_p()
{
    string ss(S);
    sort(ss.begin(), ss.end());
    {
        if (ss != S)
        {
            for (size_t pos = 0; pos < P; ++pos)
            {
                solution.push_back(S.substr(pos, 1));
            }
        }
    }
}

void GameSortPart2::print_solution(ostream &fo) const
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

    void (GameSortPart2::*psolve)() =
        (naive ? &GameSortPart2::solve_naive : &GameSortPart2::solve);
    if (solve_ver == 1) { psolve = &GameSortPart2::solve; } // solve1
    ostream &fout = *pfo;
    ul_t fpos_last = pfi->tellg();
    for (unsigned ci = 0; ci < n_cases; ci++)
    {
        GameSortPart2 gamesortp2(*pfi);
        getline(*pfi, ignore);
        if (tellg)
        {
            ul_t fpos = pfi->tellg();
            cerr << "Case (ci+1)="<<(ci+1) << ", tellg=[" <<
                fpos_last << " " << fpos << ") size=" <<
                (fpos - fpos_last) << "\n";
            fpos_last = fpos;
        }

        (gamesortp2.*psolve)();
        fout << "Case #"<< ci+1 << ":";
        gamesortp2.print_solution(fout);
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
    if (dbg_flags & 0x100) { save_case("gamesortp2-curr.in"); }
    if (small)
    {
        GameSortPart2 p{vu_t()};
        p.solve_naive();
        solution_naive = p.get_solution();
    }
    {
        GameSortPart2 p{vu_t()};
        p.solve();
        solution = p.get_solution();
    }
    if (small && (solution != solution_naive))
    {
        rc = 1;
        cerr << "Failed: solution = " << solution << " != " <<
            solution_naive << " = solution_naive\n";
        save_case("gamesortp2-fail.in");
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
